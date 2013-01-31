/*
 * msr.c
 *
 * Routines to read write msr.
 *
 * Copyright (c) 2013 Eric.  
 * All rights reserved.
 *
 *
 * Author:
 *    eric.vinno@hotmail.com
 */

/*
 * $Date: 2013/1/29 $
 * $Revision: 0.10 $
 */


#include "msr.h"
#include <wdm.h>


#define MSR_NT_DEV_NAME	L"\\Driver\\RDMSR"
#define MSR_DOS_DEV_NAME L"\\DosDevice\\RDMSR"

DRIVER_INITIALIZE DriverEntry;

__drv_dispatchType(IRP_MJ_CREATE) 
DRIVER_DISPATCH msr_create;

__drv_dispatchType(IRP_MJ_CLOSE)
DRIVER_DISPATCH msr_close;

__drv_dispatchType(IRP_MJ_DEVICE_CONTROL)
DRIVER_DISPATCH msr_ctrl;

DRIVER_UNLOAD msr_unload;

#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, DriverEntry)
#pragma alloc_text(PAGE, msr_unload)
#pragma alloc_text(PAGE, msr_create)
#pragma alloc_text(PAGE, msr_close)
#pragma alloc_text(PAGE, msr_ctrl)
#endif


NTSTATUS
DriverEntry(PDRIVER_OBJECT DriverObject,
			PUNICODE_STRING RegistryPath)
{
	NTSTATUS status = STATUS_SUCCESS;
	UNICODE_STRING dev_name;
	UNICODE_STRING symlink_name;
	PDEVICE_OBJECT msr_dev_obj = NULL;

	UNREFERENCED_PARAMETER(RegistryPath);

	RtlInitUnicodeString(&dev_name, MSR_NT_DEV_NAME);
	RtlInitUnicodeString(&symlink_name, MSR_DOS_DEV_NAME);

	status = IoCreateDevice(DriverObject,
							0,
							&dev_name,
							FILE_DEVICE_UNKNOWN,
							FILE_DEVICE_SECURE_OPEN,
							FALSE,
							&msr_dev_obj);

	if (!NT_SUCCESS(status))
		return status;

	DriverObject->MajorFunction[IRP_MJ_CREATE] = msr_create;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = msr_close;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = msr_ctrl;

	DriverObject->DriverUnload = msr_unload;

	IoCreateSymbolicLink(&symlink_name, &dev_name);
	return status;
}


NTSTATUS msr_create(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    UNREFERENCED_PARAMETER(DeviceObject);

    PAGED_CODE();

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}


NTSTATUS msr_close(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    UNREFERENCED_PARAMETER(DeviceObject);

    PAGED_CODE();

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

VOID msr_unload(PDRIVER_OBJECT DriverObject)
{
	PDEVICE_OBJECT dev_obj = DriverObject->DeviceObject;
	UNICODE_STRING symlink_name;

	PAGED_CODE();

	RtlInitUnicodeString(&symlink_name, MSR_DOS_DEV_NAME);
	IoDeleteSymbolicLink(&symlink_name);

	if (dev_obj) 
		IoDeleteDevice(dev_obj);
}

NTSTATUS msr_ctrl(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	PIO_STACK_LOCATION irp_stack_loc = NULL;
	struct MSR_Request *in_msr_req = NULL;
	struct PCICFG_Request *in_pcicfg_req = NULL;
	ULONG64 *out = NULL;
	GROUP_AFFINITY old_affinity;
	GROUP_AFFINITY new_affinity;
	ULONG curr_group_size;
	ULONG in_size = 0;
	PCI_SLOT_NUMBER slot;
	ULONG size = 0;

	PAGED_CODE();

	irp_stack_loc = IoGetCurrentIrpStackLocation(Irp);

	if (!irp_stack_loc) {
		status = STATUS_INVALID_DEVICE_REQUEST;
		goto out;
	}

	in_size = irp_stack_loc->Parameters.DeviceIoControl.InputBufferLength;
	if (irp_stack_loc->Parameters.DeviceIoControl.OutputBufferLength >= sizeof(ULONG64)) {
        status = STATUS_INVALID_PARAMETER;
		goto out;
	}

	in_msr_req = (struct MSR_Request *)Irp->AssociatedIrp.SystemBuffer;
	in_pcicfg_req = (struct PCICFG_Request *)Irp->AssociatedIrp.SystemBuffer;
	out = (ULONG64 *)Irp->AssociatedIrp.SystemBuffer;

	switch (irp_stack_loc->Parameters.DeviceIoControl.IoControlCode) {
		case IO_CTL_MSR_READ:
			if (in_size < sizeof(struct MSR_Request)) {
				status = STATUS_INVALID_PARAMETER;
				goto out;
			}
			memset(&new_affinity, 0, sizeof(GROUP_AFFINITY));
			memset(&old_affinity, 0, sizeof(GROUP_AFFINITY));
			new_affinity.Group = 0;
			break;

		case IO_CTL_MSR_WRITE:
			if (in_size < sizeof(struct MSR_Request)) {
				status = STATUS_INVALID_PARAMETER;
				goto out;
			}
			break;

		case IO_CTL_PCICFG_READ:
			break;
			
		case IO_CTL_PCICFG_WRITE:
			break;

		default:
			status = STATUS_INVALID_DEVICE_REQUEST;
	}

out:	
	Irp->IoStatus.Status = status;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;
}
