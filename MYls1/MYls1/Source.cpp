#include <windows.h>
#include <tchar.h> 
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

	WIN32_FIND_DATAW fileData;
	HANDLE hFile;
	WCHAR szDir[MAX_PATH];

	if (argc!=2) {
		wprintf(L"Too long or small name \n");
	}
	else {
		wcsncpy_s(szDir, argv[1], MAX_PATH);
		wcsncat_s(szDir, TEXT("\\*"), MAX_PATH);

		hFile = FindFirstFile(szDir, &fileData);
		if (hFile == INVALID_HANDLE_VALUE) {
			MYPerror();
		}
		do {
			//File mode
			if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				wprintf(L"D \t");
			else
				wprintf(L"F \t");
			//File time
			SYSTEMTIME stUTC, stLocal;
			FileTimeToSystemTime(&fileData.ftLastWriteTime, &stUTC);
			SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);
			wprintf(L"%02d/%02d/%d  %02d:%02d \t",
				stLocal.wMonth, stLocal.wDay, stLocal.wYear,
				stLocal.wHour, stLocal.wMinute);
			//File size
			wprintf(L"Size= %d \t", fileData.nFileSizeLow-fileData.nFileSizeHigh);

			//File attributes
			switch (fileData.dwFileAttributes) {
			case FILE_ATTRIBUTE_ARCHIVE: {wprintf(L"Attribute= FILE_ATTRIBUTE_ARCHIVE \t"); break; }
			case FILE_ATTRIBUTE_COMPRESSED: {wprintf(L"Attribute= FILE_ATTRIBUTE_COMPRESSED \t"); break; }
			case FILE_ATTRIBUTE_DEVICE: {wprintf(L"Attribute= FILE_ATTRIBUTE_DEVICE \t"); break; }
			case FILE_ATTRIBUTE_DIRECTORY: {wprintf(L"Attribute= FILE_ATTRIBUTE_DIRECTORY \t"); break; }
			case FILE_ATTRIBUTE_ENCRYPTED: {wprintf(L"Attribute= FILE_ATTRIBUTE_ENCRYPTED \t"); break; }
			case FILE_ATTRIBUTE_HIDDEN: {wprintf(L"Attribute= FILE_ATTRIBUTE_HIDDEN \t"); break; }
			case FILE_ATTRIBUTE_INTEGRITY_STREAM: {wprintf(L"Attribute= FILE_ATTRIBUTE_INTEGRITY_STREAM \t"); break; }
			case FILE_ATTRIBUTE_NORMAL: {wprintf(L"Attribute= FILE_ATTRIBUTE_NORMAL \t"); break; }
			case FILE_ATTRIBUTE_NOT_CONTENT_INDEXED: {wprintf(L"Attribute= FILE_ATTRIBUTE_NOT_CONTENT_INDEXED \t"); break; }
			case FILE_ATTRIBUTE_NO_SCRUB_DATA: {wprintf(L"Attribute= FILE_ATTRIBUTE_NO_SCRUB_DATA \t"); break; }
			case FILE_ATTRIBUTE_OFFLINE: {wprintf(L"Attribute= FILE_ATTRIBUTE_OFFLINE \t"); break; }
			case FILE_ATTRIBUTE_READONLY: {wprintf(L"Attribute= FILE_ATTRIBUTE_READONLY \t"); break; }
			case FILE_ATTRIBUTE_REPARSE_POINT: {wprintf(L"Attribute= FILE_ATTRIBUTE_REPARSE_POINT \t"); break; }
			case FILE_ATTRIBUTE_SPARSE_FILE: {wprintf(L"Attribute= FILE_ATTRIBUTE_SPARSE_FILE \t"); break; }
			case FILE_ATTRIBUTE_SYSTEM: {wprintf(L"Attribute= FILE_ATTRIBUTE_SYSTEM \t"); break; }
			case FILE_ATTRIBUTE_TEMPORARY: {wprintf(L"Attribute= FILE_ATTRIBUTE_TEMPORARY \t"); break; }
			case FILE_ATTRIBUTE_VIRTUAL: {wprintf(L"Attribute= FILE_ATTRIBUTE_VIRTUAL \t"); break; }
			default:{wprintf(L"Attribute= FILE_ATTRIBUTE_UNKNOWN \t"); }
			}

			//File Name
			wprintf(L" FileName= %s \t", fileData.cFileName);
			wprintf(L"\n\n");
		} while (FindNextFile(hFile, &fileData) != 0);
		MYPerror();
		FindClose(hFile);
	}
	MYPerror();
	//getchar();
	return 0;
}
