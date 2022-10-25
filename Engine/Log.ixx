#include <chrono>
#include <fstream>
#include <format>
#include <iostream>
export module Log; 

import color;
export namespace Log
{
	std::ofstream file;
	std::ofstream filelast;
	const std::string currentDateTime()
	{
		std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		std::string a(30, '\0');
		ctime_s(a.data(), a.size(), &now);
		return a;
	}
	void init()
	{
		file.open("logs\\" + currentDateTime() + ".log", std::ios::ate);
		filelast.open("logs\\debug.log", std::ios::out | std::ios::trunc);

	}
	void flush()
	{
		file.flush();
		filelast.flush();
	}
	template<class... T>
	void debug(std::_Fmt_string<T...> fmt, T&&... args)
	{
		auto str = "[DEBUG]: " + std::format(fmt, std::forward<T>(args)...);
		file << str;
		filelast << str;
		std::cout << hue::blue << str << hue::reset;
	}
	template<class... T>
	void error(std::_Fmt_string<T...> fmt, T&&... args)
	{
		auto str = "[ERROR]: " + std::format(fmt, std::forward<T>(args)...);
		file << str;
		filelast << str;
		std::cout << hue::red << str << hue::reset;
	}
	template<class... T>
	void warning(std::_Fmt_string<T...> fmt, T&&... args)
	{
		auto str = "[WARNING]: " + std::format(fmt, std::forward<T>(args)...);
		file << str;
		filelast << str;
		std::cout << hue::yellow << str << hue::reset;
	}
	template<class... T>
	void info(std::_Fmt_string<T...> fmt, T&&... args)
	{
		auto str = "[INFO]: " + std::format(fmt, std::forward<T>(args)...);
		file << str;
		filelast << str;
		std::cout << hue::green << str << hue::reset;
	}
	template<class... T>
	void verbose(std::_Fmt_string<T...> fmt, T&&... args)
	{
		auto str = "[VERBOSE]: " + std::format(fmt, std::forward<T>(args)...);
		file << str;
		filelast << str;
		std::cout << hue::grey << str << hue::reset;
	}
	void deinit()
	{
		file.close();
		filelast.close();
	}
};