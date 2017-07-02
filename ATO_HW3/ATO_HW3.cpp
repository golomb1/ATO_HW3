// ATO_HW3.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <string>
#include <fstream>

#define NUM_OF_HANDLERS			  3
#define FILE_NOTIFY_ARRAY_SIZE    1024
#define	CMD_EXECUTE				  L"cmd.exe /C "

using namespace std;

typedef VOID(*RumCommandFunc)(__in DWORD pid);
typedef BOOL(*FileHandle)(__in std::wstring FilePath);
typedef BOOL(*ChangeHandle)(
	__in PFILE_NOTIFY_INFORMATION pFileInfo,
	__in struct SuffixHandler Handlers[],
	__in DWORD HandlerArraySize);


typedef struct SuffixHandler {
	FileHandle	Handler;
	wstring		FileSuffix;
};


BOOL HandleEXE(__in std::wstring FilePath) {
	FilePath.insert(0, L".\\");
	STARTUPINFO si;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	BOOL res = CreateProcess(FilePath.c_str(), NULL, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);
	int error = GetLastError();
	return res;
}

BOOL HandleTXT(__in std::wstring FilePath) {
	size_t cmd_prefix = wcslen(CMD_EXECUTE);
	std::wifstream file(FilePath.c_str());
	wstring filebuffer;
	wstring linebuffer;
	bool firstIteration = true;
	while (file && getline(file, linebuffer)) {
		if (linebuffer.length() == 0) {
			continue;
		}
		if (firstIteration) {
			firstIteration = false;
		}
		else {
			filebuffer.append(L"\\n");
		}
		filebuffer.append(linebuffer);
	}
	PWCHAR cmd = new WCHAR[cmd_prefix + filebuffer.length() + 1];
	ZeroMemory(cmd, sizeof(WCHAR)*(cmd_prefix + filebuffer.length() + 1));
	wcscpy_s(cmd, (filebuffer.length() + 1 + cmd_prefix) * sizeof(WCHAR), CMD_EXECUTE);
	wcscpy_s(cmd + cmd_prefix, filebuffer.length() * sizeof(WCHAR), filebuffer.c_str());
	STARTUPINFO si;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	BOOL res = CreateProcess(NULL, cmd, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);
	int error = GetLastError();
	return res;
}

BOOL HandleDLL(__in std::wstring FilePath) {
	HMODULE dllModule = LoadLibrary(FilePath.c_str());
	if (dllModule == NULL) {
		return FALSE;
	}
	RumCommandFunc proc = (RumCommandFunc)GetProcAddress(dllModule, "runCommand");
	proc(GetCurrentProcessId());
	FreeLibrary(dllModule);
}

BOOL HandleFileChange(
	__in PFILE_NOTIFY_INFORMATION pFileInfo,
	__in struct SuffixHandler* Handlers,
	__in DWORD HandlerArraySize)
{
	if (pFileInfo->Action & FILE_ACTION_ADDED) {
		WCHAR* buffer = new WCHAR[pFileInfo->FileNameLength / sizeof(WCHAR) + 1];
		ZeroMemory(buffer, pFileInfo->FileNameLength + sizeof(WCHAR));
		memcpy_s(buffer, pFileInfo->FileNameLength + sizeof(WCHAR), pFileInfo->FileName, pFileInfo->FileNameLength);
		wstring fileName(buffer);
		size_t extensionIndex = fileName.find_last_of(L".");
		if (extensionIndex != wstring::npos && extensionIndex + 1 < fileName.length()) {
			wstring extention = fileName.substr(extensionIndex + 1);
			for (int i = 0; i < HandlerArraySize; i++) {
				if (extention == Handlers[i].FileSuffix) {
					Handlers[i].Handler(fileName);
					break;
				}
			}
		}
		delete[] buffer;
		return TRUE;
	}
	return FALSE;
}


VOID ListenOnFolder(
	__in LPWSTR ListenPath,
	__in ChangeHandle ChangeHandler,
	__in struct SuffixHandler Handlers[],
	__in DWORD HandlerArraySize)
{

	HANDLE DirHandle = CreateFile(ListenPath,
		FILE_LIST_DIRECTORY,
		FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE,
		NULL,
		OPEN_ALWAYS,
		FILE_FLAG_BACKUP_SEMANTICS,
		NULL);

	if (DirHandle == INVALID_HANDLE_VALUE) {
		printf("\n ERROR: CreateFile function failed - %d.\n", GetLastError());
		ExitProcess(GetLastError());
	}

	if(!SetCurrentDirectory(ListenPath)){
		printf("\n ERROR: SetCurrentDirectory function failed - %d.\n", GetLastError());
		ExitProcess(GetLastError());
}
	DWORD Counter = 0;
	FILE_NOTIFY_INFORMATION FileNotifyInfo[FILE_NOTIFY_ARRAY_SIZE];
	DWORD dwByteReturned = 0;

	while (TRUE) {
		if (ReadDirectoryChangesW(DirHandle,
			&FileNotifyInfo,
			FILE_NOTIFY_ARRAY_SIZE * sizeof(FILE_NOTIFY_INFORMATION),
			FALSE,
			FILE_NOTIFY_CHANGE_FILE_NAME,
			&dwByteReturned,
			NULL,
			NULL)) 
		{
			FILE_NOTIFY_INFORMATION* pFileInfo = &(FileNotifyInfo[0]);
			while (pFileInfo->NextEntryOffset) {
				ChangeHandler(pFileInfo, Handlers, HandlerArraySize);
				pFileInfo = (FILE_NOTIFY_INFORMATION*)(((PBYTE)pFileInfo) + pFileInfo->NextEntryOffset);
			}
			ChangeHandler(pFileInfo, Handlers, HandlerArraySize);
		}
	}
}

int main()
{
	struct SuffixHandler handlers[NUM_OF_HANDLERS];
	ZeroMemory(handlers, NUM_OF_HANDLERS * sizeof(struct SuffixHandler));
	handlers[0].FileSuffix = wstring(L"exe");
	handlers[0].Handler = &HandleEXE;
	handlers[1].FileSuffix = wstring(L"txt");
	handlers[1].Handler = &HandleTXT;
	handlers[2].FileSuffix = wstring(L"dll");
	handlers[2].Handler = &HandleDLL;

	ListenOnFolder(L"C:\\A", &HandleFileChange, handlers, NUM_OF_HANDLERS);
    return 0;
}

