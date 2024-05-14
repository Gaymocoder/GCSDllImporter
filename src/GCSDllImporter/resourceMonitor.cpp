#include "GCSDllImporter/resourceMonitor.h"
#include "GCSDllImporter/extras.h"

#include <tlhelp32.h>

#include <chrono>
#include <thread>

using namespace std::chrono_literals;

std::vector <FS::path> getProcessUsedModules(uint32_t processID)
{
    std::vector <FS::path> _return;
    MODULEENTRY32 moduleEntry;
    moduleEntry.dwSize = sizeof(MODULEENTRY32);
    HANDLE prSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processID);
    if (prSnapshot == INVALID_HANDLE_VALUE)
    {
        fprintf(stderr, "Failed to create snapshot of the process %u modules. Error code: %lu\n", processID, GetLastError());
        return _return;
    }
    
    BOOL moduleFetched = Module32First(prSnapshot, &moduleEntry);
    for (; moduleFetched; moduleFetched = Module32Next(prSnapshot, &moduleEntry))
        _return.push_back(moduleEntry.szExePath);
    
    if (!_return.size())
        fprintf(stderr, "Can't get the first module, used by the process %u\nError code: %lu\n", processID, GetLastError());

    return _return;
}

void printProcessUsedModules(uint32_t processID)
{
    printf("List of modules, loaded by process %u:\n", processID);
    std::vector <FS::path> modules = getProcessUsedModules(processID);
    for(size_t i = 0, len = modules.size(); i < len; ++i)
        printf("Module %3lli: %s\n", i+1, modules[i].string().c_str());
    printf("\n");
}

HANDLE findProcess(const FS::path &exePath)
{
    return NULL;
}

HANDLE waitForStart(const FS::path &exePath)
{
    HANDLE exeProcess = findProcess(exePath);
    for(; exeProcess == NULL; exeProcess = findProcess(exePath))
        std::this_thread::sleep_for(250ms);
    return exeProcess;
}