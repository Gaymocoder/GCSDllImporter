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

bool eraseFromEndVector(size_t n, std::vector <std::string> &vec);

void deleteLeadingSpaces(char* str, size_t strLength);

void normalizePath(FS::path &path);

bool isProcessActive(HANDLE process);

void PressEnter();

#endif