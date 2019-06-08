#include "pattern_search.h"

bool dataCompare(BYTE* addr, BYTE* pattern, const char* sign, int len)
{
	if (len <= 0)
		return false;
	
	for (int i = 0; i < len; i++, addr++)
	{
		if (sign[i] == '?' || (sign[i] == 'x' && *addr == pattern[i]))
			continue;
		else
			return false;
	}

	return true;
}

DWORD findPattern(BYTE* pattern, const char* sign, DWORD beginAddr, DWORD Range)
{
	DWORD oldProtect[2], res = 0;

	int patternSize = strlen(sign);

	for (DWORD i = beginAddr; i < (beginAddr + Range); i++)
	{
		if (VirtualProtect((LPVOID)i, patternSize, PAGE_EXECUTE_READWRITE, &oldProtect[0]))
		{
			if (dataCompare((BYTE*)i, pattern, sign, patternSize)) 
			{
				VirtualProtect((LPVOID)i, patternSize, oldProtect[0], &oldProtect[1]);
				res = i;
				break;
			}

			VirtualProtect((LPVOID)i, patternSize, oldProtect[0], &oldProtect[1]);
		}
	}

	return res;
}

DWORD findPatternEx(HANDLE procHandle, BYTE* pattern, const char* sign, DWORD beginAddr, DWORD Range)
{
	DWORD oldProtect[2], res = 0;

	const int patternSize = strlen(sign);

	// Allocate byte size for pattern for external faster reading.
	BYTE* byteHolder = new BYTE[patternSize];

	for (DWORD i = beginAddr; i < (beginAddr + Range); i++)
	{
		if (VirtualProtectEx(procHandle, (LPVOID)i, patternSize, PAGE_EXECUTE_READWRITE, &oldProtect[0]))
		{
			ReadProcessMemory(procHandle, (LPVOID)i, byteHolder, patternSize, 0);
			// Not gonna restore because on some parts it causes a crash. Probably because of wrong privelege re-writing.
			//VirtualProtectEx(procHandle, (LPVOID)i, patternSize, oldProtect[0], &oldProtect[1]);  

			if (dataCompare(byteHolder, pattern, sign, patternSize))
			{
				res = i;
				break;
			}
		}
	}

	// Free mem.
	delete[patternSize] byteHolder;

	return res;
}
