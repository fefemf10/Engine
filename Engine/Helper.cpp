#include "Helper.hpp"
#pragma warning(disable:4996)
std::vector<std::string> Helper::split(const std::string& line, std::string delimiter)
{
    std::vector<std::string> splitLine;
    size_t start;
    size_t end = 0;

    while ((start = line.find_first_not_of(delimiter, end)) != std::string::npos)
    {
        end = line.find(delimiter, start);
        splitLine.push_back(line.substr(start, end - start));
    }
    return splitLine;
}

std::vector<std::string> Helper::split(char* line, const char* delim)
{
    std::vector<std::string> splitLine;
    splitLine.reserve(5);
    char* token = std::strtok(line, delim);
    while (token) {
        splitLine.push_back(token);
        token = std::strtok(nullptr, delim);
    }
    return splitLine;
}
void strtok(std::vector<const char*>& splitLine, char* line, char delim)
{
    int start{}, i{};
    char lastChar{1};
    while (line[start + i] != 0 && (lastChar != 0 && lastChar != '\n'))
    {
        while (line[start + i] != delim && (line[start + i] != 0 && line[start + i] != '\n'))
        {
            ++i;
        }
        lastChar = line[start + i];
        line[start + i] = 0;
        if (i)
        {
            char* buf = new char[i + 1];
            memcpy(buf, &line[start], i + 1);
            splitLine.emplace_back(buf);
        }
        start += i + 1;
        i = 0;
    }
}
std::vector<const char*> Helper::splitc(char* line, const char* delim)
{
    std::vector<const char*> splitLine;
    splitLine.reserve(5);
    strtok(splitLine,line, *delim);
    return splitLine;
}