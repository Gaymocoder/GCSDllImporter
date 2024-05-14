#ifndef __RESOURCE_MONITOR_H__
#define __RESOURCE_MONITOR_H__

#include <windows.h>

#include <vector>
#include <cstdint>
#include <filesystem>

namespace FS = std::filesystem;

typedef STARTUPINFOA WIN_SI;
typedef PROCESS_INFORMATION WIN_PI;

std::vector <FS::path> getProcessUsedModules(uint32_t processID);
void printProcessUsedModules(uint32_t processID);

bool startApp(const FS::path &path, WIN_PI* pi, WIN_SI* si);

#endif