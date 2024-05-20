#ifndef __IMPORTER_H__
#define __IMPORTER_H__

#include "extras.h"

typedef FS::path::iterator PathIterator;

PathIterator operator + (PathIterator it, uint32_t steps);

bool maskMatch(const FS::path &mask, PathIterator maskIt, const FS::path &path, PathIterator pathIt);
bool parseInstructions(FILE* file, std::vector <FS::path> &instructions);

#endif