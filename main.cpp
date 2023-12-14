#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define SIZE 4096

int main() {
    HANDLE fileMapping = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, SIZE, "SharedMemory");

    if (fileMapping == NULL) {
        perror("CreateFileMapping");
        return 1;
    }

    char *shared_memory = (char *)MapViewOfFile(fileMapping, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, SIZE);

    if (shared_memory == NULL) {
        perror("MapViewOfFile");
        CloseHandle(fileMapping);
        return 1;
    }

    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);

    if (!CreateProcess(NULL, "ChildProcess.exe", NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        perror("CreateProcess");
        CloseHandle(fileMapping);
        return 1;
    }

    printf("Parent writes: %s\n", shared_memory);

    WaitForSingleObject(pi.hProcess, INFINITE);

    UnmapViewOfFile(shared_memory);
    CloseHandle(fileMapping);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return 0;
}