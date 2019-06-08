#ifndef PROCESS_HANDLE_H
#define PROCESS_HANDLE_H
#include "main.h"

int getProcID(std::string processName);

HANDLE getProcessHandle(int processid, long desiredAccess = PROCESS_ALL_ACCESS);

DWORD getProcessModuleAddr(int processid, std::string moduleName);

BYTE getMemByte(HANDLE prochandle, DWORD addr);

bool setMemByte(HANDLE prochandle, DWORD addr, BYTE newByte);

#endif