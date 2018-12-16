#include <Windows.h>
#include <stdio.h>
#include <time.h>
#include <locale.h>
CRITICAL_SECTION crt_sec;
DWORD army1_soldiers, army2_soldiers;
HANDLE threadHandle0, threadHandle1;
void MYPerror() {
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
}
DWORD WINAPI Army_1(void *p) {
	while (army2_soldiers <= 0) {
		EnterCriticalSection(&crt_sec);
		army1_soldiers += rand()%10;
		army2_soldiers -= rand()%400-1;
		LeaveCriticalSection(&crt_sec);
	}
	wprintf(L"Army_1 win!!!\nDefinitive Soldiers of Army_1 is %d\n", army1_soldiers);
	if (!TerminateThread(threadHandle1, 1))
		MYPerror();
	return 0;
}
DWORD WINAPI Army_2(void *p) {
	while (army1_soldiers <= 0) {
		EnterCriticalSection(&crt_sec);
		army1_soldiers -= rand()%10;
		army2_soldiers += rand()%400-1;
		LeaveCriticalSection(&crt_sec);
	}
	wprintf(L"Army_2 win!!!\nDefinitive Soldiers of Army_2 is %d\n", army2_soldiers);
	if (!TerminateThread(threadHandle0, 1))
		MYPerror();
	return 0;
}

int wmain(int argc, WCHAR** argv) {
	InitializeCriticalSection(&crt_sec);
	EnterCriticalSection(&crt_sec);
	srand(time(NULL));
	army1_soldiers = rand()%1000;
	army2_soldiers = rand()%1000;
	LeaveCriticalSection(&crt_sec);
	DWORD thread0ID;
	threadHandle0 = CreateThread(NULL, 0, Army_1, NULL, 0, &thread0ID);
	if (threadHandle0 == NULL)
	{
		MYPerror();
		exit(1);
	}
	DWORD thread1ID;
	threadHandle1 = CreateThread(NULL, 0, Army_2, NULL, 0, &thread1ID);
	if (threadHandle1 == NULL)
	{
		MYPerror();
		exit(1);
	}
	HANDLE handles[] = { threadHandle0,threadHandle1 };
	WaitForMultipleObjects(2, handles, TRUE, INFINITE);

	CloseHandle(threadHandle0);
	CloseHandle(threadHandle1);
	getchar();
	return 0;

}
