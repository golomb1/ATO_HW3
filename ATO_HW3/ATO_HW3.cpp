// ATO_HW3.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <string>
#include <fstream>

#define NUM_OF_HANDLERS			  3
#define FILE_NOTIFY_ARRAY_SIZE    1024
#define	CMD_EXECUTE				  L"cmd.exe /C \""

using namespace std;

typedef VOID(*RumCommandFunc)(__in DWORD pid);
typedef BOOL(*FileHandle)(__in wstring FilePath);

struct SuffixHandler {
	FileHandle	Handler;
	wstring		FileSuffix;
};

typedef BOOL(*ChangeHandle)(
	__in PFILE_NOTIFY_INFORMATION pFileInfo,
	__in struct SuffixHandler Handlers[],
	__in DWORD HandlerArraySize);




BOOL HandleEXE(__in wstring FilePath) {
	FilePath.insert(0, L".\\");
	STARTUPINFO si;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	BOOL res = CreateProcess(FilePath.c_str(), nullptr, nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS, nullptr, nullptr, &si, &pi);
	return res;
}



BOOL HandleTXT(__in wstring FilePath) {
	SetCurrentDirectory(FilePath.c_str());
	size_t cmd_prefix = wcslen(CMD_EXECUTE);
	wifstream file(L"ExampleText.txt");
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
	int size = filebuffer.length();
	if (size == 0)
	{
		return FALSE;
	}
	PWCHAR cmd = new WCHAR[cmd_prefix + size + 2];
	ZeroMemory(cmd, sizeof(WCHAR)*(cmd_prefix + size + 2));
	wsprintf(cmd, L"%s%s\"", CMD_EXECUTE, filebuffer.c_str());

	STARTUPINFO si;
	ZeroMemory(&si, sizeof si);
	si.cb = sizeof si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof pi);
	return CreateProcess(nullptr, cmd, nullptr, nullptr, FALSE, NULL, nullptr, nullptr, &si, &pi);
}

BOOL HandleDLL(__in wstring FilePath) {
	HMODULE dllModule = LoadLibrary(FilePath.c_str());
	if (dllModule == nullptr) {
		return FALSE;
	}
	RumCommandFunc proc = (RumCommandFunc)GetProcAddress(dllModule, "runCommand");
	if (proc != nullptr) {
		proc(GetCurrentProcessId());
	}
	FreeLibrary(dllModule);
	return TRUE;
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
		nullptr,
		OPEN_ALWAYS,
		FILE_FLAG_BACKUP_SEMANTICS,
		nullptr);

	if (DirHandle == INVALID_HANDLE_VALUE) {
		printf("\n ERROR: CreateFile function failed - %d.\n", GetLastError());
		ExitProcess(GetLastError());
	}

	if (!SetCurrentDirectory(ListenPath)) {
		printf("\n ERROR: SetCurrentDirectory function failed - %d.\n", GetLastError());
		ExitProcess(GetLastError());
	}
	FILE_NOTIFY_INFORMATION FileNotifyInfo[FILE_NOTIFY_ARRAY_SIZE];
	DWORD dwByteReturned = 0;

	while (true) {
		if (ReadDirectoryChangesW(DirHandle,
			&FileNotifyInfo,
			FILE_NOTIFY_ARRAY_SIZE * sizeof(FILE_NOTIFY_INFORMATION),
			FALSE,
			FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_SIZE,
			&dwByteReturned,
			nullptr,
			nullptr)) 
		{
			FILE_NOTIFY_INFORMATION* pFileInfo = &FileNotifyInfo[0];
			while (pFileInfo->NextEntryOffset) {
				ChangeHandler(pFileInfo, Handlers, HandlerArraySize);
				pFileInfo = (FILE_NOTIFY_INFORMATION*)((PBYTE)pFileInfo + pFileInfo->NextEntryOffset);
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

