#include <Windows.h>
#include <stdio.h>
#include <iostream>
#define BUFFSIZE 4096
DWORD dwBytesRead;
HANDLE threads[3];
CHAR* buffer;
HANDLE event;
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
bool write(HANDLE hfile, CHAR* buffer, DWORD bufSize) {
	DWORD dwBytesWritten;
	if (!WriteFile(hfile, buffer, bufSize * sizeof(CHAR), &dwBytesWritten, NULL)) {
		wprintf(L"Write\n");
		MYPerror();
		return FALSE;
	}
	return TRUE;
}
DWORD WINAPI WordCount(void* p){
	while (1) {
		WaitForSingleObject(event, INFINITE);
		DWORD word_count = 1;
		for (int i = 0; i < dwBytesRead; i++)
			if (buffer[i] == ' ')word_count++;
		wprintf(L"WordCount is equal to %d\n", word_count);
	}
	return 0;
}
DWORD WINAPI ByteCount(void* p){
	while (1) {
		WaitForSingleObject(event, INFINITE);
		DWORD count = dwBytesRead * sizeof(CHAR) - 2;
		wprintf(L"Byte Count is equal to %d\n", count);
	}
	return 0;
}
DWORD WINAPI CapitalPrint(void* p){
	while (1) {
		WaitForSingleObject(event, INFINITE);
		for (int i = 0; i < dwBytesRead; i++)
			if ('a' <= buffer[i] && buffer[i] <= 'z')buffer[i] -= 32;
		DWORD dwBytesWritten;
		if (!WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), buffer, dwBytesRead * sizeof(CHAR), &dwBytesWritten, NULL)) {
			wprintf(L"Write\n");
			MYPerror();
		}
		wprintf(L"\n");
	}
	return 0;
}
void read() {
	if (!ReadFile(GetStdHandle(STD_INPUT_HANDLE), buffer, BUFFSIZE * sizeof(CHAR), &dwBytesRead, NULL)) {
		wprintf(L"Error during read\n");
		MYPerror();
		exit(1);
	}
}

int wmain(int argc, WCHAR* argv[]) {
	DWORD thread_ids[3];
	event = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (event == NULL)
	{
		wprintf(L"CreateEvent failed\n");
		MYPerror();
		exit(1);
	}
	threads[0] = CreateThread(NULL, 0, WordCount, NULL, 0, &thread_ids[0]);
	threads[1] = CreateThread(NULL, 0, ByteCount, NULL, 0, &thread_ids[1]);
	threads[2] = CreateThread(NULL, 0, CapitalPrint, NULL, 0, &thread_ids[2]);
	while (isContinue) {
		buffer = (CHAR*)malloc(sizeof(CHAR)*BUFFSIZE);
		read();
		if (!SetEvent(event)) {
			wprintf(L"Set error \n");
		}
		if (!ResetEvent(event)) {
			wprintf(L"Reset error \n");
		}
		Sleep(100);
		free(buffer);
	}
	WaitForMultipleObjects(3, threads, TRUE, INFINITE);
	for (int i = 0; i < 3; i++)
		CloseHandle(threads[i]);
	getchar();
	return 0;
}