#include "extras.h"

#include <cstdint>
#include <iostream>

bool isProcessActive(HANDLE process)
{
    return (WaitForSingleObject(process, 0) == WAIT_TIMEOUT);
}

void PressEnter()
{
    fprintf(stdout, "Press Enter to exit ");
    getchar();
}