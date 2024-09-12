#ifndef __IMPORTER_H__
#define __IMPORTER_H__

#include "extras.h"

typedef FS::path::iterator PathIterator;

bool parseInstructs(FILE* iniFile, std::vector <FS::path> &instructions);
bool parseInstructsLine(const std::string &instruct, std::vector <FS::path> &instructs);
bool maskMatch(const FS::path &mask, PathIterator maskIt, const FS::path &path, PathIterator pathIt, std::vector <std::string> &questions);

FS::path destinationSetup(const FS::path &destMask, const std::vector <std::string> &questions);
bool getDestinationPath(const FS::path &src, const FS::path &iniFilePath, FS::path &dest);

#endif