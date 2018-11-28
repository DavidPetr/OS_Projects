#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <iostream>
bool isCont;
void MYPerror(DWORD error_code=GetLastError()) {
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
BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
	if (fdwCtrlType == CTRL_C_EVENT) {
		printf("\nThe Control Handler is installed.\n");
		isCont = FALSE;
		return true;
	}
	return false;
}
bool copyFile(HANDLE h_file1, HANDLE h_file2)
{
	if (h_file1 == INVALID_HANDLE_VALUE) {
		wprintf(L"File1: %d\n", GetLastError());
		return false;
	}
	if (h_file2 == INVALID_HANDLE_VALUE) {
		CloseHandle(h_file1);
		wprintf(L"File2: %d\n", GetLastError());
		return false;
	}
	const int count = 4096;
	WCHAR buff[count];
	DWORD number1, number2;
	while (ReadFile(h_file1, buff, count * sizeof(WCHAR), &number1, NULL) &&
		number1 != 0 && isCont) {
		if (!WriteFile(h_file2, buff, number1, &number2, NULL) || number1 != number2) {
			CloseHandle(h_file1);
			wprintf(L"Write: %d\n", GetLastError());
			return false;
		}
	}
	CloseHandle(h_file1);
	return true;
}


int wmain(int argc, WCHAR* argv[]) {
	HANDLE hFile;
	isCont = true;
	if (0 == wcscmp(argv[argc - 2], L">")) {
		HANDLE h_fileLast = CreateFile(
			argv[argc - 1],
			GENERIC_WRITE,
			0,
			NULL,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL
		);
		for (int i = 1; i < argc - 2; i++) {
			hFile = CreateFile(
				argv[i],
				GENERIC_READ,
				0,
				NULL,
				OPEN_EXISTING,
				0,
				NULL
			);
			MYPerror(!copyFile(hFile, h_fileLast));
			isCont = true;
		}
		CloseHandle(h_fileLast);
	}
	else if (argc == 2) {
		hFile = CreateFile(
			argv[1],
			GENERIC_READ,
			0,
			NULL,
			OPEN_EXISTING,
			0,
			NULL
		);
		HANDLE handle1 = GetStdHandle(STD_OUTPUT_HANDLE);
		MYPerror(!copyFile(hFile, handle1));
		isCont = true;
		CloseHandle(handle1);
	}
	else if (argc > 2) {
		HANDLE handle1 = GetStdHandle(STD_OUTPUT_HANDLE);
		for (int i = 1; i < argc; i++) {
			hFile = CreateFile(
				argv[i],
				GENERIC_READ,
				0,
				NULL,
				OPEN_EXISTING,
				0,
				NULL
			);
			MYPerror(!copyFile(hFile, handle1));
			isCont = true;
		}
		CloseHandle(handle1);
	}
	getchar();
	return 0;
}
