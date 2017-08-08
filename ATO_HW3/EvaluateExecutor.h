#include <Windows.h>
#include <string>

#define FILE_NOTIFY_ARRAY_SIZE		1024
#define	CMD_EXECUTE					L"cmd.exe /C \""

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


/// <summary> 
/// Execute the executable at the given path.
/// </summary>
/// <param name="FilePath">The path of the executable</param>
BOOL HandleEXE(__in wstring FilePath);

/// <summary> 
/// Read the given file and execute it's content inside a shell.
/// </summary>
/// <param name="FilePath">The path of the text file</param>
BOOL HandleTXT(__in wstring FilePath);

/// <summary> 
/// Execute the 'runCommand' function from the given dll.
/// </summary>
/// <param name="FilePath">The path of the dll</param>
BOOL HandleDLL(__in wstring FilePath);


/// <summary> 
/// This function handle file creating event.
/// </summary>
/// <param name="pFileInfo">The event metadata</param>
/// <param name="Handlers">Array of possible handlers</param>
/// <param name="HandlerArraySize">Number of handlers in the Handlers parameter.</param>
BOOL HandleFileChange(
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
VOID ListenOnFolder(
	__in LPWSTR ListenPath,
	__in ChangeHandle ChangeHandler,
	__in struct SuffixHandler Handlers[],
	__in DWORD HandlerArraySize);