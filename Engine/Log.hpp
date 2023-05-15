#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <format>
#include <string_view>
#include "Color.hpp"

#ifdef _DEBUG
constinit const static bool debug{ true };
#else
constinit const static bool debug{ false };
#endif
namespace Log
{
	static std::ofstream file;
	static std::ofstream filelast;
	void init();
	void flush();
	void deinit();
	template<class... T>
	void debug(std::format_string<T...> fmt, T&&... args)
	{
		if constexpr (::debug)
		{
			auto str = "[DEBUG]: " + std::format(fmt, std::forward<T>(args)...);
			file << str;
			filelast << str;
			std::cout << hue::blue << str << hue::reset;
		}
	}
	template<class... T>
	void error(std::format_string<T...> fmt, T&&... args)
	{
		auto str = "[ERROR]: " + std::format(fmt, std::forward<T>(args)...);
		file << str;
		filelast << str;
		std::cout << hue::red << str << hue::reset;
	}
	template<class... T>
	void warning(std::format_string<T...> fmt, T&&... args)
	{
		auto str = "[WARNING]: " + std::format(fmt, std::forward<T>(args)...);
		file << str;
		filelast << str;
		std::cout << hue::yellow << str << hue::reset;
	}
	template<class... T>
	void info(std::format_string<T...> fmt, T&&... args)
	{
		auto str = "[INFO]: " + std::format(fmt, std::forward<T>(args)...);
		file << str;
		filelast << str;
		std::cout << hue::green << str << hue::reset;
	}
	template<class... T>
	void verbose(std::format_string<T...> fmt, T&&... args)
	{
		auto str = "[VERBOSE]: " + std::format(fmt, std::forward<T>(args)...);
		file << str;
		filelast << str;
		std::cout << hue::grey << str << hue::reset;
	}
}