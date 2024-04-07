#include <windows.h>
#include <iostream>
#include <filesystem>

namespace FS = std::filesystem;

void dllMonitor() {}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <path-to-PE-file>\n", argv[0]);
        return 1;
    }

    FS::path fullAppPath = FS::path();
    try
    {
        fullAppPath = FS::canonical(argv[1]);
    }
    catch(FS::filesystem_error)
    {
        fprintf(stderr, "Invalid path has been requested:\nFile \"%s\" not found\n", fullAppPath.string().c_str());
        return 1;
    }

    if (fullAppPath.extension() != ".exe")
    {
        fprintf(stderr, "Invalid path has been requested:\nFile \"%s\" is not executable\n", fullAppPath.string().c_str());
        return 1;
    }

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));
    si.cb = sizeof(si);
    
    BOOL success = CreateProcessA(fullAppPath.string().c_str(), NULL, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);

    if (success)
    {
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    else
    {
        printf("Failed to create process. Error code: %d\n", GetLastError());
    }
}