#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <iostream>
void MYPerror(DWORD error_code = GetLastError()) {
	LPWSTR pBuffer = NULL;
	if (!FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		error_code,
		0,
		(LPWSTR)&pBuffer,
		0,
		NULL))
	{
		wprintf(L"Format message failed with %d\n", GetLastError());
		return;
	}
	setlocale(LC_ALL, "Russian");
	wprintf(L"Formatted message: %s\n", pBuffer);

}
int wmain(int argc, WCHAR* argv[]) {
	SECURITY_ATTRIBUTES secAtt;
	secAtt.nLength = sizeof(SECURITY_ATTRIBUTES);
	secAtt.bInheritHandle = TRUE;
	secAtt.lpSecurityDescriptor = NULL;
	HANDLE h_file1 = CreateFile(
		L"F11.txt",
		GENERIC_READ|GENERIC_WRITE,
		0,
		&secAtt,
		OPEN_EXISTING,
		0,
		NULL
	);
	if (h_file1 == INVALID_HANDLE_VALUE) {
		MYPerror();
		wprintf(L"File1\n");
		exit(1);
	}

	DWORD AID = GetCurrentProcessId();
	wprintf(L"AID %d\n", AID);
	STARTUPINFO s;
	PROCESS_INFORMATION p;
	ZeroMemory(&s, sizeof(s));
	s.cb = sizeof(s);
	ZeroMemory(&p, sizeof(p));
	WCHAR cmd[] = L"Child.exe ";
	WCHAR buffer[12];
	WCHAR *handle = _itow((DWORD)h_file1, buffer, 10);
	wcscat(cmd, buffer);
	//DuplicateHandle(GetCurrentProcess(), h_file1, p.hProcess, &h_B, 0, TRUE, DUPLICATE_SAME_ACCESS);
	if (!CreateProcess(
		NULL,
		cmd,
		NULL,
		NULL,
		TRUE,
		0,
		NULL,
		NULL,
		&s,
		&p
	))
	{
		wprintf(L"Error %d \n", GetLastError());
		exit(1);
	}
	WaitForSingleObject(p.hProcess, INFINITE);
	CloseHandle(h_file1);
	CloseHandle(p.hProcess);
	CloseHandle(p.hThread);
	system("PAUSE");
	return 0;
}


