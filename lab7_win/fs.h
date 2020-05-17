#pragma once
#include <windows.h>
#include <fstream>

#define MAX_FILE_NAME_LENGTH 20
#define AMOUNT_OF_BLOCKS 1000
#define MAX_AMOUNT_OF_FILES 100
#define BLOCK_DATA_SIZE 120

using namespace std;

//static const int MAX_AMOUNT_OF_FILES = 100;
//static const int AMOUNT_OF_BLOCKS = 1000;

typedef struct FileMetaDataTableField {
	char name[MAX_FILE_NAME_LENGTH] = { 0 };
	int begining_offset = 0;
};

typedef struct Block {
	byte busy_flag = 0;
	char data[BLOCK_DATA_SIZE] = { 0 };
	int next_offset = 0;
};

class FS {
private:
	std::fstream sourceFile;
	FileMetaDataTableField fmdt[MAX_AMOUNT_OF_FILES];
	int freeSpace;

	void SaveFMDT();
	void SaveBlock(Block block, int offset);
	Block ReadBlock(int offset);
	void ClearBlock(Block &block);
	int FindFreeBlock(bool next);
	void CopyString(char* destinantion, int destination_max_size, const char* source);
	std::string FileToBuffer(int begining_offset);
	std::string ProcessFileBuffer(std::string);
	void SaveFile(std::string, int begining_offset);

public:
	static const char sourceName[];

	typedef char byte;
	typedef FileMetaDataTableField* FileMetaDataTable;

	static const int firstBlockOffset = MAX_AMOUNT_OF_FILES * sizeof(FileMetaDataTableField) + 1;
	static const int fmdtSize = firstBlockOffset - 1;

	FS();
	~FS();

	void Help();
	void ShowFiles();
	void CreateNewFile();
	void RemoveFile();
	void OpenFile();
	
};




