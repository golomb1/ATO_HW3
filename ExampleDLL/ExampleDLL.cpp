// ExampleDLL.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "ExampleDLL.h"

extern "C" {
	// This is an example of an exported function.
	EXAMPLEDLL_API int runCommand(DWORD pid)
	{
		HANDLE file = CreateFile(L"C:\\B\\LogDll.txt", GENERIC_WRITE, NULL, nullptr, CREATE_ALWAYS, NULL, nullptr);
		DWORD written;
		WCHAR username[256 + 1];
		DWORD username_len = 256 + 1;
		GetUserName(username, &username_len);
		WriteFile(file, username, username_len * sizeof(WCHAR), &written, nullptr);
		CloseHandle(file);
		return 42;
	}
}