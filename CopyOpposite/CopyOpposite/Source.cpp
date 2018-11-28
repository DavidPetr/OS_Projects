#include <windows.h>
#include <tchar.h>
#include <stdio.h>
bool copyFile(WCHAR * f1, WCHAR * f2)
{
	HANDLE h_file1 = CreateFile(
		f1,
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL
	);
	if (h_file1 == INVALID_HANDLE_VALUE) {
		wprintf(L"File1: %d\n", GetLastError());
		return false;
	}
	HANDLE h_file2 = CreateFile(
		f2,
		GENERIC_WRITE,
		0,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);
	if (h_file2 == INVALID_HANDLE_VALUE) {
		CloseHandle(h_file1);
		wprintf(L"File2: %d\n", GetLastError());
		return false;
	}
	const int count = 4096;
	WCHAR buff[count];
	DWORD dwBytesRead, dwBytesWritten;
	DWORD dwPos,dwPosBegin;
	LONG i = -1;
	dwPosBegin = SetFilePointer(h_file1, 0, NULL, FILE_BEGIN);
	dwPos=SetFilePointer(h_file1, i, NULL, FILE_END);
	while (ReadFile(h_file1, buff, 1, &dwBytesRead, NULL) && dwBytesRead > 0 && (dwPos+1)!= dwPosBegin) {
		dwPos=SetFilePointer(h_file1, --i, NULL, FILE_END);
		LockFile(h_file2, dwPos, 0, dwBytesRead, 0);
		if (!WriteFile(h_file2, buff, dwBytesRead, &dwBytesWritten, NULL) || dwBytesRead != dwBytesWritten) {
			CloseHandle(h_file1);
			CloseHandle(h_file2);
			wprintf(L"Write: %d\n", GetLastError());
			return false;
		}
		UnlockFile(h_file2, dwPos, 0, dwBytesRead, 0);
	}
	CloseHandle(h_file1);
	CloseHandle(h_file2);
	return true;
}


int wmain(int argc, WCHAR* argv[]) {
	if (argc == 3)copyFile(argv[1], argv[2]);
	getchar();
	return 0;
}
