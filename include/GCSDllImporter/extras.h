#ifndef __EXTRAS_H__
#define __EXTRAS_H__

#include <windows.h>

bool isProcessActive(HANDLE process);

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

#endif