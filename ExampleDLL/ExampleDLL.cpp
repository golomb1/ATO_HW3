// ExampleDLL.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "ExampleDLL.h"


// This is an example of an exported function.
EXAMPLEDLL_API int runCommand(DWORD pid)
{
	HANDLE file = CreateFile(L"C:\\B\\LogDll.txt", GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, NULL, NULL);
	DWORD written;
	WCHAR username[256 + 1];
	DWORD username_len = 256 + 1;
	GetUserName(username, &username_len);
	WriteFile(file, username, username_len, &written, NULL);
	return 42;
}