#pragma once

#define BUFFER_SIZE 256

#ifdef ASYNCIO_EXPORTS
#define ASYNCIO_API __declspec(dllexport)
#else
#define ASYNCIO_API __declspec(dllimport)
#endif

using namespace std;

extern "C" ASYNCIO_API BOOL ReadFileAsync(char[BUFFER_SIZE], char[BUFFER_SIZE]);

extern "C" ASYNCIO_API BOOL WriteFileAsync(char[BUFFER_SIZE], char[BUFFER_SIZE]);
