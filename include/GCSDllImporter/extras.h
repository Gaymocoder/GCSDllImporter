#ifndef __EXTRAS_H__
#define __EXTRAS_H__

#include <windows.h>

#include <locale>
#include <codecvt>
#include <string>

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

bool isProcessActive(HANDLE process);

#endif