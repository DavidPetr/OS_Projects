#include <windows.h>
#include <stdio.h>
#define BUFSIZE 4096

int wmain(int argc, WCHAR* argv[], WCHAR** envp) {
	WCHAR *var = L"Path";
	LPWSTR buffer = (LPWSTR)malloc(BUFSIZE * sizeof(WCHAR));
	if (NULL == buffer)return 0;

	DWORD err = GetEnvironmentVariable(var, buffer, BUFSIZE);
	if (err == 0) {
		wprintf(L"Error\n");
		return 0;
	}
	else if (err > BUFSIZE) {
		buffer = (LPWSTR)realloc(buffer, err * sizeof(WCHAR));
		if (NULL == buffer)return 0;
		err = GetEnvironmentVariable(var, buffer, err);
	}
	wcscat(buffer,L"\\");
	wcscat(buffer, argv[1]);
	if (!SetEnvironmentVariable(var, buffer))
	{
		printf("SetEnvironmentVariable failed (%d)\n", GetLastError());
		return FALSE;
	}
	free(buffer);
	LPWSTR buf = (LPWSTR)malloc(BUFSIZE * sizeof(WCHAR));
	GetEnvironmentVariable(var, buf, BUFSIZE);
	wprintf(L"Variable: %s\n", buf);
	free(buf);
	getchar();
	return 0;
}