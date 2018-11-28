#include <Windows.h>
#include <stdio.h>

CRITICAL_SECTION crt_sec;
DWORD n, k;
DWORD result0, result1, result2;
DWORD factorial(DWORD x) {
	DWORD i, f=1;
	for (i = 1; i <= x; i++)f *= i;
	return f;
}
DWORD WINAPI f0(void *p) {
	EnterCriticalSection(&crt_sec);
	result0 = factorial(n);
	LeaveCriticalSection(&crt_sec);
	return 0;
}
DWORD WINAPI f1(void *p) {
	EnterCriticalSection(&crt_sec);
	result1 = factorial(n - k);
	LeaveCriticalSection(&crt_sec);
	return 0;
}
DWORD WINAPI f2(void *p) {
	EnterCriticalSection(&crt_sec);
	result2 = factorial(k);
	LeaveCriticalSection(&crt_sec);
	return 0;
}

int wmain(int argc, WCHAR** argv) {
	InitializeCriticalSection(&crt_sec);
	EnterCriticalSection(&crt_sec);
	n =  _wtoi(argv[1]);
	k =  _wtoi(argv[2]);
	LeaveCriticalSection(&crt_sec);
	DWORD thread0ID;
	HANDLE threadHandle0 = CreateThread(
		NULL,
		0,
		f0,
		NULL,
		0,
		&thread0ID
	);
	if (threadHandle0 == NULL)
	{
		wprintf(L"ERROR 0 %d \n", GetLastError());
		exit(1);
	}
	DWORD thread1ID;
	HANDLE threadHandle1 = CreateThread(
		NULL,
		0,
		f1,
		NULL,
		0,
		&thread1ID
	);
	if (threadHandle1 == NULL)
	{
		wprintf(L"ERROR 1 %d \n", GetLastError());
		exit(1);
	}
	DWORD thread2ID;
	HANDLE threadHandle2 = CreateThread(
		NULL,
		0,
		f2,
		NULL,
		0,
		&thread2ID
	);
	if (threadHandle2 == NULL)
	{
		wprintf(L"ERROR 2 %d \n", GetLastError());
		exit(1);
	}
	HANDLE handles[] = { threadHandle0, threadHandle1, threadHandle2 };
	WaitForMultipleObjects(3, handles, TRUE, INFINITE);
	EnterCriticalSection(&crt_sec);
	DWORD result = result0/ (result1*result2);
	LeaveCriticalSection(&crt_sec);
	wprintf(L"Combination equal to %d\n", result);
	CloseHandle(threadHandle0);
	CloseHandle(threadHandle1);
	CloseHandle(threadHandle2);
	getchar();
	return 0;

}
