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


