#pragma once
#define EVALUATEEXECUTORDLL_API		__declspec(dllexport)
#define FILE_NOTIFY_ARRAY_SIZE		1024
#define	CMD_EXECUTE					L"cmd.exe /C \""

using namespace std;


extern "C"
{
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


	/// <summary> 
	/// Execute the executable at the given path.
	/// </summary>
	/// <param name="FilePath">The path of the executable</param>
	EVALUATEEXECUTORDLL_API BOOL HandleEXE(__in wstring FilePath);

	/// <summary> 
	/// Read the given file and execute it's content inside a shell.
	/// </summary>
	/// <param name="FilePath">The path of the text file</param>
	EVALUATEEXECUTORDLL_API BOOL HandleTXT(__in wstring FilePath);

	/// <summary> 
	/// Execute the 'runCommand' function from the given dll.
	/// </summary>
	/// <param name="FilePath">The path of the dll</param>
	EVALUATEEXECUTORDLL_API BOOL HandleDLL(__in wstring FilePath);


	/// <summary> 
	/// This function handle file creating event.
	/// </summary>
	/// <param name="pFileInfo">The event metadata</param>
	/// <param name="Handlers">Array of possible handlers</param>
	/// <param name="HandlerArraySize">Number of handlers in the Handlers parameter.</param>
	EVALUATEEXECUTORDLL_API BOOL HandleFileChange(
		__in PFILE_NOTIFY_INFORMATION pFileInfo,
		__in struct SuffixHandler* Handlers,
		__in DWORD HandlerArraySize);


	/// <summary> 
	/// This function list on a folder for file creation event.
	///	This function change the current directory of the executing process.
	/// This function doesn't not return.
	/// </summary>
	/// <param name="ListenPath">The path of the folder to listen.</param>
	/// <param name="ChangeHandler">A function that decide how to handle a change event.</param>
	/// <param name="Handlers">Array of possible handlers, it is passed to the ChangeHandler parameter.</param>
	/// <param name="HandlerArraySize">Number of handlers in the Handlers parameter.</param>
	EVALUATEEXECUTORDLL_API VOID ListenOnFolder(
		__in LPWSTR ListenPath,
		__in ChangeHandle ChangeHandler,
		__in struct SuffixHandler Handlers[],
		__in DWORD HandlerArraySize);


	EVALUATEEXECUTORDLL_API VOID EvaluatorExecutorMain();
}