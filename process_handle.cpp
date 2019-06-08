#include "process_handle.h"

int getProcID(std::string processName)
{
	int processid = 0;

	HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snap != 0)
	{
		PROCESSENTRY32 pe;
		pe.dwSize = sizeof(PROCESSENTRY32);

		Process32First(snap, &pe);
		do
		{
			if (!strcmp(pe.szExeFile, processName.c_str()))
			{
				processid = pe.th32ProcessID;
				break;
			}
		} while (Process32Next(snap, &pe));
		
		CloseHandle(snap);
	}

	return processid;
}

HANDLE getProcessHandle(int processid, long desiredAccess)
{
	HANDLE prochandle = OpenProcess(desiredAccess, 0, processid);
	return prochandle;
}

DWORD getProcessModuleAddr(int processid, std::string moduleName)
{
	DWORD res = 0;
	HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processid);

	if (snap != 0)
	{
		MODULEENTRY32 me;
		me.dwSize = sizeof(MODULEENTRY32);

		Module32First(snap, &me);
		do
		{
			if (!strcmp(me.szModule, moduleName.c_str()))
			{
				res = (DWORD)me.modBaseAddr;
				break;
			}
		} while (Module32Next(snap, &me));

		CloseHandle(snap);
	}
	return res;
}

BYTE getMemByte(HANDLE prochandle, DWORD addr)
{
	BYTE res = 0;
	if (ReadProcessMemory(prochandle, (LPVOID)addr, &res, 1, 0))
	{
		return res;
	}
	return -1;
}

bool setMemByte(HANDLE prochandle, DWORD addr, BYTE newByte)
{
	BYTE newval = newByte; 
	SIZE_T writtenBytes;
	if (WriteProcessMemory(prochandle, (LPVOID)addr, &newval, 1, &writtenBytes))
	{
		if (writtenBytes == 1)
			return true;
		else
			return false;
	}

	return false;
}
