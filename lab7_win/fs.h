#pragma once
#include <windows.h>
#include <fstream>

#define MAX_FILE_NAME 15
#define BLOCKS_COUNT 100
#define MAX_FILES_COUNT 5
#define BLOCK_DATA_SIZE 120

using namespace std;

typedef struct file_data {
	char name[MAX_FILE_NAME] = { 0 };
	int begining_offset = 0;
};

typedef struct block_of_data {
	byte busy_flag = 0;
	char data[BLOCK_DATA_SIZE] = { 0 };
	int next_offset = 0;
};

struct fs {
	fstream mainFile;
	file_data fd[MAX_FILES_COUNT];
	int available_data;
};

static const char mainFileName[] = "disk.bin";
static const int first_offset = MAX_FILES_COUNT * sizeof(file_data) + 1;
static const int fd_size = first_offset - 1;
