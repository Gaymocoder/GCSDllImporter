#include "GCSDllImporter/extras.h"
#include "GCSDllImporter/resourceMonitor.h"

#include <psapi.h>
#include <tlhelp32.h>

#include <chrono>
#include <thread>
#include <iostream>
#include <algorithm>

using namespace std::chrono_literals;

std::vector <FS::path> getProcessModules(uint32_t processID)
{
    std::vector <FS::path> _return;
    MODULEENTRY32W moduleEntry;
    moduleEntry.dwSize = sizeof(MODULEENTRY32W);
    HANDLE prSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processID);
    while (prSnapshot == INVALID_HANDLE_VALUE)
    {
        if (GetLastError() == 24)
        {
            prSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processID);
            continue;
        }
        fprintf(stderr, "Failed to create snapshot of the process %u modules. Error code: %lu\n", processID, GetLastError());
        return _return;
    }
    
    BOOL moduleFetched = Module32FirstW(prSnapshot, &moduleEntry);
    for (; moduleFetched; moduleFetched = Module32NextW(prSnapshot, &moduleEntry))
        _return.push_back(FS::path(moduleEntry.szExePath));
    
    if (!_return.size())
        fprintf(stderr, "Can't get the first module, used by the process %u\nError code: %lu\n", processID, GetLastError());

    return _return;
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
        HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | SYNCHRONIZE, FALSE, PIDs[i]);
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
    fprintf(stderr, "The application has been launched with PID%lu. ", GetProcessId(exeProcess));
    return exeProcess;
}

bool trackProcessModules(HANDLE process, std::vector <FS::path> *modules)
{
    fprintf(stderr, "Tracking used modules... ");
    bool local_modules = false;
    if (modules == NULL)
    {
        modules = new std::vector <FS::path> ();
        local_modules = true;
    }

    while (isProcessActive(process))
    {
        std::vector <FS::path> currentModules = getProcessModules(GetProcessId(process));
        if (currentModules.size() == 0) return false;
        for(auto it = currentModules.begin(); it != currentModules.end(); ++it)
            if (std::find(modules->begin(), modules->end(), *it) == modules->end() && it->extension() != ".exe")
            {
                fprintf(stderr, "%3llu. %ls\n", modules->size(), it->wstring().c_str());
                modules->push_back(*it);
            }
        std::this_thread::sleep_for(100ms);
    }

    if (local_modules)
    {
        fprintf(stdout, "Process has been terminated.\n\nModules been used by the application:\n");
        for(size_t i = 0; i < modules->size(); ++i)
            fprintf(stdout, "%3llu. %s\n", i, (*modules)[i].string().c_str());
        delete modules;
    }

    return true;
}