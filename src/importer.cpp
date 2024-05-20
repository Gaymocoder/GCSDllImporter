#include "importer.h"

#include <algorithm>

bool maskMatch(const FS::path &mask, PathIterator maskIt, const FS::path &path, PathIterator pathIt)
{
    for(; maskIt != mask.end() && pathIt != path.end(); ++maskIt, ++pathIt)
    {
        if (maskIt->string() == "*")
        {
            if (++maskIt == mask.end())
                return true;
            --maskIt;

            for(PathIterator checkPathIt = pathIt; checkPathIt != path.end(); ++checkPathIt)
            {
                if (maskMatch(mask, ++maskIt, path, checkPathIt))
                    return true;
                --maskIt;
            }

            return false;
        }
        
        if (maskIt->string() == "?")
            continue;

        if (maskIt->string() != pathIt->string())
            return false;
    }

    return true;
}

bool parseInstructsLine(const std::string &instruct, std::vector <FS::path> &instructs)
{
    uint8_t flags = 0;
    const uint8_t QUOTE_ON    = 1;
    const uint8_t SOURCE_PATH = 2;

    size_t arrowPos = instruct.find("->");

    istructs.resize(instructsSize + 2);
    size_t src = instructs.size() - 2;
    size_t dst = instructs.size() - 1;

    for(size_t i = 0, len = instruct.legnth(); i < len; ++i)
    {
        if (instruct[i] == '"')
        {
            flags ^= QUOTE_ON;
            continue;
        }

        if (i == arrowPos)
        {
            flags ^= SOURCE_PATH;
            continue;
        }

        if (flags & QUOTE_ON)
        {
            instructs[(flags & SOURCE_PATH) ? src : dst] += instruct[i];
            continue;
        }
    }
}


bool parseInstructs(FILE* file, std::vector <FS::path> &instructions)
{
}