#include "GCSDllImporter/extras.h"
#include "GCSDllImporter/resourceMonitor.h"

#include <psapi.h>
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

bool findProcess(const char* requestedExePath, HANDLE* returnProcess)
{
    DWORD processCount;
    DWORD* PIDs = (DWORD*) malloc (sizeof(DWORD) * 1024);
    if (PIDs == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for PIDs (findProcess-error)\n");
        return false;
    }

    EnumProcesses(PIDs, sizeof(DWORD) * 1024, &processCount);
    PIDs = (DWORD*) realloc(PIDs, (size_t) processCount);
    processCount /= sizeof(DWORD);

    for(uint32_t i = 0; i < processCount && *returnProcess == NULL; ++i)
    {
        HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, PIDs[i]);
        if (process == NULL) continue;

        char* processExePath = (char*) malloc (MAX_PATH + 1);
        if (processExePath == NULL)
        {
            fprintf(stderr, "Failed to allocate memory for processExePath (findProcess-error)\n");
            return false;
        }

        uint32_t pathLen = GetModuleFileNameExA(process, NULL, processExePath, MAX_PATH + 1);
        processExePath = (char*) realloc(processExePath, pathLen + 1);

        if (strcmp(requestedExePath, processExePath) == 0)
            *returnProcess = process;
        free(processExePath);
    }

    free(PIDs);
    return true;
}

HANDLE waitForStart(const FS::path &exePath)
{
    fprintf(stderr, "Waiting for application to start: \"%s\"...\n", exePath.string().c_str());
    HANDLE exeProcess = NULL;
    bool success = findProcess(exePath.string().c_str(), &exeProcess);
    for(; exeProcess == NULL && success; success = findProcess(exePath.string().c_str(), &exeProcess))
        std::this_thread::sleep_for(100ms);
    return exeProcess;
}