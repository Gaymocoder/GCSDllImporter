#ifndef __IMPORTER_H__
#define __IMPORTER_H__

#include "extras.h"

FS::path::iterator operator + (FS::path::iterator it, uint32_t steps);

bool maskMatch(const FS::path &mask, uint16_t maskStart, const FS::path &path, uint16_t pathStart);
bool parseInstructions(FILE* file, std::vector <FS::path> &instructions);

#endif