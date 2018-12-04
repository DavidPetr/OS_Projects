#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <iostream>
BOOL isContinue = TRUE;
static BOOL WINAPI ctrl_c_handler(DWORD fdwCtrlType)
{
	if (fdwCtrlType == CTRL_C_EVENT) {
		printf("\nThe Control Handler is installed.\n");
		isContinue = FALSE;
		return TRUE;
	}
	return FALSE;
}
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
bool Copyfile(HANDLE h_file1,HANDLE h_file2)
{
	if (h_file1 == INVALID_HANDLE_VALUE) {
		wprintf(L"File1 Error \n");
		MYPerror();
		return FALSE;
	}
	if (h_file2 == INVALID_HANDLE_VALUE) {
		wprintf(L"File2 Error \n");
		MYPerror();
		return FALSE;
	}
	const int count = 4096;
	WCHAR buff[count];
	DWORD dwBytesToRead, dwBytesWritten;
	DWORD dwPos = SetFilePointer(h_file2, 0, NULL, FILE_END);
	while (ReadFile(h_file1, buff, count * sizeof(WCHAR), &dwBytesToRead, NULL) && dwBytesToRead != 0) {
		if (!WriteFile(h_file2, buff, dwBytesToRead, &dwBytesWritten, NULL) || dwBytesToRead != dwBytesWritten) {
			wprintf(L"Write Error \n");
			MYPerror();
			return FALSE;
		}
	}
	return TRUE;
}

int wmain(int argc, WCHAR* argv[]) {
	SECURITY_ATTRIBUTES secAtt;
	secAtt.nLength = sizeof(SECURITY_ATTRIBUTES);
	secAtt.bInheritHandle = TRUE;
	secAtt.lpSecurityDescriptor = NULL;
	HANDLE h_file1 = CreateFile(
		L"F1.txt",
		GENERIC_READ | GENERIC_WRITE,
		0,
		&secAtt,
		OPEN_EXISTING,
		0,
		NULL
	);
	if (h_file1 == INVALID_HANDLE_VALUE) {
		MYPerror();
		wprintf(L"File1\n");
		getchar();
		exit(1);
	}

	HANDLE Mutex;
	Mutex = CreateMutex(&secAtt, FALSE, L"Mutex");
	if (Mutex == NULL)
	{
		printf("CreateMutex error: %d\n", GetLastError());
		getchar();
		return 1;
	}

	STARTUPINFO s;
	PROCESS_INFORMATION p;
	ZeroMemory(&s, sizeof(s));
	s.cb = sizeof(s);
	ZeroMemory(&p, sizeof(p));
	WCHAR cmd[30] = L"SubProcess.exe ";
	WCHAR buffer[12];
	_itow((DWORD)h_file1, buffer, 10);
	wcscat(cmd, buffer);
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
	DWORD dwWaitResult;
	while (isContinue) {
		dwWaitResult = WaitForSingleObject(Mutex, INFINITE);
		if (dwWaitResult == WAIT_OBJECT_0) {
			__try {
				wprintf(L"First \n");
				HANDLE Mutex = OpenMutex(MUTEX_ALL_ACCESS, TRUE, L"Mutex");
				if (Mutex == NULL)
				{
					printf("CreateMutex error: %d\n", GetLastError());
					getchar();
					return 1;
				}
				wprintf(L"First Lock Mutex \n");
				HANDLE h_input = GetStdHandle(STD_INPUT_HANDLE);
				if (!Copyfile(h_input, h_file1))
				{
					wprintf(L"Writting error\n");
					MYPerror();
				}
			}
			__finally {
				if (!ReleaseMutex(Mutex)) {
					wprintf(L"Mutex Release Error\n");
					MYPerror();
				}
			}
		}
	}
	HANDLE h_input = GetStdHandle(STD_INPUT_HANDLE);
	if (!Copyfile(h_input, h_file1))
	{
		wprintf(L"Writting error\n");
		MYPerror();
	}
	CloseHandle(p.hProcess);
	CloseHandle(p.hThread);
	CloseHandle(h_file1);
	return 0;
}
