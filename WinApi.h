#ifndef WINAPI_H
#define WINAPI_H
#include <string>
#include <Windows.h>

typedef int (WINAPI* fMsgBox) (HWND, LPCWSTR, LPCWSTR, UINT);
typedef struct {
	fMsgBox pFunc;
	HWND hWnd;
	LPCWSTR lpTextb;
	LPCWSTR lpCaption;
	UINT uType;
} MsgParam;

class WinApi {
private:
	static int __textWriteToFile(std::string);
	static HHOOK _hook;
	static KBDLLHOOKSTRUCT kbdStruct;
	static LRESULT __stdcall __HookCallback(int, WPARAM, LPARAM);
	void __ReleaseHook();
	void __SetHook();
	void __PrintProcessNameAndID(DWORD);
	static void WINAPI __MsgBoxWrap(MsgParam*);
	static void __after_MsgBoxWrap();
public:
	void _Keylogger();
	void _RunMessageBoxInAnotherProcessThread();
	void _EnumSystemProcesses();
	void _DynamicDLLImport();
	void _NotepadChangeTitleExample();
	void _ListFiles(std::string);
	void _InfoAboutSystem();
	void __Help();
	void _ShowWindow();
};

#endif