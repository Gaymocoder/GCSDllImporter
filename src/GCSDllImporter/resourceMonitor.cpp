#include <typeinfo>
#include "GCSDllImporter/resourceMonitor.h"

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
        waitLaunchedProcess(path, pi, si);
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