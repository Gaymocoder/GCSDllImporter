#include "GCSDllImporter/extras.h"

#include <cstdint>
#include <iostream>

bool isProcessActive(HANDLE process)
{
    return (WaitForSingleObject(process, 0) == WAIT_TIMEOUT);
}