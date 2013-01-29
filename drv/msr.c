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

#ifde ALLOC_PRAGMA
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
							&msr_dev_obj);

	if (!NT_SUCCESS(status))
		return status;

	DriverObject->MajorFuncion[IRP_MJ_CREATE] = msr_create;
	DriverObject->MajorFuncion[IRP_MJ_CLOSE] = msr_close;
	DriverObject->MajorFuncion[IRP_MJ_DEVICE_CONTROL] = msr_ctrl;

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
