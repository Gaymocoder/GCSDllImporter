#include "resourceMonitor.h"
#include "importer.h"
#include "extras.h"

#include <locale>
#include <iostream>

int main(int argc, char** argv)
{
    const FS::path importerAppDir = getCurrentExeDir();
    if (importerAppDir == "")
        return 1;

    setNormalLocale();
    wchar_t** wargv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <path-to-EXE-file>\n", argv[0]);
        return 2;
    }

    FS::path fullAppPath = FS::weakly_canonical(FS::path(wargv[1]));
    if (!FS::exists(fullAppPath))
    {
        fprintf(stderr, "Invalid path has been requested:\nFile \"%ls\" not found\n", fullAppPath.wstring().c_str());
        return 3;
    }

    if (fullAppPath.extension() != ".exe")
    {
        fprintf(stderr, "Invalid path has been requested:\nFile \"%ls\" is not executable\n", fullAppPath.wstring().c_str());
        return 4;
    }

    HANDLE exeProcess = waitForStart(fullAppPath);
    if (exeProcess == NULL)
        return 5;

    std::vector <FS::path> modulesSrcPaths;
    if (!trackProcessModules(exeProcess, &modulesSrcPaths))
        return 6;
    CloseHandle(exeProcess);

    std::vector <FS::path> modulesImportPaths;
    const FS::path configFilePath = FS::weakly_canonical(importerAppDir / "./config.ini");
    if (!configureImport(configFilePath, modulesSrcPaths, modulesImportPaths))
        return 7;

    fprintf(stderr, "Modules to import:\n");
    size_t modulesCount = modulesSrcPaths.size();
    for(size_t i = 0; i < modulesCount; ++i)
        fprintf(stderr, "%-125s -> %s\n", modulesSrcPaths[i].string().c_str(), modulesImportPaths[i].string().c_str());

    if (!modulesCount)
    {
        fprintf(stderr, "There's no modules to import. ");
        PressEnter();
        return 0;
    }

    fprintf(stdout, "\nImport enumerated modules? ");
    if (!confirmRequest())
    {
        fprintf(stderr, "The import has been canceled. ");
        PressEnter();
        return 0;
    }

    std::error_code err; err.clear();
    size_t succeedCount = 0;
    for(size_t i = 0, modulesCount = modulesSrcPaths.size(); i < modulesCount; ++i)
    {
        FS::create_directories(modulesImportPaths[i].remove_filename());
        FS::copy(modulesSrcPaths[i], modulesImportPaths[i], err);
        if (err.value() == 0) ++succeedCount;
    }

    fprintf(stderr, "%llu dll-s have been imported successfully. ", succeedCount);
    PressEnter();
    return 0;
}