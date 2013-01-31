
#ifndef INST_DRV_H
#define INST_DRV_H

int InstDrvInit();

int InstDrvInstall(LPTSTR ServiceName, LPTSTR DriverPath);

SC_HANDLE InstDrvGetService(LPTSTR ServiceName, LPTSTR DriverPaht);

int InstDrvStart(LPTSTR ServiceName, LPTSTR DriverPath);
int InstDrvStop(LPTSTR ServiceName, LPTSTR DriverPath);


int InstDrvRemove(LPTSTR ServiceName, LPTSTR DriverPath);


void InstDrvGetErrMsg(LPDWORD ErrorCode, LPTSTR  lppErrMsg);

#endif
