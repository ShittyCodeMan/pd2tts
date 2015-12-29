#include <windows.h>
#include <stdio.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

char ModulePath[MAX_PATH];

void _STSpeech(const char *message)
{
	PROCESS_INFORMATION pi;
	STARTUPINFOA si;
	DWORD numberOfBytesWritten;

	HANDLE readTemp;
	HANDLE readPipe  = NULL;
	HANDLE writePipe = NULL;

	char FileName[MAX_PATH];

	CreatePipe(&readTemp, &writePipe, NULL, 0);

	DuplicateHandle(
		GetCurrentProcess(), readTemp,
		GetCurrentProcess(), &readPipe,
		0, TRUE, DUPLICATE_SAME_ACCESS);

	CloseHandle(readTemp);
	
	SecureZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESTDHANDLES;
	si.hStdInput = readPipe;

	lstrcpy(FileName, ModulePath);
	lstrcat(FileName, "stspeech.exe");
	CreateProcess(
		FileName, NULL,
		NULL, NULL,
		TRUE,
		CREATE_NO_WINDOW,
		NULL, NULL,
		&si, &pi);
	WriteFile(writePipe, message, lstrlen(message), &numberOfBytesWritten, NULL);
	
	CloseHandle(readPipe);
	CloseHandle(writePipe);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
}

__declspec(dllexport) int luaSTSpeech(lua_State *L) // lua_State is broken. I don't know how to fix :p
{
	HANDLE hFile;
	char FileName[MAX_PATH];
	char str[1024];
	DWORD numberOfBytesRead;
	
	lstrcpy(FileName, ModulePath);
	lstrcat(FileName, "msg.dat");
	hFile = CreateFile(
		FileName,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (INVALID_HANDLE_VALUE == hFile) {
		return 0;
	}
	ReadFile(hFile, str, sizeof(str), &numberOfBytesRead, NULL);
	str[numberOfBytesRead] = '\0';
	CloseHandle(hFile);
	
	_STSpeech(str);
	
	return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
					   DWORD  ul_reason_for_call,
					   LPVOID lpReserved
					 )
{
	int i;
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		GetModuleFileName(hModule, ModulePath, sizeof(ModulePath));
		for(i = sizeof(ModulePath) - 1; i > 0 && ModulePath[i] != '\\'; i--) ModulePath[i] = '\0';
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

