#include "WinApi.h"
#include <Windows.h>
#include <iostream>
#include <tchar.h> 
#include <strsafe.h>
#include <psapi.h>
#include <string>


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
		-H : help\n \
		-S : call MessageBoxW\n \
		-I : Info about system\n \
		-L : List files\n \
				also provide path: -L C:\\\\Files\\ \n \
		-N : Change title and text of notepad process \n \
		-E : Ennumerate processes \n";
	exit(0);
}
void WinApi::_ShowWindow() {
	MessageBoxW(NULL, TEXT("Text inside the window"), TEXT("Title of the window"), MB_OK);
}