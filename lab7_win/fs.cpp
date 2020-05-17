#include "fs.h"
#include <conio.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

const char FS::sourceName[] = "disk.bin";

FS::FS() {
	this->freeSpace = sizeof(Block) * AMOUNT_OF_BLOCKS;
	SecureZeroMemory(this->fmdt, MAX_AMOUNT_OF_FILES * sizeof(FileMetaDataTableField));
	this->sourceFile.open(this->sourceName, ios::binary | ios::in | ios::out | ios::ate);
	if (!this->sourceFile.is_open()) {
		this->sourceFile.open(this->sourceName, ios::binary | ios::out | ios::ate);

		const int empty_file_size = AMOUNT_OF_BLOCKS * sizeof(Block) + this->fmdtSize;
		byte set_arry[empty_file_size];
		SecureZeroMemory(set_arry, empty_file_size);
		this->sourceFile.write(set_arry, empty_file_size);

		this->sourceFile.close();
		this->sourceFile.open(this->sourceName, ios::binary | ios::in | ios::out | ios::ate);
	}
	else {
		this->sourceFile.seekg(0);
		this->sourceFile.read((byte*)this->fmdt, sizeof(FileMetaDataTableField) * MAX_AMOUNT_OF_FILES);
	}
}

FS::~FS() {
	this->sourceFile.close();
}

void FS::Help() {
	cout << endl;
	cout << "-------------------------------" << endl;
	cout << "dir		show all files" << endl;
	cout << "addfile	adding a new file" << endl;
	cout << "remove		removing a component" << endl;
	cout << "reset		reset disk" << endl;
	cout << "clear		clear screen" << endl;
	cout << "open		open file" << endl;
	cout << "exit		exit the programm" << endl;
	cout << "-------------------------------" << endl;
	cout << endl;
}

void FS::ShowFiles() {
	cout << endl << "All files: " << endl;
	for (int i = 0; i < MAX_AMOUNT_OF_FILES; i++) {
		if (this->fmdt[i].begining_offset != 0) {
			cout << '-' << this->fmdt[i].name << endl;
		}
	}
	cout << endl;
}


void FS::CreateNewFile() {
	cout << "Enter file name: ";
	string newFileName;
	cin >> newFileName;
	for (int i = 0; i < MAX_AMOUNT_OF_FILES; i++) {
		if (strcmp(this->fmdt[i].name, newFileName.c_str()) == 0) {
			cout << endl << "File name is already in use, please repeat..." << endl;
			return;
		}
	}

	for (int i = 0; i < MAX_AMOUNT_OF_FILES; i++) {
		if (this->fmdt[i].begining_offset == 0) {
			this->fmdt[i].begining_offset = this->FindFreeBlock(false);
			if (this->fmdt[i].begining_offset == 0)
				break;

			int copy_size = MAX_FILE_NAME_LENGTH < newFileName.length() ? MAX_FILE_NAME_LENGTH : newFileName.length();
			CopyString(this->fmdt[i].name, copy_size, newFileName.c_str());

			this->SaveFMDT();

			Block temp_block;
			temp_block.busy_flag = 1;
			this->SaveBlock(temp_block, this->fmdt[i].begining_offset);

			return;
		}
	}
	cout << endl << endl << "Max amount of files is reached! Free some space, to perform this operation..." << endl << endl;
}

void FS::SaveFMDT() {
	this->sourceFile.seekp(0);
	this->sourceFile.write((byte*)this->fmdt, sizeof(FileMetaDataTableField) * MAX_AMOUNT_OF_FILES);
	this->sourceFile.flush();
}

void FS::SaveBlock(Block block, int offset) {
	this->sourceFile.seekp(offset);
	this->sourceFile.write((byte*)&block, sizeof(Block));
	this->sourceFile.clear();
	this->sourceFile.flush();
	this->sourceFile.clear();
}

void FS::ClearBlock(Block &block) {
	block.next_offset = 0;
	block.busy_flag = 0;
	SecureZeroMemory(block.data, BLOCK_DATA_SIZE);
}

Block FS::ReadBlock(int offset) {
	Block temp_block;
	this->sourceFile.seekg(offset);
	this->sourceFile.read((byte*)&temp_block, sizeof(Block));
	this->sourceFile.clear();
	return temp_block;
}

int FS::FindFreeBlock(bool next) {
	byte current_flag = 0;
	int offset = this->firstBlockOffset;
	for (int i = 0; i < AMOUNT_OF_BLOCKS; i++) {
		this->sourceFile.seekg(offset);
		sourceFile.read(&current_flag, sizeof(byte));
		if (current_flag == 0) {
			if (next) {
				next = false;
				continue;
			}
			return offset;
		}
		offset += sizeof(Block);
	}
	return 0;
}

void FS::CopyString(char* destinantion, int destination_max_size, const char* source) {
	for (int i = 0; i < destination_max_size && source[i] != '\0'; i++) {
		destinantion[i] = source[i];
	}
}

void FS::RemoveFile() {
	cout << "Please, enter the name of a file to delete" << endl;
	cin.clear();
	string deleting_file;
	cin >> deleting_file;
	for (int i = 0; i < MAX_AMOUNT_OF_FILES; i++) {
		if (strcmp(this->fmdt[i].name, deleting_file.c_str()) == 0) {
			int offset = this->fmdt[i].begining_offset;
			while (true) {
				Block temp_block = this->ReadBlock(offset);

				int next_offset = temp_block.next_offset;

				this->ClearBlock(temp_block);

				this->SaveBlock(temp_block, offset);

				if (next_offset == 0)
					break;
				else
					offset = next_offset;
			}
			this->fmdt[i].begining_offset = 0;
			SecureZeroMemory(this->fmdt[i].name, MAX_FILE_NAME_LENGTH);

			this->SaveFMDT();

			return;
		}
	}
	cout << endl << "Incorrect file name, please repeat..." << endl;
}

string FS::FileToBuffer(int begining_offset) {
	string result_str;
	int offset = begining_offset;
	while (true) {
		Block temp_block = this->ReadBlock(offset);

		char temp_buffer[BLOCK_DATA_SIZE + 1] = { 0 };
		this->CopyString(temp_buffer, BLOCK_DATA_SIZE, temp_block.data);

		result_str.append(temp_buffer);

		if (temp_block.next_offset == 0)
			return result_str;
		else
			offset = temp_block.next_offset;
	}
}

void FS::OpenFile() {
	cout << "Please, enter the name of a file" << endl;

	string current_file;
	cin >> current_file;

	for (int i = 0; i < MAX_AMOUNT_OF_FILES; i++) {
		if (strcmp(this->fmdt[i].name, current_file.c_str()) == 0) {
			string fileBuffer = this->FileToBuffer(this->fmdt[i].begining_offset);

			string processed;
			processed = this->ProcessFileBuffer(fileBuffer);

			if (processed.length() == 0) {
				int offset = this->fmdt[i].begining_offset;
				while (true) {
					Block temp_block = this->ReadBlock(offset);

					int next_offset = temp_block.next_offset;

					this->ClearBlock(temp_block);

					this->SaveBlock(temp_block, offset);

					if (next_offset == 0)
						break;
					else
						offset = next_offset;
				}

				Block firstFileBlock;
				firstFileBlock.busy_flag = 1;
				this->SaveBlock(firstFileBlock, this->fmdt[i].begining_offset);

				return;
			}

			this->SaveFile(processed, this->fmdt[i].begining_offset);
			return;
		}
	}
	cout << endl << "Incorrect file name, please repeat..." << endl;
	return;
}

void FS::SaveFile(string processed, int begining_offset)
{
	int unprocessed_length = processed.length();
	int offset = begining_offset;
	while (true) {
		int min_size = unprocessed_length < BLOCK_DATA_SIZE ? unprocessed_length : BLOCK_DATA_SIZE;

		Block temp_block = this->ReadBlock(offset);
		SecureZeroMemory(temp_block.data, BLOCK_DATA_SIZE);
		temp_block.busy_flag = 1;

		this->CopyString(temp_block.data, min_size, processed.c_str());

		this->SaveBlock(temp_block, offset);

		unprocessed_length -= min_size;
		processed.erase(0, min_size);

		if (unprocessed_length == 0) {
			if (temp_block.next_offset != 0) {
				int clear_offset = temp_block.next_offset;
				temp_block.next_offset = 0;
				this->SaveBlock(temp_block, offset);
				while (true) {
					Block clear_block = this->ReadBlock(clear_offset);

					int old_offset = clear_offset;
					clear_offset = clear_block.next_offset;

					this->ClearBlock(clear_block);
					this->SaveBlock(clear_block, old_offset);

					if (clear_offset == 0)
						break;
				}
			}
			else return;
		}

		if (temp_block.next_offset == 0) {
			if (unprocessed_length != 0) {
				temp_block.next_offset = this->FindFreeBlock(false);

				if (temp_block.next_offset == 0) {
					cout << endl << endl << "Not enough space on disk! Some information was lost!" << endl << endl;
					return;
				}

				this->SaveBlock(temp_block, offset);
			}
			else return;
		}
		offset = temp_block.next_offset;
	}
}

string FS::ProcessFileBuffer(string fileBuffer) {
	string processed = fileBuffer;
	while (true) {
		system("cls");
		cout << processed << endl << endl;
		cout << "Choose an option:" << endl;
		cout << "1. Clear file" << endl;
		cout << "2. Remove from position" << endl;
		cout << "3. Add to position" << endl;
		cout << "0. Close file and save changes" << endl;
		switch (_getch()) {
		case '0': {
			return processed;
		}
		case '1': {
			processed.clear();
			break;
		}
		case '2': {
			int delete_position = -1;
			int delete_size = -1;

			while (delete_position < 0 || delete_size < 0 || ((delete_size + delete_position) > processed.length() + 1)) {
				cout << "Please, enter the position, where to start deleting..." << endl;
				cin >> delete_position;

				cout << "Please, enter the size of a fragment..." << endl;
				cin >> delete_size;
			}
			processed.erase(delete_position, delete_size);
			break;
		}
		case '3': {
			cout << "Please, select option:" << endl;
			cout << "1. Append" << endl;
			cout << "2. From position" << endl;
			char symbol = 0;
			while (symbol != '1' && symbol != '2') {
				symbol = _getch();
			}
			string additional;
			cout << "Continue..." << endl;
			cin.ignore();
			cin.clear();
			cout << "Please, enter the text" << endl;
			getline(cin, additional);
			if (symbol == '1') {
				processed.append(additional);
			}
			else {
				int adding_position = -1;
				while (adding_position < 0 || (adding_position > processed.length() - 1)) {
					cout << "Please, enter the position, where to start adding text..." << endl;
					cin >> adding_position;
				}
				processed.insert(adding_position, additional);
			}
			break;
		}
		}
	}
}












