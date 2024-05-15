#ifndef __RESOURCE_MONITOR_H__
#define __RESOURCE_MONITOR_H__

#include <windows.h>

#include <vector>
#include <cstdint>
#include <filesystem>

namespace FS = std::filesystem;

typedef STARTUPINFOA WIN_SI;
typedef PROCESS_INFORMATION WIN_PI;

HANDLE waitForStart(const FS::path &exePath);
bool findProcess(const FS::path &exePath, HANDLE* returnProcess);

bool trackProcessModules(HANDLE process, std::vector <FS::path> *modules);
std::vector <FS::path> getProcessModules(uint32_t processID);

#endif