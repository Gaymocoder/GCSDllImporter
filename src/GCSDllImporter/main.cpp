#include "GCSDllImporter/resourceMonitor.h"

#include <windows.h>
#include <iostream>
#include <filesystem>

namespace FS = std::filesystem;

void dllMonitor() {}

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
        return 1;
    }

    if (fullAppPath.extension() != ".exe")
    {
        fprintf(stderr, "Invalid path has been requested:\nFile \"%s\" is not executable\n", fullAppPath.string().c_str());
        return 1;
    }

    bool success = launchApp(fullAppPath, NULL, NULL);
}