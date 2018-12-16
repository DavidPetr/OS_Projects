#include <windows.h>
#include <stdio.h>
#include <locale.h>
#include <time.h>
#include <wchar.h>
#define EnvironmentError L"Cannot SetEnvironmentVariable\n"
#define ProcessError L"Cannot Create Process\n"
#define MapFileError L"Cannot Create MapFile\n"
#define MapBufferError L"Could not map view of file\n"
#define BUF_SIZE 12
VOID MYPerror() {
	DWORD error_code = GetLastError();
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
	Sleep(5000);
}
VOID assert(LPVOID actual_value, WCHAR* msg, int exit_code)
{
	if (actual_value == NULL)
	{
		wprintf(msg);
		MYPerror();
		exit(exit_code);
	}
}
VOID assert_b(BOOL actual_value, WCHAR* msg, int exit_code)
{
	if (actual_value == 0)
	{
		wprintf(msg);
		MYPerror();
		exit(exit_code);
	}
}

int wmain(int argc, WCHAR* argv[]) {
	SECURITY_ATTRIBUTES secAtt;
	secAtt.nLength = sizeof(SECURITY_ATTRIBUTES);
	secAtt.bInheritHandle = TRUE;
	secAtt.lpSecurityDescriptor = NULL;

	HANDLE mutex = CreateMutex(&secAtt, FALSE, NULL);
	assert(mutex, L"Cannot create mutex\n", 1);

	STARTUPINFO s1, s2;
	PROCESS_INFORMATION p1, p2;
	ZeroMemory(&s1, sizeof(s1));
	ZeroMemory(&s2, sizeof(s2));
	s1.cb = sizeof(s1);
	s2.cb = sizeof(s2);
	ZeroMemory(&p1, sizeof(p1));
	ZeroMemory(&p2, sizeof(p2));
	
	WCHAR cmd1[50] = L"Army1.exe ", cmd2[50] = L"Army2.exe ";
	WCHAR buffer_army1[BUF_SIZE], buffer_army2[BUF_SIZE], buffer_mutex[BUF_SIZE];
	srand(time(NULL));
	_itow(rand()%1000, buffer_army1, 10);
	_itow(rand()%1000, buffer_army2, 10);
	_itow(mutex, buffer_mutex, 10);
	//swprintf_s(&cmd1[10], 12, buffer_mutex);
	//swprintf_s(&cmd2[10], 12, buffer_mutex);
	wcscat(cmd1, buffer_mutex);
	wcscat(cmd2, buffer_mutex);	
	

	HANDLE hMapFileArmy1, hMapFileArmy2;
	LPCTSTR pBufferArmy1, pBufferArmy2;

	hMapFileArmy1 = CreateFileMapping(INVALID_HANDLE_VALUE, &secAtt, PAGE_READWRITE, 0, BUF_SIZE, NULL);
	hMapFileArmy2 = CreateFileMapping(INVALID_HANDLE_VALUE, &secAtt, PAGE_READWRITE, 0, BUF_SIZE, NULL);
	assert(hMapFileArmy1, MapFileError, 1);
	assert(hMapFileArmy2, MapFileError, 1);

	pBufferArmy1 = MapViewOfFile(hMapFileArmy1, FILE_MAP_ALL_ACCESS, 0, 0, BUF_SIZE);
	assert(pBufferArmy1, MapBufferError, 1);
	pBufferArmy2 = MapViewOfFile(hMapFileArmy2, FILE_MAP_ALL_ACCESS, 0, 0, BUF_SIZE);
	assert(pBufferArmy2, MapBufferError, 1);

	CopyMemory(pBufferArmy1, buffer_army1, (wcslen(buffer_army1) * sizeof(WCHAR)));
	CopyMemory(pBufferArmy2, buffer_army2, (wcslen(buffer_army2) * sizeof(WCHAR)));

	wcscat(cmd1, L" ");
	wcscat(cmd2, L" ");
	ZeroMemory(buffer_army1, sizeof(buffer_army1));
	ZeroMemory(buffer_army2, sizeof(buffer_army2));
	_itow(hMapFileArmy1, buffer_army1, 10);
	_itow(hMapFileArmy2, buffer_army2, 10);
	wcscat(cmd1, buffer_army1);
	wcscat(cmd2, buffer_army2);
	wcscat(cmd1, L" ");
	wcscat(cmd2, L" ");
	wcscat(cmd1, buffer_army2);
	wcscat(cmd2, buffer_army1);

	assert_b(CreateProcess(NULL, cmd1, NULL, NULL, TRUE, 0, NULL, NULL, &s1, &p1), ProcessError, 1);	
	assert_b(CreateProcess(NULL, cmd2, NULL, NULL, TRUE, 0, NULL, NULL, &s2, &p2), ProcessError, 1);	

	HANDLE lpHandles[2] = { p1.hProcess ,p2.hProcess };
	WaitForMultipleObjects(2, lpHandles, TRUE, INFINITE);

	UnmapViewOfFile(pBufferArmy1);
	UnmapViewOfFile(pBufferArmy2);
	CloseHandle(p1.hProcess);
	CloseHandle(p1.hThread);
	CloseHandle(p2.hProcess);
	CloseHandle(p2.hThread);
	CloseHandle(hMapFileArmy1);
	CloseHandle(hMapFileArmy2);
	CloseHandle(mutex);
	getchar();
	return 0;
}




