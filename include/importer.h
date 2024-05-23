#ifndef __IMPORTER_H__
#define __IMPORTER_H__

#include "extras.h"

typedef FS::path::iterator PathIterator;
typedef std::vector <std::string>

bool parseInstructions(FILE* file, std::vector <FS::path> &instructions);
bool parseInstructsLine(const std::string &instruct, std::vector <FS::path> &instructs);
bool maskMatch(const FS::path &mask, PathIterator maskIt, const FS::path &path, PathIterator pathIt, std::vector <std::string> &questions);

#endif