#include <iostream>
#include <Windows.h>

#include "WinApi.h"

int main(int argc, char* argv[]) {
	WinApi WinApiObject;

	//std::cout << argc << std::endl;
	//for (int i = 0; i < argc; i++)
	//	std::cout << argv[i] << std::endl;

	if (argc <= 1) {
		WinApiObject.__Help();
	}

	if (!strcmp(argv[1], "-S"))
		WinApiObject._ShowWindow();
	else if (!strcmp(argv[1], "-H"))
		WinApiObject.__Help();
	else if (!strcmp(argv[1], "-I"))
		WinApiObject._InfoAboutSystem();
	else if (!strcmp(argv[1], "-L")) {
		if (argc <= 2)
			WinApiObject.__Help();
		WinApiObject._ListFiles(argv[2]);
	}
	else if (!strcmp(argv[1], "-N"))
		WinApiObject._NotepadChangeTitleExample();
	else if (!strcmp(argv[1], "-D"))
		WinApiObject._DynamicDLLImport();
	else if (!strcmp(argv[1], "-E"))
		WinApiObject._EnumSystemProcesses();
	else if (!strcmp(argv[1], "-R"))
		WinApiObject._RunMessageBoxInAnotherProcessThread();
	else if (!strcmp(argv[1], "-K"))
		WinApiObject._Keylogger();
	else if (!strcmp(argv[1], "-APC"))
		WinApiObject._APCFunction();

	return 0;
}