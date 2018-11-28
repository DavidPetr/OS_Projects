#include <Windows.h>
#include <stdio.h>
#include <time.h>

CRITICAL_SECTION crt_sec;
DWORD *prime_arr;
DWORD index_prime;
struct interval_info
{
	DWORD inf;
	DWORD sup;
}int0, int1, int2, *interval;
BOOL prime(DWORD x) {
	if (x < 2)return FALSE;
	if (x == 2)return TRUE;
	if (x % 2 == 0)return FALSE;
	DWORD i;
	for (i = 3; i*i <= x; i += 2)
		if (x%i == 0)return FALSE;
	return TRUE;
}
DWORD WINAPI f(void *p) {
	EnterCriticalSection(&crt_sec);
	interval = (interval_info*)p;
	for (int i = interval->inf; i <= interval->sup; i++)
		if (prime(i))
			prime_arr[index_prime++] = i;
	LeaveCriticalSection(&crt_sec);
	return 0;
}

void Bubble(DWORD arr[], DWORD size) {
	DWORD i, j;
	for (i = 0; i < size; i++)
		for (j = i + 1; j<size; j++)
			if (arr[i] > arr[j]) {
				arr[i] ^= arr[j];
				arr[j] ^= arr[i];
				arr[i] ^= arr[j];
			}
	return;
}
int wmain(int argc, WCHAR **argv) {
	/*
	* argv[1] interval infimum
	* argv[2] interval supremum
	* Example we want [20,50] interval 	
	* PrimeNumbers.exe 20 50 
	*/ 
	InitializeCriticalSection(&crt_sec);
	EnterCriticalSection(&crt_sec);
	DWORD inf = _wtoi(argv[1]);
	DWORD sup = _wtoi(argv[2]);
	index_prime = 0;
	int0.inf = inf;
	int0.sup = int0.inf + (sup - inf) / 3;
	int1.inf = int0.sup + 1;
	int1.sup = int1.inf + (sup - inf) / 3;
	int2.inf = int1.sup + 1;
	int2.sup = sup;
	prime_arr = (DWORD*)malloc(sup - inf);
	LeaveCriticalSection(&crt_sec);
	DWORD thread0ID;
	HANDLE threadHandle0 = CreateThread(
		NULL,
		0,
		f,
		&int0,
		0,
		&thread0ID
	);
	if (threadHandle0 == NULL)
	{
		wprintf(L"ERROR %d \n", GetLastError());
		exit(1);
	}
	DWORD thread1ID;
	HANDLE threadHandle1 = CreateThread(
		NULL,
		0,
		f,
		&int1,
		0,
		&thread1ID
	);
	if (threadHandle1 == NULL)
	{
		wprintf(L"ERROR %d \n", GetLastError());
		exit(1);
	}
	DWORD thread2ID;
	HANDLE threadHandle2 = CreateThread(
		NULL,
		0,
		f,
		&int2,
		0,
		&thread2ID
	);
	if (threadHandle2 == NULL)
	{
		wprintf(L"ERROR %d \n", GetLastError());
		exit(1);
	}
	HANDLE handles[] = { threadHandle0,threadHandle1,threadHandle2 };
	WaitForMultipleObjects(3, handles, TRUE, INFINITE);

	EnterCriticalSection(&crt_sec);
	Bubble(prime_arr, index_prime);
	LeaveCriticalSection(&crt_sec);
	DWORD i;
	for (i = 0; i < index_prime; i++) {
		printf("%d ",prime_arr[i]);
	}
	free(prime_arr);
	CloseHandle(threadHandle0);
	CloseHandle(threadHandle1);
	CloseHandle(threadHandle2);
	getchar();
	return 0;

}