#pragma once
#define EVALUATEEXECUTORDLL_API		__declspec(dllexport)

#define NUM_OF_HANDLERS				3
#define FILE_NOTIFY_ARRAY_SIZE		1024
#define	CMD_EXECUTE					L"cmd.exe /C \""

using namespace std;

EVALUATEEXECUTORDLL_API typedef VOID(*RumCommandFunc)(__in DWORD pid);
EVALUATEEXECUTORDLL_API typedef BOOL(*FileHandle)(__in wstring FilePath);

EVALUATEEXECUTORDLL_API struct SuffixHandler {
	FileHandle	Handler;
	wstring		FileSuffix;
};

EVALUATEEXECUTORDLL_API typedef BOOL(*ChangeHandle)(
	__in PFILE_NOTIFY_INFORMATION pFileInfo,
	__in struct SuffixHandler Handlers[],
	__in DWORD HandlerArraySize);


EVALUATEEXECUTORDLL_API BOOL HandleEXE(__in wstring FilePath);
EVALUATEEXECUTORDLL_API BOOL HandleTXT(__in wstring FilePath);
EVALUATEEXECUTORDLL_API BOOL HandleDLL(__in wstring FilePath);

EVALUATEEXECUTORDLL_API VOID ListenOnFolder(
	__in LPWSTR ListenPath,
	__in ChangeHandle ChangeHandler,
	__in struct SuffixHandler Handlers[],
	__in DWORD HandlerArraySize);


