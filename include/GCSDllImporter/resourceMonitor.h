#include <windows.h>
#include <tlhelp32.h>

#include <vector>
#include <chrono>
#include <filesystem>

namespace FS = std::filesystem;
using namespace std::chrono_literals;

typedef STARTUPINFOA WIN_SI;
typedef PROCESS_INFORMATION WIN_PI;

bool isProcessActive(HANDLE process);
bool startApp(const FS::path &path, WIN_PI* pi, WIN_SI* si);

std::vector <FS::path> getProcessUsedModules(uint32_t processID);
void printProcessUsedModules(uint32_t processID);