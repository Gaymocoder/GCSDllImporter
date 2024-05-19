#include "importer.h"

#include <algorithm>

/*

"C:/System32/*" -> ""
"*\Qt\?\?\plugins\?\?" -> "./?3/?4"

*/

FS::path::iterator operator + (FS::path::iterator it, uint32_t steps)
{
    for(; steps > 0; --steps)
        ++it;
    return it;
}

bool maskMatch(const FS::path &mask, uint16_t maskStart, const FS::path &path, uint16_t pathStart)
{
    uint16_t maskLength = std::distance(mask.begin(), mask.end());
    uint16_t pathLength = std::distance(path.begin(), path.end());

    for(uint16_t i = maskStart, ii = pathStart; i < maskLength && ii < pathLength; ++i, ++ii)
    {
        if ((mask.begin() + i)->string() == "?")
            continue;

        if ((mask.begin() + i)->string() == "*")
        {
            if (mask.begin() + i + 1 == mask.end())
                return true;

            for(uint16_t iii = ii; iii < pathLength; ++iii)
                if (maskMatch(mask, i+1, path, iii)) return true;
            return false;
        }

        if ((mask.begin() + i)->string() != (path.begin() + ii)->string())
            return false;
    }
}

bool parseInstructions(FILE* file, std::vector <FS::path> &instructions)
{

}