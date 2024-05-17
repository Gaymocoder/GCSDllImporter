#ifndef __EXTRAS_H__
#define __EXTRAS_H__

#include <windows.h>

#include <string>
#include <vector>
#include <filesystem>

namespace FS = std::filesystem;

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

void PressEnter();
bool isProcessActive(HANDLE process);

#endif