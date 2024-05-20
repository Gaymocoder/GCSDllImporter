#include "extras.h"

#include <cstdint>
#include <iostream>

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
    fprintf(stdout, "Press Enter to exit ");
    getchar();
}