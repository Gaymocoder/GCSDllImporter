#include "GCSDllImporter/extras.h"
#include "GCSDllImporter/resourceMonitor.h"

#include <chrono>
#include <thread>
#include <iostream>

using namespace std::chrono_literals;

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <path-to-EXE-file>\n", argv[0]);
        return 1;
    }

    FS::path fullAppPath = FS::path();
    try
    {
        fullAppPath = FS::canonical(argv[1]);
    }
    catch(FS::filesystem_error &err)
    {
        fprintf(stderr, "Invalid path has been requested:\nFile \"%s\" not found\n", fullAppPath.string().c_str());
        return 2;
    }

    if (fullAppPath.extension() != ".exe")
    {
        fprintf(stderr, "Invalid path has been requested:\nFile \"%s\" is not executable\n", fullAppPath.string().c_str());
        return 3;
    }

    WIN_PI* pi = (WIN_PI*) malloc (sizeof(WIN_PI));
    if (pi == NULL)
    {
        fprintf(stderr, "Cannot allocate memory for PROCESS_INFORMATION\n");
        return 4;
    }

    bool success = startApp(fullAppPath, pi, NULL);
    if (!success) return 5;

    while (isProcessActive(pi->hProcess))
    {
        std::this_thread::sleep_for(500ms);
        system("cls");
        printProcessUsedModules(pi->dwProcessId);
    }
}