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

    HANDLE exeProcess = waitForStart(fullAppPath);
    CloseHandle(exeProcess);
}