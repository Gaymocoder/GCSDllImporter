#include "extras.h"

#include <memory>
#include <cstdint>
#include <iostream>

#ifdef _WIN32
    #define WIN true
#else
    #define WIN false
#endif

void setNormalLocale()
{
    if (WIN == 1)
        std::system("chcp 65001 > nul");
}

bool eraseFromEndVector(size_t n, std::vector <std::string> &vec)
{
    auto first = vec.begin() + (vec.size() - n);
    vec.erase(first, vec.end());
    return false;
}

void deleteLeadingSpaces(char* str, size_t strLength)
{
    size_t leadingSpaces = 0;
    while (str[leadingSpaces] == ' ') ++leadingSpaces;

    memmove(str, str + leadingSpaces, strLength - leadingSpaces + 1);
    str = (char*) realloc (str, strLength - leadingSpaces + 1);
}

void normalizePath(FS::path &path)
{
    FS::path buf;
    for(auto it = path.begin(); it != path.end(); ++it)
        if (*it != "")
            buf / *it;
    path = buf;
}

bool isProcessActive(HANDLE process)
{
    return (WaitForSingleObject(process, 0) == WAIT_TIMEOUT);
}

void PressEnter()
{
    char tmp = 'x';
    printf("Press Enter to exit ");
    std::cin.get(tmp);
}