#include "pch.h"
#include "Header.h"
#include <Windows.h>

#define BUFFER_SIZE 256

void Converter(char source[BUFFER_SIZE], wchar_t dest[BUFFER_SIZE]) {
	MultiByteToWideChar(0, 0, source, strlen(source) + 1, dest, strlen(source) + 1);
}

BOOL ReadFileAsync(char path[BUFFER_SIZE], char buffer[BUFFER_SIZE]) {
	wchar_t dest[BUFFER_SIZE];
	Converter(path, dest);
	HANDLE fileHandler = CreateFile(dest, GENERIC_ALL, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
	if (fileHandler == INVALID_HANDLE_VALUE) {
		return FALSE;
	}
	OVERLAPPED ol{ 0 };
	if (ReadFileEx(fileHandler, buffer, BUFFER_SIZE - 1, &ol, [](DWORD, DWORD, LPOVERLAPPED) {}) == FALSE) {
		CloseHandle(fileHandler);
		return FALSE;
	}
	SleepEx(5000, TRUE);
	CloseHandle(fileHandler);
	return TRUE;
}

BOOL WriteFileAsync(char path[BUFFER_SIZE], char buffer[BUFFER_SIZE]) {
	wchar_t dest[BUFFER_SIZE];
	//Converter(path, dest);
	// отображает строку символов в строку UTF-16 (широкие символы)
	MultiByteToWideChar(0, 0, path, strlen(path) + 1, dest, strlen(path) + 1);

	// открываем файл
	HANDLE fileHandler = CreateFile(dest, GENERIC_ALL, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
	if (fileHandler == INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	SetEndOfFile(fileHandler); // перемещаем позицию метки в конец файла (EOF)

	OVERLAPPED ol{ 0 }; // структура для асинхронного ввода/вывода
	// наши файлы не поддержмвают смещение байтов, значит параметры струкнуры игнорируются

	// WriteFileEx - пишет жанные в файл и асинхронно завершается
	if (WriteFileEx(fileHandler, buffer, strlen(buffer), &ol, [](DWORD, DWORD, LPOVERLAPPED) {}) == FALSE) {
		// [](DWORD, DWORD, LPOVERLAPPED) {} - лямюда-функция обраного вызова (для асинхронных операций)
		CloseHandle(fileHandler);
		return FALSE;
	}

	SleepEx(5000, TRUE);
	CloseHandle(fileHandler);
	return TRUE;
}