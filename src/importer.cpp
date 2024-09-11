#include "importer.h"

#include <algorithm>

bool maskMatch(const FS::path &mask, PathIterator maskIt, const FS::path &path, PathIterator pathIt, std::vector <std::string> &questions)
{
    PathIterator localMaskIt = maskIt;
    PathIterator localPathIt = pathIt;

    for(; maskIt != mask.end() && pathIt != path.end(); ++maskIt, ++pathIt)
    {

        if (maskIt->string() == "*")
        {
            if (++maskIt == mask.end())
                break;
            --maskIt;

            for(PathIterator checkPathIt = pathIt; checkPathIt != path.end(); ++checkPathIt)
            {
                if (maskMatch(mask, ++maskIt, path, checkPathIt, questions)) return true;
                --maskIt;
            }

            return false;
        }

        if (maskIt->string() == "?")
            continue;

        if (maskIt->string() != pathIt->string())
            return false;
    }

    if (pathIt != path.end()) return false;

    for(; localMaskIt != mask.end() && localPathIt != path.end(); ++localMaskIt, ++localPathIt)
        if (localMaskIt->string() == "?") questions.push_back(localPathIt->string());

    return true;
}

bool parseInstructsLine(const std::string &instruct, std::vector <FS::path> &instructs)
{
    uint8_t flags = 0;
    const uint8_t QUOTE_ON = 1;
    const uint8_t SOURCE_PATH = 2;

    size_t arrowPos = instruct.find("->");

    instructs.resize(instructs.size() + 2);
    size_t src = instructs.size() - 2;
    size_t dst = instructs.size() - 1;

    for(size_t i = 0, len = instruct.length(); i < len; ++i)
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

    return true;
}

bool parseInstructs(FILE* file, std::vector <FS::path> &instructs)
{
    const uint16_t bufferLength = 500;
    char* buffer = (char*) malloc (bufferLength);

    while (fgets(buffer, bufferLength, file) != NULL)
    {
        std::string instructStr(buffer);
        parseInstructsLine(instructStr, instructs);
    }

    free(buffer);
    return true;
}

FS::path destinationSetup(const FS::path &destMask, const std::vector <std::string> &questions)
{
    FS::path _return = "";
    for(PathIterator it = destMask.begin(); it != destMask.end(); ++it)
    {
        std::string itStr = it->string();
        if (itStr[0] == '?')
        {
            _return /= questions[(int) itStr[1] - 49];
            continue;
        }
        _return /= *it;
    }

    _return = FS::weakly_canonical(FS::current_path() / _return);
    return _return;
}