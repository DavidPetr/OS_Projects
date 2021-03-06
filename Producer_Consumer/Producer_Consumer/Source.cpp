#include <Windows.h>
#include <stdio.h>
#include <iostream>

#define ProducerThreadError L"Error Producer Thread Init\n"
#define ConsumerThreadError L"Error Consumer Thread Init\n"
#define SemaphoreError L"Cannot create Semaphore\n"
#define MutexError L"Cannot create Mutex\n"
#define AllocateError L"Allocation fails\n"
#define FULL 0
#define EMPTY 5
#define SemaphoreMaximumCount 5
#define Producers_Count 3
#define Consumers_Count 7
#define BUFFER_SIZE 5
HANDLE full_semaphore;
HANDLE empty_semaphore;
HANDLE* producers, *consumers;
HANDLE mutex;
DWORD* producer_ids, *consumer_ids;
DWORD* buffer = NULL;
DWORD index_buffer = 0;

VOID MYPerror(DWORD error_code = GetLastError()) {

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
VOID assert(void* actual_value, wchar_t* msg, int exit_code)
{
	if (actual_value == NULL)
	{
		wprintf(msg);
		MYPerror();
		exit(exit_code);
	}
}
VOID initilize() {
	assert(producers = (HANDLE*)malloc(sizeof(HANDLE)*Producers_Count), AllocateError, 1);
	assert(consumers = (HANDLE*)malloc(sizeof(HANDLE)*Consumers_Count), AllocateError, 1);
	assert(producer_ids = (DWORD*)malloc(sizeof(DWORD)*Producers_Count), AllocateError, 1);
	assert(consumer_ids = (DWORD*)malloc(sizeof(DWORD)*Consumers_Count), AllocateError, 1);
	assert(mutex = CreateMutex(NULL, FALSE, NULL), MutexError, 1);
	assert(full_semaphore = CreateSemaphore(NULL, FULL, SemaphoreMaximumCount, NULL), SemaphoreError, 1);
	assert(empty_semaphore = CreateSemaphore(NULL, EMPTY, SemaphoreMaximumCount, NULL), SemaphoreError, 1);
	assert(buffer = (DWORD*)malloc(sizeof(DWORD) * BUFFER_SIZE), AllocateError, 1);
	ZeroMemory(buffer, BUFFER_SIZE * sizeof(DWORD));
}
VOID finalize() {
	for (int i = 0; i < Producers_Count; i++)
		CloseHandle(producers[i]);
	for (int i = 0; i < Consumers_Count; i++)
		CloseHandle(consumers[i]);
	free(producers);
	free(consumers);
	free(producer_ids);
	free(consumer_ids);
	CloseHandle(empty_semaphore);
	CloseHandle(full_semaphore);
	CloseHandle(mutex);
	free(buffer);
}
VOID state() {
	for (int i = 0; i < BUFFER_SIZE; i++)
		wprintf(L"%d\n", buffer[i]);
	wprintf(L"\n");
	Sleep(500);
}
DWORD WINAPI producer(void *p) {
	HANDLE needed[2] = { mutex,empty_semaphore };

	while (1) {
		WaitForMultipleObjects(2, needed, TRUE, INFINITE);
		buffer[index_buffer++] = 1;
		state();
		ReleaseMutex(mutex);
		ReleaseSemaphore(full_semaphore, 1, NULL);
	}
	return 0;
}
DWORD WINAPI consumer(void *p) {
	HANDLE needed[2] = { mutex,full_semaphore };

	while (1) {
		WaitForMultipleObjects(2, needed, TRUE, INFINITE);
		buffer[index_buffer--] = 0;
		state();
		ReleaseMutex(mutex);
		ReleaseSemaphore(empty_semaphore, 1, NULL);
	}
	return 0;
}

int wmain(int argc, WCHAR** argv) {

	initilize();
	state();
	for (int i = 0; i < Producers_Count; i++) {
		producers[i]= CreateThread(
			NULL,
			0,
			producer,
			NULL,
			0,
			&producer_ids[i]
		);
		if (producers[i] == NULL)
		{
			wprintf(ProducerThreadError);
			MYPerror();
			exit(1);
		}
	}
	for (int i = 0; i < Consumers_Count; i++) {
		consumers[i] = CreateThread(
			NULL,
			0,
			consumer,
			NULL,
			0,
			&consumer_ids[i]
		);
		if (consumers[i] == NULL)
		{
			wprintf(ConsumerThreadError);
			MYPerror();
			exit(1);
		}
	}
	WaitForMultipleObjects(Producers_Count, producers, TRUE, INFINITE);
	WaitForMultipleObjects(Consumers_Count, consumers, TRUE, INFINITE);
	finalize();
	return 0;
}
