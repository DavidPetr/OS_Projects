#include <windows.h>
#include <tchar.h> 
#include <iostream>
#define BUFSIZE MAX_PATH
BOOL extension_check(WCHAR*, WCHAR*);
DWORD wordCount(LPWSTR);
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
int wmain(int argc, WCHAR** argv) {

	WIN32_FIND_DATAW fileData;
	HANDLE hFile;
	WCHAR szDir[MAX_PATH];

	if (argc != 2) {
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
			wprintf(L"Size= %d \t", fileData.nFileSizeLow - fileData.nFileSizeHigh);

			//File attributes
			switch (fileData.dwFileAttributes) {
			case FILE_ATTRIBUTE_ARCHIVE: {wprintf(L"Attribute= ARCHIVE \t"); break; }
			case FILE_ATTRIBUTE_COMPRESSED: {wprintf(L"Attribute= FILE_ATTRIBUTE_COMPRESSED \t"); break; }
			case FILE_ATTRIBUTE_DEVICE: {wprintf(L"Attribute= FILE_ATTRIBUTE_DEVICE \t"); break; }
			case FILE_ATTRIBUTE_DIRECTORY: {wprintf(L"Attribute= DIRECTORY \t"); break; }
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
			default: {wprintf(L"Attribute= FILE_ATTRIBUTE_UNKNOWN \t"); }
			}

			//File Name
			if (extension_check(fileData.cFileName, L"txt")) {
				wprintf(L" FileName= %s \t", fileData.cFileName);
				//File word count if txt and assume that written in ASCII
				TCHAR Buffer[BUFSIZE], initBuff[BUFSIZE];
				if (!GetCurrentDirectory(BUFSIZE, Buffer)) {
					MYPerror();
					return 0;
				};
				if (!GetCurrentDirectory(BUFSIZE, initBuff)) {
					MYPerror();
					return 0;
				};
				wcscat(Buffer, L"\\");
				wcscat(Buffer, argv[1]);
				if (!SetCurrentDirectory(Buffer)) {
					MYPerror();
					return 0;
				};
				wcscat(fileData.cFileName, L".txt");
				wprintf(L"Word Count= %d", wordCount(fileData.cFileName));
				if (!SetCurrentDirectory(initBuff)) {
					MYPerror();
					return 0;
				};
			}
			wprintf(L"\n\n");
		} while (FindNextFile(hFile, &fileData) != 0);
		MYPerror();
		FindClose(hFile);
	}
	MYPerror();
	return 0;
}
BOOL extension_check(WCHAR* str, WCHAR *ext) {
	WCHAR * pch, *our = L"";
	pch = wcstok(str, L".");
	while (pch != NULL)
	{
		our = pch;
		pch = wcstok(NULL, L".");
	}
	if (wcscmp(our, ext) == 0)return TRUE;
	return FALSE;
}
DWORD wordCount(LPWSTR fileName) {
	HANDLE hFile = CreateFile(
		fileName,
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL
	);
	if (hFile == INVALID_HANDLE_VALUE) {
		MYPerror();
		wprintf(L"Invalid Handle\n");
		return 0;
	}
	const int count = 2;
	CHAR buff[count];
	DWORD dwBytesRead, word_count = 1;
	while (ReadFile(hFile, buff, 1 * sizeof(CHAR), &dwBytesRead, NULL) && dwBytesRead != 0) {
		if (buff[0] == ' ')word_count++;
	}
	if (CloseHandle(hFile) == 0) {
		MYPerror();
		wprintf(L"Invalid CloseHandle\n");
		return 0;
	};
	return word_count;
}