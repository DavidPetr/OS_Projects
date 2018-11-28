#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <iostream>
#define  countBuff 25
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
DWORD length(DWORD x) {
	DWORD i = 0;
	while (x) {
		x /= 10;
		i++;
	}
	return i;
}
bool write(HANDLE hfile, WCHAR buffer[], DWORD bufSize) {
	if (hfile == INVALID_HANDLE_VALUE) {
		wprintf(L"File1: %d\n", GetLastError());
		return FALSE;
	}
	DWORD dwBytesWritten;
	if (!WriteFile(hfile, buffer, bufSize * sizeof(WCHAR), &dwBytesWritten, NULL)) {
		if (bufSize * sizeof(WCHAR) == 0)wprintf(L"Because of 0 \n");
		wprintf(L"Write: %d\n", GetLastError());
		return FALSE;
	}
	return TRUE;
}
bool read(HANDLE hfile, CHAR destBuf[]) {
	if (hfile == INVALID_HANDLE_VALUE) {
		wprintf(L"File1 B: %d\n", GetLastError());
		return FALSE;
	}
	CHAR buffer[countBuff];
	DWORD dwBytesRead, i = 0;
	while (ReadFile(hfile, buffer, 1 * sizeof(CHAR), &dwBytesRead, NULL) && dwBytesRead != 0) {
		if ('0' <= buffer[0] && buffer[0] <= '9')destBuf[i++] = buffer[0];
	}
	destBuf[i] = '\0';
	return TRUE;
}
DWORD toDWORD(CHAR* buffer) {
	DWORD len = strlen(buffer);
	DWORD x = 0, i = 0;
	while (i < len) {
		x *= 10;
		x += buffer[i++] - '0';
	}
	return x;
}
int wmain(int argc, WCHAR* argv[]) {
	wprintf(L"Process2 Running \n");
	HANDLE h_file1 = CreateFile(
		L"F1.txt",
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL
	);
	if (h_file1 == INVALID_HANDLE_VALUE) {
		MYPerror();
		wprintf(L"File1\n");
		exit(1);
	}
	DWORD Process2ID = GetCurrentProcessId();
	DWORD count = length(Process2ID);
	WCHAR buff[20];
	_itow(Process2ID, buff, 10);
	if (!write(h_file1, buff, count)) {
		MYPerror();
		wprintf(L"Cannot Write\n");
		exit(1);
	}
	CloseHandle(h_file1);
	wprintf(L"P22== %d", Process2ID);
	Sleep(10000);
	HANDLE h_file1_again = CreateFile(
		L"F1.txt",
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL
	);
	CHAR tempBuffer[countBuff];
	if (!read(h_file1_again, tempBuffer)) {
		MYPerror();
		wprintf(L"Cannot Read\n");
		exit(1);
	}
	CloseHandle(h_file1_again);
	DWORD dupHANDLE = toDWORD(tempBuffer);
	WCHAR buffer[]= L"Process2 use duphandle \n";
	if (!write((HANDLE)dupHANDLE, buffer, wcslen(buffer))) {
		MYPerror();
		wprintf(L"Cannot Write process2 \n");
		exit(1);
	}
	system("PAUSE");
	return 0;
}