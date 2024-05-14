#include "GCSDllImporter/resourceMonitor.h"
#include "GCSDllImporter/extras.h"

#include <tlhelp32.h>

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

// If NULL has been passed as pi-argument,
// function will wait untill the process exits
// to close the process's and thread's handles by itself
bool startApp(const FS::path &path, WIN_PI* pi, WIN_SI* si)
{
    bool local_pi = false,
         local_si = false;

    if (!checkPtr(pi, local_pi))
    {
        fprintf(stderr, "Cannot allocate memory for PROCESS_INFORMATION\n");
        return false;
    }
    
    if (!checkPtr(si, local_si))
    {
        fprintf(stderr, "Cannot allocate memory for STARTUPINFO\n");
        return false;
    }

    ZeroMemory(si, sizeof(*si));
    ZeroMemory(pi, sizeof(*pi));
    si->cb = sizeof(*si);
    
    BOOL success = CreateProcessA(path.string().c_str(), NULL, NULL, NULL, TRUE, 0, NULL, NULL, si, pi);
    
    if (!success)
    {
        fprintf(stderr, "Failed to create process. Error code: %lu\n", GetLastError());
        if (local_pi) free(pi);
        if (local_si) free(si);
        return false;
    }

    if (!local_pi)
    {
        if (local_si) free(si);
        return true;
    }

    WaitForSingleObject(pi->hProcess, INFINITE);
    CloseHandle(pi->hProcess);
    CloseHandle(pi->hThread);
    if (local_si) free(si);
    free(pi);

    return true;
}