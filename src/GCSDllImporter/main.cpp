#include "GCSDllImporter/extras.h"
#include "GCSDllImporter/resourceMonitor.h"

#include <locale>
#include <iostream>

int main(int argc, char** argv)
{
    setlocale(LC_ALL, "");
    wchar_t** wargv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <path-to-EXE-file>\n", argv[0]);
        return 1;
    }

    FS::path fullAppPath = FS::path();
    try
    {
        fullAppPath = FS::canonical(FS::path(wargv[1]));
    }
    catch(FS::filesystem_error &err)
    {
        fprintf(stderr, "Invalid path has been requested:\nFile \"%ls\" not found\n", fullAppPath.wstring().c_str());
        return 2;
    }

    if (fullAppPath.extension() != ".exe")
    {
        fprintf(stderr, "Invalid path has been requested:\nFile \"%ls\" is not executable\n", fullAppPath.wstring().c_str());
        return 3;
    }

    HANDLE exeProcess = waitForStart(fullAppPath);
    if (exeProcess == NULL) return 4;

    std::vector <FS::path> modules;
    if (!trackProcessModules(exeProcess, &modules)) return 5;
    CloseHandle(exeProcess);

    PressEnter();
}