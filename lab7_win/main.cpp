#include <iostream>
#include <string>
#include "fs.h"
#include <conio.h>

using namespace std;

fs _fs;

block_of_data read_block_of_data(int);
string file_to_buffer(int);
string process_file_buffer(string);
void clear_block_of_data(block_of_data&);
void dir();
void init_fs();
int get_free_block(bool);
void save_block_of_data(block_of_data, int);
void save_file_data();
void save_file(string, int);
void copy_str(char*, int, const char*);
void mf();
void rf();
void chf();

void help() {
	cout << "dir" << "\tview current folder" << endl;
	//cout << "cd" << "\tgo to folder" << endl;
	//cout << "md" << "\tcreate new folder" << endl;
	//cout << "rm" << "\tdelete folder" << endl;
	cout << "mf" << "\tcreate new file" << endl;
	cout << "rf" << "\tdelete file" << endl;
	cout << "chf" << "\tchange file" << endl;
	cout << "cls" << "\tclear console" << endl;
	cout << "help" << "\tshow all comands" << endl;
}

int main() {
	string command;
	while (true) {
		cout << ">";
		cin >> command;
		if (command == "exit") break;
		else if (command == "help")
			help();
		else if (command == "dir")
			dir();
		else if (command == "mf")
			mf();
		else if (command == "rf")
			rf();
		else if (command == "chf")
			chf(); // change file
		else if (command == "clear")
			system("cls");
		else cout << " Unknown command" << endl;
	}
}

block_of_data read_block_of_data(int offset) {
	block_of_data temp_block;
	_fs.mainFile.seekg(offset);
	_fs.mainFile.read((char*)&temp_block, sizeof(block_of_data));
	_fs.mainFile.clear();
	return temp_block;
}

string file_to_buffer(int begining_offset) {
	string result_str;
	int offset = begining_offset;
	while (true) {
		block_of_data temp_block = read_block_of_data(offset);

		char temp_buffer[BLOCK_DATA_SIZE + 1] = { 0 };
		copy_str(temp_buffer, BLOCK_DATA_SIZE, temp_block.data);

		result_str.append(temp_buffer);

		if (temp_block.next_offset == 0)
			return result_str;
		else
			offset = temp_block.next_offset;
	}
}

string process_file_buffer(string fileBuffer) {
	string processed = fileBuffer;
	char c;
	while (true) {
		system("cls");
		cout << processed << endl << endl;
		cout << "Choose an option:" << endl;
		cout << "1. Clear file" << endl;
		cout << "2. Remove from position" << endl;
		cout << "3. Add to position" << endl;
		cout << "0. Close file and save changes" << endl;
		cin >> c;
		switch (c) {
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

void clear_block_of_data(block_of_data &block) {
	block.next_offset = 0;
	block.busy_flag = 0;
	SecureZeroMemory(block.data, BLOCK_DATA_SIZE);
}

void dir() {
	if (_fs.fd[0].begining_offset == 0) {
		cout << "dir is clear\n";
		return;
	}
	cout << endl << "root:" << endl;
	for (int i = 0; i < MAX_FILES_COUNT; i++) {
		if (_fs.fd[i].begining_offset != 0) {
			cout << '-' << _fs.fd[i].name << endl;
		}
	}
	cout << endl;
}

void init_fs() {
	_fs.available_data = sizeof(block_of_data) * BLOCKS_COUNT;
	SecureZeroMemory(_fs.fd, MAX_FILES_COUNT * sizeof(file_data));
	_fs.mainFile.open(mainFileName, ios::binary | ios::in | ios::out | ios::ate);
	if (!_fs.mainFile.is_open()) {
		_fs.mainFile.open(mainFileName, ios::binary | ios::out | ios::ate);

		const int empty_file_size = BLOCKS_COUNT * sizeof(block_of_data) + fd_size;
		char set_arry[empty_file_size];
		SecureZeroMemory(set_arry, empty_file_size);
		_fs.mainFile.write(set_arry, empty_file_size);

		_fs.mainFile.close();
		_fs.mainFile.open(mainFileName, ios::binary | ios::in | ios::out | ios::ate);
	}
	else {
		_fs.mainFile.seekg(0);
		_fs.mainFile.read((char*)_fs.fd, sizeof(file_data) * MAX_FILES_COUNT);
	}
}

int get_free_block(bool next) {
	char current_flag = 0;
	int offset = first_offset;
	for (int i = 0; i < BLOCKS_COUNT; i++) {
		_fs.mainFile.seekg(offset);
		_fs.mainFile.read(&current_flag, sizeof(byte));
		if (current_flag == 0) {
			if (next) {
				next = false;
				continue;
			}
			return offset;
		}
		offset += sizeof(block_of_data);
	}
	return 0;
}

void save_file_data() {
	_fs.mainFile.seekp(0);
	_fs.mainFile.write((char*)_fs.fd, sizeof(file_data) * MAX_FILES_COUNT);
	_fs.mainFile.flush();
}

void save_file(string processed, int begining_offset)
{
	int unprocessed_length = processed.length();
	int offset = begining_offset;
	while (true) {
		int min_size = unprocessed_length < BLOCK_DATA_SIZE ? unprocessed_length : BLOCK_DATA_SIZE;

		block_of_data temp_block = read_block_of_data(offset);
		SecureZeroMemory(temp_block.data, BLOCK_DATA_SIZE);
		temp_block.busy_flag = 1;

		copy_str(temp_block.data, min_size, processed.c_str());

		save_block_of_data(temp_block, offset);

		unprocessed_length -= min_size;
		processed.erase(0, min_size);

		if (unprocessed_length == 0) {
			if (temp_block.next_offset != 0) {
				int clear_offset = temp_block.next_offset;
				temp_block.next_offset = 0;
				save_block_of_data(temp_block, offset);
				while (true) {
					block_of_data clear_block = read_block_of_data(clear_offset);

					int old_offset = clear_offset;
					clear_offset = clear_block.next_offset;

					clear_block_of_data(clear_block);
					save_block_of_data(clear_block, old_offset);

					if (clear_offset == 0)
						break;
				}
			}
			else return;
		}

		if (temp_block.next_offset == 0) {
			if (unprocessed_length != 0) {
				temp_block.next_offset = get_free_block(false);

				if (temp_block.next_offset == 0) {
					cout << endl << endl << "Not enough space on disk! Some information was lost!" << endl << endl;
					return;
				}

				save_block_of_data(temp_block, offset);
			}
			else return;
		}
		offset = temp_block.next_offset;
	}
}

void save_block_of_data(block_of_data block, int offset) {
	_fs.mainFile.seekp(offset);
	_fs.mainFile.write((char*)&block, sizeof(block_of_data));
	_fs.mainFile.clear();
	_fs.mainFile.flush();
	_fs.mainFile.clear();
}

void copy_str(char* destinantion, int destination_max_size, const char* source) {
	for (int i = 0; i < destination_max_size && source[i] != '\0'; i++) {
		destinantion[i] = source[i];
	}
}

void mf() {
	cout << "Enter file name: ";
	string newFileName;
	cin >> newFileName;
	for (int i = 0; i < MAX_FILES_COUNT; i++) {
		if (strcmp(_fs.fd[i].name, newFileName.c_str()) == 0) {
			cout << endl << "File name is already in use, please repeat..." << endl;
			return;
		}
	}

	for (int i = 0; i < MAX_FILES_COUNT; i++) {
		if (_fs.fd[i].begining_offset == 0) {
			_fs.fd[i].begining_offset = get_free_block(false);
			if (_fs.fd[i].begining_offset == 0)
				break;

			int copy_size = MAX_FILE_NAME < newFileName.length() ? MAX_FILE_NAME : newFileName.length();
			copy_str(_fs.fd[i].name, copy_size, newFileName.c_str());

			save_file_data();

			block_of_data temp_block;
			temp_block.busy_flag = 1;
			save_block_of_data(temp_block, _fs.fd[i].begining_offset);
			return;
		}
	}
	cout << endl << endl << "Max amount of files is reached! Free some space, to perform this operation..." << endl << endl;
}

void rf() {
	cout << "Please, enter the name of a file to delete" << endl;
	cin.clear();
	string deleting_file;
	cin >> deleting_file;
	for (int i = 0; i < MAX_FILES_COUNT; i++) {
		if (strcmp(_fs.fd[i].name, deleting_file.c_str()) == 0) {
			int offset = _fs.fd[i].begining_offset;
			while (true) {
				block_of_data temp_block = read_block_of_data(offset);

				int next_offset = temp_block.next_offset;

				clear_block_of_data(temp_block);

				save_block_of_data(temp_block, offset);

				if (next_offset == 0)
					break;
				else
					offset = next_offset;
			}
			_fs.fd[i].begining_offset = 0;
			SecureZeroMemory(_fs.fd[i].name, MAX_FILE_NAME);

			save_file_data();
			return;
		}
	}
	cout << endl << "Incorrect file name, please repeat..." << endl;
}

void chf() {
	cout << "Please, enter the name of a file" << endl;
	string current_file;
	cin >> current_file;
	for (int i = 0; i < MAX_FILES_COUNT; i++) {
		if (strcmp(_fs.fd[i].name, current_file.c_str()) == 0) {
			string fileBuffer = file_to_buffer(_fs.fd[i].begining_offset);

			string processed;
			processed = process_file_buffer(fileBuffer);

			if (processed.length() == 0) {
				int offset = _fs.fd[i].begining_offset;
				while (true) {
					block_of_data temp_block = read_block_of_data(offset);

					int next_offset = temp_block.next_offset;

					clear_block_of_data(temp_block);

					save_block_of_data(temp_block, offset);

					if (next_offset == 0)
						break;
					else
						offset = next_offset;
				}
				block_of_data firstFileBlock;
				firstFileBlock.busy_flag = 1;
				save_block_of_data(firstFileBlock, _fs.fd[i].begining_offset);

				return;
			}
			save_file(processed, _fs.fd[i].begining_offset);
			return;
		}
	}
	cout << endl << "Incorrect file name, please repeat..." << endl;
	return;
}