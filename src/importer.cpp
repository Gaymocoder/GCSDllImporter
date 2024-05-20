#include "importer.h"

#include <algorithm>

PathIterator operator + (PathIterator it, uint32_t steps)
{
    for(; steps > 0; --steps)
        ++it;
    return it;
}

bool maskMatch(const FS::path &mask, PathIterator maskIt, const FS::path &path, PathIterator pathIt)
{
    for(; maskIt != mask.end() && pathIt != path.end(); ++maskIt, ++pathIt)
    {
        if (maskIt->string() == "?")
            continue;

        if (maskIt->string() == "*")
        {
            if (++maskIt == mask.end())
                return true;
            --maskIt;

            for(PathIterator checkPathIt = pathIt; checkPathIt != path.end(); ++checkPathIt)
            {
                if (maskMatch(mask, ++maskIt, path, checkPathIt)) return true;
                --maskIt;
            }

            return false;
        }

        if (maskIt->string() != pathIt->string())
            return false;
    }

    return true;
}

bool parseInstructions(FILE* file, std::vector <FS::path> &instructions)
{

}