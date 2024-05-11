#include <windows.h>
#include <filesystem>

namespace FS = std::filesystem;

typedef STARTUPINFO WIN_SI;
typedef PROCESS_INFORMATION WIN_PI;

bool launchApp(const FS::path &path, WIN_PI* pi, WIN_SI* si);

uint16_t processDllCount(uint32_t processID);