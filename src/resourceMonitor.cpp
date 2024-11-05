#include "resourceMonitor.h"
#include "extras.h"

#include <psapi.h>
#include <tlhelp32.h>

#include <chrono>
#include <thread>
#include <algorithm>

using namespace std::chrono_literals;

std::vector <FS::path> getProcessModules(const HANDLE &process)
{
    std::vector <FS::path> _return;
    HMODULE* modules = (HMODULE*) malloc (sizeof(HMODULE) * 1024);
    if (modules == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for \"modules\" (getProcessModules)\n");
        return _return;
    }

    DWORD modulesCount = 0;
    if (!EnumProcessModulesEx(process, modules, sizeof(HMODULE) * 1024, &modulesCount, LIST_MODULES_ALL))
    {
        fprintf(stderr, "Failed to get modules list of process %lu. Error code: %lu\n", GetProcessId(process), GetLastError());
        free(modules);
        return _return;
    }

    modulesCount /= sizeof(HMODULE);
    modules = (HMODULE*) realloc(modules, sizeof(HMODULE) * modulesCount);
    for(size_t i = 0; i < modulesCount; ++i)
    {
        wchar_t* modulePath = (wchar_t*) malloc (sizeof(wchar_t) * (MAX_PATH+1));
        if (modulePath == NULL)
        {
            fprintf(stderr, "Failed to allocate memory for \"modulePath\" (getProcessModules)\n");
            free(modules);
            return _return;
        }

        uint32_t pathSize = GetModuleFileNameExW(process, modules[i], modulePath, (MAX_PATH+1) * (sizeof(wchar_t)));
        if (pathSize == 0)
        {
            uint32_t error = GetLastError();
            if (error != ERROR_INVALID_HANDLE && error != ERROR_PARTIAL_COPY)
                fprintf(stderr, "Failed to get module path. Error code: %u\n", error);
            free(modulePath);
            continue;
        }

        modulePath = (wchar_t*) realloc(modulePath, (pathSize + 1) * sizeof(wchar_t));
        _return.push_back(FS::path(modulePath));
        free(modulePath);
    }

    free(modules);
    return _return;
}

bool findProcess(const wchar_t* requestedExePath, HANDLE* returnProcess)
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

        uint32_t pathLen = sizeof(wchar_t) * (MAX_PATH + 1);
        wchar_t* processExePath = (wchar_t*) malloc (pathLen);
        if (processExePath == NULL)
        {
            fprintf(stderr, "Failed to allocate memory for processExePath (findProcess-error)\n");
            CloseHandle(process);
            free(PIDs);
            return false;
        }

        pathLen = (GetModuleFileNameExW(process, NULL, processExePath, pathLen) + 1) * sizeof(wchar_t);
        processExePath = (wchar_t*) realloc (processExePath, pathLen);

        if (wcscmp(requestedExePath, processExePath) == 0)
            *returnProcess = process;

        if (*returnProcess == NULL)
            CloseHandle(process);

        free(processExePath);
    }

    free(PIDs);
    return true;
}

HANDLE waitForStart(const FS::path &exePath)
{
    fprintf(stderr, "Waiting for application to start: \"%ls\"...\n", exePath.wstring().c_str());
    HANDLE exeProcess = NULL;
    bool success = findProcess(exePath.wstring().c_str(), &exeProcess);
    for(; exeProcess == NULL && success; success = findProcess(exePath.wstring().c_str(), &exeProcess))
        std::this_thread::sleep_for(100ms);
    if (success) fprintf(stderr, "The application has been launched with PID%lu. ", GetProcessId(exeProcess));
    return exeProcess;
}

bool trackProcessModules(HANDLE process, std::vector <FS::path> *modules)
{
    bool local_modules = false;
    if (modules == NULL)
    {
        modules = new std::vector <FS::path> ();
        local_modules = true;
    }

    fprintf(stderr, "Tracking used modules... \n");
    while (isProcessActive(process))
    {
        std::vector <FS::path> currentModules = getProcessModules(process);
        if (currentModules.size() == 0)
        {
            if (local_modules) delete modules;
            return false;
        }

        for(auto it = currentModules.begin(); it != currentModules.end(); ++it)
        {
            if (std::find(modules->begin(), modules->end(), *it) == modules->end() && it->extension() != ".exe")
            {
                modules->push_back(*it);
                fprintf(stderr, "%3llu. %ls\n", modules->size(), it->wstring().c_str());
            }
        }
        std::this_thread::sleep_for(100ms);
    }

    fprintf(stderr, "\nThe tracking process has been terminated.\n");
    if (local_modules) delete modules;
    return true;
}