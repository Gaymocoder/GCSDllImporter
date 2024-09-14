#include "resourceMonitor.h"
#include "importer.h"
#include "extras.h"

#include <locale>
#include <iostream>

int main(int argc, char** argv)
{
    setNormalLocale();
    wchar_t** wargv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <path-to-EXE-file>\n", argv[0]);
        return 1;
    }

    FS::path fullAppPath = FS::weakly_canonical(FS::path(wargv[1]));
    if (!FS::exists(fullAppPath))
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
    if (exeProcess == NULL)
        return 4;

    std::vector <FS::path> modulesSrcPaths;
    if (!trackProcessModules(exeProcess, &modulesSrcPaths))
        return 5;
    CloseHandle(exeProcess);

    std::vector <FS::path> modulesImportPaths;
    const FS::path configFilePath = FS::weakly_canonical("./config.ini");
    if (!configureImport(configFilePath, modulesSrcPaths, modulesImportPaths))
        return 6;

    fprintf(stderr, "Modules to import:\n");
    for(size_t i = 0, modulesCount = modulesSrcPaths.size(); i < modulesCount; ++i)
        fprintf(stderr, "%-125s -> %s\n", modulesSrcPaths[i].string().c_str(), modulesImportPaths[i].string().c_str());

    PressEnter();
}