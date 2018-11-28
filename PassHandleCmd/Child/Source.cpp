#include <Windows.h>
#include <stdio.h>
bool CopyfileFromEnd(HANDLE h_file1, WCHAR* F2)
{
	if (h_file1 == INVALID_HANDLE_VALUE) {
		wprintf(L"File1: %d\n", GetLastError());
		return false;
	}
	HANDLE h_file2 = CreateFile(
		F2,
		GENERIC_WRITE,
		0,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);
	if (h_file2 == INVALID_HANDLE_VALUE) {
		wprintf(L"File2: %d\n", GetLastError());
		return false;
	}
	const int count = 4096;
	WCHAR buff[count];
	DWORD dwBytesRead, dwBytesWritten;
	DWORD dwPos, dwPosBegin;
	dwPosBegin = SetFilePointer(h_file1, 0, NULL, FILE_BEGIN);
	dwPos = SetFilePointer(h_file2, 0, NULL, FILE_END);
	while (ReadFile(h_file1, buff, count * sizeof(WCHAR), &dwBytesRead, NULL) && dwBytesRead != 0) {
		dwPos = SetFilePointer(h_file2, 0, NULL, FILE_END);
		LockFile(h_file2, dwPos, 0, dwBytesRead, 0);
		if (!WriteFile(h_file2, buff, dwBytesRead, &dwBytesWritten, NULL) || dwBytesRead != dwBytesWritten) {
			CloseHandle(h_file2);
			wprintf(L"Write: %d\n", GetLastError());
			return false;
		}
		UnlockFile(h_file2, dwPos, 0, dwBytesRead, 0);
	}
	CloseHandle(h_file2);
	return true;
}
int wmain(int argc, WCHAR* argv[])
{
	wprintf(L"B process is Running\n");
	if (argc == 2) {
		CopyfileFromEnd((HANDLE)_wtoi(argv[1]), L"F2.txt");
	}
	system("PAUSE");
	return 0;
}