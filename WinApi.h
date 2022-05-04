#ifndef WINAPI_H
#define WINAPI_H
#include <string>
#include <Windows.h>

class WinApi {
private:
	void __PrintProcessNameAndID(DWORD);
public:
	void _EnumSystemProcesses();
	void _DynamicDLLImport();
	void _NotepadChangeTitleExample();
	void _ListFiles(std::string);
	void _InfoAboutSystem();
	void __Help();
	void _ShowWindow();
};

#endif