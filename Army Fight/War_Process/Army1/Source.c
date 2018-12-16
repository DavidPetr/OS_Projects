#include <windows.h>
#include <stdio.h>
#include <locale.h>
#include <time.h>
#include <wchar.h>
#define MapFileError L"Cannot Create MapFile\n"
#define MapBufferError L"Could not map view of file\n"
#define BUF_SIZE 12
#define BadAlloc L"Allocation is failed \n"
#define ReleaseError L"Cannot Release mutex\n"
#define VICTORY L"Army1 win :)!!!\n our soldiers is %d\n their soldiers is %d\n"
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
		wprintf(L"Army1\n");
		wprintf(msg);
		MYPerror();
		exit(exit_code);
	}
}
VOID assert_b(BOOL actual_value, WCHAR* msg, int exit_code)
{
	if (actual_value == 0)
	{
		wprintf(L"Army1\n");
		wprintf(msg);
		MYPerror();
		exit(exit_code);
	}
}
int own_troops;
int enemy_troops;
int wmain(int argc, WCHAR* argv[]) {
	HANDLE mutex = _wtoi(argv[1]);
	HANDLE hMapFileArmy1 = _wtoi(argv[2]);
	HANDLE hMapFileArmy2 = _wtoi(argv[3]);
	WaitForSingleObject(mutex, INFINITE);

	WCHAR buffer_own_troop[BUF_SIZE], buffer_enemy_troop[BUF_SIZE];
	ZeroMemory(buffer_own_troop, sizeof(buffer_own_troop));
	ZeroMemory(buffer_enemy_troop, sizeof(buffer_enemy_troop));

	LPCTSTR pBufferArmy1, pBufferArmy2;

	pBufferArmy1 = MapViewOfFile(hMapFileArmy1, FILE_MAP_ALL_ACCESS, 0, 0, BUF_SIZE);
	assert(pBufferArmy1, MapBufferError, 1);
	pBufferArmy2 = MapViewOfFile(hMapFileArmy2, FILE_MAP_ALL_ACCESS, 0, 0, BUF_SIZE);
	assert(pBufferArmy2, MapBufferError, 1);

	own_troops = _wtoi(pBufferArmy1);
	enemy_troops = _wtoi(pBufferArmy2);
	ZeroMemory(buffer_own_troop, sizeof(buffer_own_troop));
	ZeroMemory(buffer_enemy_troop, sizeof(buffer_enemy_troop));
	_itow(own_troops, buffer_own_troop, 10);
	_itow(enemy_troops, buffer_enemy_troop, 10);
	CopyMemory(pBufferArmy1, buffer_own_troop, (wcslen(buffer_own_troop) * sizeof(WCHAR)));
	CopyMemory(pBufferArmy2, buffer_enemy_troop, (wcslen(buffer_enemy_troop) * sizeof(WCHAR)));
	//wprintf(L"Army1=%d enemy=%d\n", own_troops, enemy_troops);
	assert_b(ReleaseMutex(mutex), ReleaseError, 1);
	while (enemy_troops > 0 && own_troops > 0) {
		WaitForSingleObject(mutex, INFINITE);
		__try {

			WCHAR buffer_own_troop[BUF_SIZE], buffer_enemy_troop[BUF_SIZE];
			LPCTSTR pBufferArmy1, pBufferArmy2;

			pBufferArmy1 = MapViewOfFile(hMapFileArmy1, FILE_MAP_ALL_ACCESS, 0, 0, BUF_SIZE);
			assert(pBufferArmy1, MapBufferError, 1);
			pBufferArmy2 = MapViewOfFile(hMapFileArmy2, FILE_MAP_ALL_ACCESS, 0, 0, BUF_SIZE);
			assert(pBufferArmy2, MapBufferError, 1);

			own_troops = _wtoi(pBufferArmy1);
			enemy_troops = _wtoi(pBufferArmy2);
			//wprintf(L"Army11=%d enemy=%d\n", own_troops, enemy_troops);
			if (enemy_troops <= 0 || own_troops <= 0)break;
			srand(time(NULL));
			own_troops += rand() % 10;
			enemy_troops -= rand() % 400-1;
			//wprintf(L"Army111=%d enemy=%d\n", own_troops, enemy_troops);
			ZeroMemory(buffer_own_troop, sizeof(buffer_own_troop));
			ZeroMemory(buffer_enemy_troop, sizeof(buffer_enemy_troop));
			_itow(own_troops, buffer_own_troop, 10);
			_itow(enemy_troops, buffer_enemy_troop, 10);
			CopyMemory(pBufferArmy1, buffer_own_troop, (wcslen(buffer_own_troop) * sizeof(WCHAR)));
			CopyMemory(pBufferArmy2, buffer_enemy_troop, (wcslen(buffer_enemy_troop) * sizeof(WCHAR)));
		}
		__finally {
			assert_b(ReleaseMutex(mutex), ReleaseError, 1);
		}
	}
	if (enemy_troops <= 0 && own_troops > 0) {
		wprintf(VICTORY, own_troops, enemy_troops);
	}
	else if (own_troops <= 0)wprintf(L"Army1 lose :(\n");
	else wprintf(L"Army1 error\n");
	return 0;
}