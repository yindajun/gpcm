/*
 * instdrv.c
 *
 * Routines to install driver.
 *
 * Copyright (c) 2013 Eric.  
 * All rights reserved.
 *
 *
 * Author:
 *    eric.vinno@hotmail.com
 */

/*
 * $Date: 2013/1/30 $
 * $Revision: 0.10 $
 */
#include <windows.h>
#include <tchar.h>

#include "instdrv.h"
#include "resource.h"


#pragma comment (lib, "Advapi32.lib")


SC_HANDLE g_schManager;

extern HWND g_hwndDlg;



int InstDrvInit()
{
	TCHAR buff[MAX_PATH];
	DWORD dwErrCode;
	LPTSTR lpErrMsg;

	g_schManager = OpenSCManager(NULL,
						 		 NULL,
								 SC_MANAGER_CREATE_SERVICE|
								 	SERVICE_START|
									SERVICE_STOP|
									DELETE|
									SC_MANAGER_ENUMERATE_SERVICE);

	if(g_schManager == NULL) {
		InstDrvGetErrMsg(&dwErrCode, (LPTSTR)&lpErrMsg);
		wsprintf(buff, "[InstDrv]OpenSCManager failed(0x%4x)!%s", 
					GetLastError(), lpErrMsg);

		SetDlgItemText(g_hwndDlg, IDC_STATE, buff);
		return -1;
	}

	return 0;
}


int InstDrvInstall(LPTSTR ServiceName, LPTSTR DriverPath)
{
	// install the driver
	TCHAR buff[MAX_PATH];
	HANDLE hFile;
	SC_HANDLE schService;
	
	DWORD dwErrCode;
	LPTSTR lpErrMsg;

	
	hFile = CreateFile(DriverPath,
					   GENERIC_READ, 
					   FILE_SHARE_READ|FILE_SHARE_WRITE, 
					   NULL, 
					   OPEN_EXISTING, 
					   0, 
					   0);
	
	if(hFile == INVALID_HANDLE_VALUE) {
		SetDlgItemText(g_hwndDlg, IDC_STATE, "[InstDrv]The file driverpath assigned do not exist!");
		return -1;
	}

	CloseHandle(hFile);

	schService = CreateService(g_schManager, 
							   ServiceName, 
							   ServiceName, 
							   //SERVICE_START|SERVICE_STOP|DELETE, 
							   SERVICE_ALL_ACCESS,
							   SERVICE_KERNEL_DRIVER, 
							   SERVICE_DEMAND_START, 
							   SERVICE_ERROR_NORMAL, 
							   DriverPath, 
							   0, 
							   0,
							   0,
							   0,
							   0);

	if(schService == NULL) {
		InstDrvGetErrMsg(&dwErrCode, (LPTSTR)&lpErrMsg);
		wsprintf(buff, "[InstDrv]CreateService failed(0x%4x)!%s", GetLastError(), lpErrMsg);
		SetDlgItemText(g_hwndDlg, IDC_STATE, buff);
		return -1;
	}

	
	CloseServiceHandle(schService);

	return 0;
}

SC_HANDLE InstDrvGetService(LPTSTR ServiceName, LPTSTR DriverPath)
{
	TCHAR buff[MAX_PATH];
	SC_HANDLE schService;
	DWORD dwBytesRead;

	DWORD dwErrCode;
	LPTSTR lpErrMsg;
	LPQUERY_SERVICE_CONFIG lpqscBuf;

	DWORD iSrv;
	
	if(ServiceName != NULL) {
		// verify this two arguments
		schService = OpenService(g_schManager, 
								 ServiceName, 
								 SERVICE_START|
								 	SERVICE_QUERY_CONFIG|
									SERVICE_STOP|DELETE);

		if(schService == NULL) {
			InstDrvGetErrMsg(&dwErrCode, (LPTSTR)&lpErrMsg);

			wsprintf(buff, "[InstDrv]OpenService failed(0x%4x)!%s", 
						GetLastError(), lpErrMsg);
			SetDlgItemText(g_hwndDlg, IDC_STATE, buff);
			return NULL;
		}
		
		lpqscBuf = (LPQUERY_SERVICE_CONFIG)LocalAlloc(LPTR, 4096);

		if(DriverPath != NULL) {
			QueryServiceConfig(schService,
							   lpqscBuf, 
							   4096, 
							   &dwBytesRead);
			
			_tcsupr(lpqscBuf->lpBinaryPathName);
			_tcsupr(DriverPath);

			if(lstrcmp(lpqscBuf->lpBinaryPathName + 4, DriverPath) != 0) {
				LocalFree(lpqscBuf);
				// the driver path does not match the service name
				SetDlgItemText(g_hwndDlg, IDC_STATE, 
						"[InstDrv]The dirver path does not match the service name");

				CloseServiceHandle(schService);
				return NULL;
			}
		
		}
		LocalFree(lpqscBuf);
	}

	if(ServiceName == NULL) {
		
		DWORD cnServices = 0;
		DWORD hNextItem = 0;
		LPENUM_SERVICE_STATUS lpSrvStatus;

		if (DriverPath == NULL) {
			// no name and no path
			SetDlgItemText(g_hwndDlg, IDC_STATE, 
					"[InstDrv]Please fill in the service name (may optional driver path)");
			return NULL;
		}
		else {
			ENUM_SERVICE_STATUS ssStatus = {0};
			// only get the path
			EnumServicesStatus(g_schManager,
							   SERVICE_DRIVER, 
							   SERVICE_STATE_ALL, 
							   &ssStatus, 
							   sizeof(ssStatus), 
							   &dwBytesRead, 
							   &cnServices, 
							   0);

			lpSrvStatus = (LPENUM_SERVICE_STATUS)GlobalAlloc(GMEM_FIXED,
										    				 dwBytesRead);

			EnumServicesStatus(g_schManager, 
							   SERVICE_KERNEL_DRIVER, 
							   SERVICE_STATE_ALL, 
							   lpSrvStatus, 
							   dwBytesRead, 
							   &dwBytesRead, 
							   &cnServices, 
							   &hNextItem);
			
			//LPQUERY_SERVICE_CONFIG lpqscBuf;
			lpqscBuf = (LPQUERY_SERVICE_CONFIG)LocalAlloc(LPTR, 4096);

			for(iSrv = 0; iSrv < cnServices; iSrv++) {
				schService = OpenService(g_schManager,
										(lpSrvStatus + iSrv)->lpServiceName,
										SERVICE_START|
											SERVICE_QUERY_CONFIG|
											SERVICE_STOP|DELETE);

				QueryServiceConfig(schService, lpqscBuf, 4096, &dwBytesRead);
				
				_tcsupr(lpqscBuf->lpBinaryPathName);
				_tcsupr(DriverPath);

				if(lstrcmp( lpqscBuf->lpBinaryPathName + 4, DriverPath) == 0 )
				{
					LocalFree(lpqscBuf);
					return schService;
				}

				CloseServiceHandle(schService);

			}

			LocalFree(lpqscBuf);


			SetDlgItemText(g_hwndDlg, IDC_STATE, 
					"[InstDrv]Cannot find the service specified by the driver path");
			return NULL;
		}

		GlobalFree(lpSrvStatus);

	}

	return schService;
	
}


int InstDrvStart(LPTSTR ServiceName, LPTSTR DriverPath)
{
	TCHAR buff[MAX_PATH];
	
	DWORD dwErrCode;
	LPTSTR lpErrMsg;

	SC_HANDLE schService = InstDrvGetService(ServiceName, DriverPath);

	if(schService == NULL)
		return -1;

	if(!StartService(schService, 0, NULL)) {
		InstDrvGetErrMsg(&dwErrCode, (LPTSTR)&lpErrMsg);

		wsprintf(buff, "[InstDrv]StartService failed(0x%4x)!%s", 
				GetLastError(), lpErrMsg);

		SetDlgItemText(g_hwndDlg, IDC_STATE, buff);
		
		return -1;
	}

	CloseServiceHandle(schService);

	return 0;
}

int InstDrvStop(LPTSTR ServiceName, LPTSTR DriverPath)
{
	TCHAR buff[MAX_PATH];
	SERVICE_STATUS ssStatus;
	
	DWORD dwErrCode;
	LPTSTR lpErrMsg;

	SC_HANDLE schService = InstDrvGetService(ServiceName, DriverPath);
	
	if(schService == NULL)
		return -1;

	if(!ControlService(schService, SERVICE_CONTROL_STOP, &ssStatus)) {
		InstDrvGetErrMsg(&dwErrCode, (LPTSTR)&lpErrMsg);

		wsprintf(buff, "[InstDrv]ControlService failed(0x%4x)!%s", 
				GetLastError(), lpErrMsg);

		SetDlgItemText(g_hwndDlg, IDC_STATE, buff);

		return -1;
	}

	CloseServiceHandle(schService);

	return 0;
}

int InstDrvRemove(LPTSTR ServiceName, LPTSTR DriverPath)
{
	TCHAR buff[MAX_PATH];

	DWORD dwErrCode;
	LPTSTR lpErrMsg;

	SC_HANDLE schService = InstDrvGetService(ServiceName, DriverPath);

	if(schService == NULL)
		return -1;

	if(!DeleteService(schService)) {
		InstDrvGetErrMsg(&dwErrCode, (LPTSTR)&lpErrMsg);
		wsprintf(buff, "[InstDrv]DeleteService failed(0x%4x)!%s", 
					GetLastError(), lpErrMsg);		
		
		SetDlgItemText(g_hwndDlg, IDC_STATE, buff);
		return -1;
	}

	CloseServiceHandle(schService);
	
	CloseServiceHandle(g_schManager);

	return 0;
}

void InstDrvGetErrMsg(LPDWORD ErrorCode, LPTSTR  lppErrMsg)
{
	*ErrorCode = GetLastError();

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
				  NULL, 
				  *ErrorCode, 
				  MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), 
				  (LPTSTR)lppErrMsg, 
				  0x20, 
				  NULL);
}

