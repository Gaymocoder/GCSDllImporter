#include <windows.h>
#include <tlhelp32.h>

#include <vector>
#include <filesystem>

namespace FS = std::filesystem;

typedef STARTUPINFOA WIN_SI;
typedef PROCESS_INFORMATION WIN_PI;

bool launchApp(const FS::path &path, WIN_PI* pi, WIN_SI* si);

std::vector <FS::path> getProcessUsedModules(uint32_t processID);
void printProcessUsedModules(uint32_t processID);