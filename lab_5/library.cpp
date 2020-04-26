#include<Windows.h>
#include<stdio.h>

__declspec(dllexport) void AsyncReadFile(HANDLE* hFile, CHAR* buffer,
	DWORD bufferSize, OVERLAPPED* gOverlapped)
{
	if (!ReadFile(*hFile, buffer, bufferSize * sizeof(CHAR), NULL, gOverlapped))
		if (GetLastError() == ERROR_IO_PENDING)
			printf("Async read started\n");
}

__declspec(dllexport) void AsyncWriteFile(HANDLE* hFile, CHAR* buffer,
	DWORD bytesToWrite, OVERLAPPED* gOverlapped)
{
	if (!WriteFile(*hFile, buffer, bytesToWrite, NULL, gOverlapped))
		printf("Async write started\n");
}