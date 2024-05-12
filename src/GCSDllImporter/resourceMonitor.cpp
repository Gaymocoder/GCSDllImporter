#include <typeinfo>
#include <thread>
#include <chrono>

#include "GCSDllImporter/resourceMonitor.h"

using namespace std::chrono_literals;

bool isProcessActive(HANDLE process)
{
    return (WaitForSingleObject(process, 0) == WAIT_TIMEOUT);
}

void waitLaunchedProcess(const FS::path &path, const WIN_PI* pi, const WIN_SI* si)
{
    fprintf(stderr, "The app \"%s\" has been successfully launched (process_id: %lu)\n", path.string().c_str(), pi->dwProcessId);
    WaitForSingleObject(pi->hProcess, INFINITE);
    CloseHandle(pi->hProcess);
    CloseHandle(pi->hThread);
    fprintf(stderr, "Process %lu has been successfully сlosed\n", pi->dwProcessId);
}

template <typename T>
bool checkPtr(T* &ptr, bool &malloced)
{
    if (ptr == nullptr)
    {
        malloced = true;
        ptr = (T*) malloc(sizeof(T));
        if (ptr == nullptr) return false;
    }
    return true;
}

std::vector <FS::path> getProcessUsedModules(uint32_t processID)
{
    std::vector <FS::path> _return;
    HANDLE prSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processID);
    if (prSnapshot == INVALID_HANDLE_VALUE)
    {
        fprintf(stderr, "Failed to create snapshot of the process %u modules. Error code: %lu\n", processID, GetLastError());
        return _return;
    }

    MODULEENTRY32 moduleEntry;
    moduleEntry.dwSize = sizeof(MODULEENTRY32);
    uint16_t i = 1;
    if (Module32First(prSnapshot, &moduleEntry))
    {
        do
        {
            fprintf(stderr, "Used module #%u: %s\n", i++, moduleEntry.szExePath);
        } while (Module32Next(prSnapshot, &moduleEntry));
        fprintf(stderr, "\n");
        return _return;
    }
    else
    {
        fprintf(stderr, "Can't get the first module, used by the process %u\nError code: %lu\n", processID, GetLastError());
        return _return;
    }
}

bool launchApp(const FS::path &path, WIN_PI* pi, WIN_SI* si)
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
    
    if (success)
    {
        while (isProcessActive(pi->hProcess))
        {
            getProcessUsedModules(pi->dwProcessId);
            std::this_thread::sleep_for(500ms);
        }
        if (local_pi) free(pi);
        if (local_si) free(si);
        return true;
    }
    else
    {
        fprintf(stderr, "Failed to create process. Error code: %lu\n", GetLastError());
        if (local_pi) free(pi);
        if (local_si) free(si);
        return false;
    }
}