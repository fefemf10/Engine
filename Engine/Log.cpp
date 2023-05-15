#include "Log.hpp"
#include <chrono>
#pragma warning(disable : 4996)

const std::string currentDateTime()
{
	std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	std::string a(255, '\0');
	ctime_s(a.data(), a.size(), &now);
	return a;
}
void Log::init()
{
	file.open("logs\\" + currentDateTime() + ".log", std::ios::out | std::ios::ate);
	filelast.open("logs\\debug.log", std::ios::out | std::ios::trunc);
}
void Log::flush()
{
	file.flush();
	filelast.flush();
}

void Log::deinit()
{
	file.close();
	filelast.close();
}