#include "importer.h"

#include <iostream>
#include <algorithm>
#include <iterator>

bool maskMatch(const FS::path &mask, PathIterator maskIt, const FS::path &path, PathIterator pathIt, std::vector <std::string> &questions)
{
    questions.clear();
    bool brokenFlag = false;
    size_t questionsCount = 0;

    for(; maskIt != mask.end() && pathIt != path.end(); ++maskIt, ++pathIt)
    {
        if (maskIt->string() == "*")
        {
            if (++maskIt == mask.end())
            {
                brokenFlag = true;
                break;
            }
            --maskIt;

            for(PathIterator checkPathIt = pathIt; checkPathIt != path.end(); ++checkPathIt)
            {
                if (maskMatch(mask, ++maskIt, path, checkPathIt, questions))
                {
                    brokenFlag = true;
                    break;
                }
                --maskIt;
            }

            if (brokenFlag) break;
            return eraseFromEndVector(questionsCount, questions);
        }

        if (maskIt->string() == "?")
        {
            questions.push_back(pathIt->string());
            ++questionsCount;
            continue;
        }

        if (*maskIt != *pathIt)
            return eraseFromEndVector(questionsCount, questions);
    }

    if ((pathIt != path.end() || maskIt != mask.end()) && !brokenFlag)
        return eraseFromEndVector(questionsCount, questions);
    return true;
}

bool parseInstructsLine(const std::string &instruct, std::vector <FS::path> &instructs)
{
    uint8_t flags = 2;
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

bool parseInstructs(FILE* configFile, std::vector <FS::path> &instructs)
{
    const uint16_t bufferLength = 500;
    char* buffer = (char*) malloc (bufferLength);
    if (buffer == NULL)
    {
        fprintf(stderr, "parseInstructs: failed to allocate memory for \"char* buffer\"\n");
        return false;
    }

    while (fgets(buffer, bufferLength, configFile) != NULL)
    {
        deleteLeadingSpaces(buffer, strlen(buffer));
        if (*buffer == '\n' || *buffer == '#')
            continue;

        std::string instructStr(buffer);
        parseInstructsLine(instructStr, instructs);
    }

    free(buffer);
    return true;
}

FS::path destinationSetup(const FS::path &destMask, const std::vector <std::string> &questions)
{
    if (destMask == "") return "";

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

    _return = FS::current_path() / _return;
    return _return;
}

bool getDestinationPath(const FS::path &src, const std::vector <FS::path> &instructs, FS::path &dest)
{
    dest = FS::path(".") / src.filename();
    for(auto it = instructs.begin(); it != instructs.end(); it += 2)
    {
        std::vector <std::string> questions = {};
        FS::path lowerCaseMask = toLower(*it);
        FS::path lowerCaseSrc = toLower(src);
        if (maskMatch(lowerCaseMask, lowerCaseMask.begin(), lowerCaseSrc, lowerCaseSrc.begin(), questions))
        {
            dest = destinationSetup(it[1], questions);
            break;
        }
    }

    dest = FS::weakly_canonical(dest);
    return true;
}

bool configureImport(const FS::path &configFilePath, std::vector <FS::path> &srcPaths, std::vector <FS::path> &destPaths)
{
    FILE* configFile = fopen(configFilePath.string().c_str(), "r");
    if (!configFile)
    {
        fprintf(stderr, "Configure importing error: failed to open config file (%s)\n", configFilePath.string().c_str());
        return false;
    }

    std::vector <FS::path> instructs = {};
    parseInstructs(configFile, instructs);
    fclose(configFile);

    destPaths.resize(srcPaths.size());
    for(size_t i = 0; i < srcPaths.size(); ++i)
    {
        getDestinationPath(srcPaths[i], instructs, destPaths[i]);
        if (destPaths[i] == "")
        {
            srcPaths.erase(srcPaths.begin() + i);
            destPaths.erase(destPaths.begin() + i);
            --i;
        }
    }

    return true;
}