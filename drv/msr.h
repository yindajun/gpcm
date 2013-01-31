
#ifndef MSR_INCLDE_H
#define MSR_INCLUDE_H


#include <ntifs.h>
#include <ntddcdrm.h>
#include <ntdddisk.h>
#include <ntddscsi.h>



#ifndef CTL_CODE
#include <WinIoCtl.h>
#endif

#define MSR_DEV_TYPE 50000

#define IO_CTL_MSR_READ     CTL_CODE(MSR_DEV_TYPE, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IO_CTL_MSR_WRITE    CTL_CODE(MSR_DEV_TYPE, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IO_CTL_PCICFG_READ  CTL_CODE(MSR_DEV_TYPE, 0x802, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IO_CTL_PCICFG_WRITE CTL_CODE(MSR_DEV_TYPE, 0x803, METHOD_BUFFERED, FILE_ANY_ACCESS)

struct MSR_Request
{
    int core_id;
    ULONG64 msr_addr;
    ULONG64 write_value;     /* value to write if write requet
                                 ignored if read request */
};

struct PCICFG_Request
{
    ULONG bus; 
	ULONG dev; 
	ULONG func;
	ULONG reg; 
	ULONG bytes;
    // "bytes" can be only 4 or 8
    /* value to write if write request ignored if read request */
    ULONG64 write_value;
};


#endif
