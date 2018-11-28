#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <winerror.h>
#include <stdio.h>
#include <iostream>
#include "accctrl.h"
#include "aclapi.h"
#define count 5
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
bool read(HANDLE hfile, CHAR destBuf[]) {
	if (hfile == INVALID_HANDLE_VALUE) {
		wprintf(L"File1 B: %d\n", GetLastError());
		return FALSE;
	}
	CHAR buffer[count];
	DWORD dwBytesRead, i = 0;
	while (ReadFile(hfile, buffer, 1 * sizeof(CHAR), &dwBytesRead, NULL) && dwBytesRead != 0) {
		if ('0' <= buffer[0] && buffer[0] <= '9')destBuf[i++] = buffer[0];
	}
	destBuf[i] = '\0';
	return TRUE;
}
bool write(HANDLE hfile, WCHAR buffer[], DWORD bufSize) {
	if (hfile == INVALID_HANDLE_VALUE) {
		wprintf(L"File1: %d\n", GetLastError());
		return FALSE;
	}
	DWORD dwBytesWritten;
	DWORD dwPos = SetFilePointer(hfile, 0, NULL, FILE_BEGIN);
	if (!WriteFile(hfile, buffer, bufSize * sizeof(WCHAR), &dwBytesWritten, NULL)) {
		wprintf(L"Write: %d\n", GetLastError());
		return FALSE;
	}
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
DWORD length(DWORD x) {
	DWORD i = 0;
	while (x) {
		x /= 10;
		i++;
	}
	return i;
}
int wmain(int argc, WCHAR* argv[])
{
	wprintf(L"Process1 Running \n");
	HANDLE h_file1 = CreateFile(
		L"F1.txt",
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL
	);
	CHAR tempBuffer[count];
	if (!read(h_file1, tempBuffer)) {
		MYPerror();
		wprintf(L"Cannot Read\n");
		exit(1);
	}
	WCHAR bufferClear[] = L"       ";
	if (!write(h_file1, bufferClear, wcslen(bufferClear))) {
		MYPerror();
		wprintf(L"Cannot Clear\n");
		exit(1);
	}
	DWORD Process2ID = toDWORD(tempBuffer);	

	HANDLE h_file2 = CreateFile(
		L"F2.txt",
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);
	WCHAR buffer[] = L"Process 'Child' creates F2.txt \n";
	if (!write(h_file2, buffer, wcslen(buffer))) {
		MYPerror();
		wprintf(L"Cannot Write\n");
		exit(1);
	}
	wprintf(L"P21== %d", Process2ID);
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, Process2ID);
	//***
	HANDLE dupHandle;
	if (!DuplicateHandle(GetCurrentProcess(), h_file2, hProcess, &dupHandle, 0, FALSE, DUPLICATE_SAME_ACCESS))
	{
		MYPerror();
		wprintf(L"Duplicate Error \n");
	}
	DWORD h_length = length((DWORD)dupHandle);
	WCHAR buff[20];
	_itow((DWORD)dupHandle, buff, 10);
	if (!write(h_file1, buff, h_length)) {
		MYPerror();
		wprintf(L"Cannot Write duphandle \n");
		exit(1);
	}
	CloseHandle(h_file1);
	Sleep(15000);	
	CloseHandle(h_file2);
	system("PAUSE");
	return 0;
}