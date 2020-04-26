#include<Windows.h>
#include<iostream>
#include<experimental/filesystem>
#include<filesystem>

using std::experimental::filesystem::directory_iterator;
using std::cin;
using std::cout;
using std::endl;


int main()
{
	HINSTANCE dll = LoadLibraryA((LPCSTR)"MyDLL.dll");// получаем дескриптор области памяти, в котором хранится ddl

	typedef void(*ReadFunc)(HANDLE* hFile, CHAR* buffer,//определение типа функ
		DWORD bufferSize, OVERLAPPED* gOverlapped);
	typedef void(*WriteFunc)(HANDLE* hFile, CHAR* buffer,
		DWORD bytesToWrite, OVERLAPPED* gOverlapped);

	ReadFunc readfunc = (ReadFunc)GetProcAddress(dll, "AsyncReadFile");
	WriteFunc writefunc = (WriteFunc)GetProcAddress(dll, "AsyncWriteFile");

	std::string path;
	cout << "Files dir: ";
	cin >> path;

	std::string pathToOutFile;
	cout << "Path to out file: ";
	cin >> pathToOutFile;

	HANDLE hOutFile = CreateFile((LPCSTR)pathToOutFile.c_str(), GENERIC_WRITE, 0, NULL, CREATE_NEW,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
	if (GetLastError() == ERROR_FILE_EXISTS)
	{
		printf("File already exist\n");
		hOutFile = CreateFile((LPCSTR)path.c_str(), GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
		if (hOutFile == INVALID_HANDLE_VALUE)
		{
			printf("error #%d\n", GetLastError());
			return 1;
		}

	}

	static const DWORD bufferSize = 1024;
	CHAR buffer[bufferSize];
	DWORD offset = 0;
	HANDLE hInFile;

	HANDLE endReadEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	OVERLAPPED gOverlappedRead;
	ZeroMemory(&gOverlappedRead, sizeof(gOverlappedRead));
	gOverlappedRead.Offset = 0;
	gOverlappedRead.OffsetHigh = 0;
	gOverlappedRead.hEvent = endReadEvent;

	OVERLAPPED gOverlappedWrite;
	ZeroMemory(&gOverlappedWrite, sizeof(gOverlappedWrite));
	gOverlappedWrite.Offset = offset;
	gOverlappedWrite.OffsetHigh = 0;
	gOverlappedWrite.hEvent = endReadEvent;


	for (const auto& entry : directory_iterator(path.c_str()))
		if (entry.path().extension() == ".txt")
		{
			std::string filepath = entry.path().string();
			hInFile = CreateFile((LPCSTR)filepath.c_str(), GENERIC_READ, 0, NULL,
				OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
			if (hInFile == INVALID_HANDLE_VALUE)
			{
				printf("error #%d\n", GetLastError());
				return 1;
			}

			readfunc(&hInFile, buffer, bufferSize * sizeof(CHAR), &gOverlappedRead);
			WaitForSingleObject(endReadEvent, INFINITE);
			CloseHandle(hInFile);

			writefunc(&hOutFile, buffer, gOverlappedRead.InternalHigh, &gOverlappedWrite);
			WaitForSingleObject(endReadEvent, INFINITE);
			offset += gOverlappedWrite.InternalHigh;
			gOverlappedWrite.Offset = offset;

		}

	CloseHandle(hOutFile);
	CloseHandle(endReadEvent);
	FreeLibrary(dll);
	system("pause");
	return 0;
}