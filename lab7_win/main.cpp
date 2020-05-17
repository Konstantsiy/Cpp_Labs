#include <iostream>
#include <string>
#include "fs.h"

using namespace std;

int main() {
	string command;
	FS fileSystem;
	cout << "To see all commands you can enter 'help'" << endl;
	while (true) {
		cout << "> ";
		cin >> command;
		if (command == "exit") break;
		else if (command == "help")
			fileSystem.Help();
		else if (command == "dir")
			fileSystem.ShowFiles();
		else if (command == "addfile")
			fileSystem.CreateNewFile();
		else if (command == "remove")
			fileSystem.RemoveFile();
		else if (command == "open")
			fileSystem.OpenFile();
		else if (command == "clear")
			system("cls");
		else cout << " Unknown command" << endl;
	}
	return 0;
}