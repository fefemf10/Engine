#pragma once
#include <vector>
#include <string>
namespace Helper
{
	std::vector<std::string> split(const std::string& line, std::string delimiter);
	std::vector<std::string> split(char* line, const char* delim);
	std::vector<const char*> splitc(char* line, const char* delim);
};

