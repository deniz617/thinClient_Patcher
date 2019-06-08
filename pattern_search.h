#ifndef PATTERN_SEARCH_H
#define PATTERN_SEARCH_H
#include "main.h"

DWORD findPattern(BYTE* pattern, const char* sign, DWORD beginRange = 0x0, DWORD endRange = 0x7FFFF);

DWORD findPatternEx(HANDLE procHandle, BYTE* pattern, const char* sign, DWORD beginRange = 0x0, DWORD endRange = 0x7FFFF);

#endif
