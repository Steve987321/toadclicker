#include "pch.h"
#include "Helpers.h"

namespace toad
{
	std::string GetDateStr(std::string_view format)
	{
		std::ostringstream ss;
		std::string time;

		auto t = std::time(nullptr);
		tm newtime{};

#ifdef _WIN32
		localtime_s(&newtime, &t);
#else
		localtime_r(&t, &newtime);
#endif

		ss << std::put_time(&newtime, format.data());
		return ss.str();
	}
}