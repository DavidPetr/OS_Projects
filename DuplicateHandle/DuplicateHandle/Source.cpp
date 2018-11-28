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

	
	STARTUPINFO s1,s2;
	PROCESS_INFORMATION p1,p2;
	ZeroMemory(&s1, sizeof(s1));
	ZeroMemory(&s2, sizeof(s2));
	s1.cb = sizeof(s1);
	s2.cb = sizeof(s2);
	ZeroMemory(&p1, sizeof(p1));
	ZeroMemory(&p2, sizeof(p2));
	WCHAR cmd1[] = L"Process1.exe ", cmd2[] = L"Process2.exe ";
	if (!CreateProcess(
		NULL,
		cmd2,
		NULL,
		NULL,
		FALSE,
		0,
		NULL,
		NULL,
		&s2,
		&p2
	))
	{
		wprintf(L"Error %d \n", GetLastError());
		exit(1);
	}
	if (!CreateProcess(
		NULL,
		cmd1,
		NULL,
		NULL,
		FALSE,
		PROCESS_DUP_HANDLE,
		NULL,
		NULL,
		&s1,
		&p1
	))
	{
		wprintf(L"Error %d \n", GetLastError());
		exit(1);

	}
	
	HANDLE lpHandles[2] = { p1.hProcess ,p2.hProcess };
	WaitForMultipleObjects(2, lpHandles, TRUE, INFINITE);

	CloseHandle(p1.hProcess);
	CloseHandle(p1.hThread);
	CloseHandle(p2.hProcess);
	CloseHandle(p2.hThread);
	system("PAUSE");
	return 0;
}


