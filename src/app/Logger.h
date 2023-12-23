#pragma once

#include <Windows.h>
#include <iostream>
#include <shared_mutex>

#include <fstream>
#include <format>

#include "Helpers.h"

#ifdef ERROR
#undef ERROR
#endif

static bool create_log_file = false;

namespace toad
{

class Logger final
{
public:
	Logger();
	~Logger();

public:
#ifdef _WIN32
	enum class CONSOLE_COLOR : unsigned short
	{
		GRAY = 8,
		WHITE = 15,
		RED = 12,
		GREEN = 10,
		BLUE = 9,
		YELLOW = 14,
		MAGENTA = 13,
		DEFAULT = WHITE,
	};
#else
	enum class CONSOLE_COLOR : unsigned short
	{
		GRAY = 37,
		WHITE = 33,
		RED = 31,
		GREEN = 32,
		BLUE = 34,
		YELLOW = 33,
		MAGENTA = 95,
		DEFAULT = WHITE,
	};
#endif

	enum class LOG_TYPE : unsigned short
	{
		DEBUG = static_cast<unsigned short>(CONSOLE_COLOR::BLUE),
		ERROR = static_cast<unsigned short>(CONSOLE_COLOR::RED),
		WARNING = static_cast<unsigned short>(CONSOLE_COLOR::YELLOW),
		EXCEPTION = static_cast<unsigned short>(CONSOLE_COLOR::MAGENTA)
	};

	std::unordered_map<LOG_TYPE, const char*> logTypeAsStr
	{
		{LOG_TYPE::DEBUG, "DEBUG"},
		{LOG_TYPE::ERROR, "ERROR"},
		{LOG_TYPE::EXCEPTION, "EXCEPTION"},
		{LOG_TYPE::WARNING, "WARNING"},
	};

public:
	/// Closes console and log file 
	void DisposeLogger();

public:
	template <typename ... Args>
	void LogDebug(const char* frmt, Args... args)
	{
		Log(frmt, LOG_TYPE::DEBUG, args...);
	}

	template <typename ... Args>
	void LogWarning(const char* frmt, Args... args)
	{
		Log(frmt, LOG_TYPE::WARNING, args...);
	}

	template <typename ... Args>
	void LogError(const char* frmt, Args... args)
	{
		Log(frmt, LOG_TYPE::ERROR, args...);
	}

	template <typename ... Args>
	void LogException(const char* frmt, Args... args)
	{
		Log(frmt, LOG_TYPE::EXCEPTION, args...);
	}

private:
	/// Writes to created log file
	void LogToFile(std::string_view str);

	void SetConsoleColor(CONSOLE_COLOR col) const;

private:
	/// Outputs string to console 
	void Print(const std::string_view str, LOG_TYPE log_type)
	{
		std::cout << '[';
		SetConsoleColor(static_cast<CONSOLE_COLOR>(log_type));
		std::cout << logTypeAsStr[log_type];

		SetConsoleColor(CONSOLE_COLOR::WHITE);
		std::cout << "] [";

		SetConsoleColor(CONSOLE_COLOR::GRAY);
		std::cout << GetDateStr("%H:%M:%S");

		SetConsoleColor(CONSOLE_COLOR::WHITE);
		std::cout << "] ";

		SetConsoleColor(CONSOLE_COLOR::GRAY);
		std::cout << str << std::endl;

		SetConsoleColor(CONSOLE_COLOR::DEFAULT);
	}

	/// Logs formatted string to console and log file
	///
	///	@param frmt Formatted string that gets formatted with the arguments using '{}'
	///	@param log_type Type of log that affects console colors and beginning message of output
	/// @param args Arguments that fit with the formatted string
	template<typename ... Args>
	void Log(const std::string_view frmt, LOG_TYPE log_type, Args&& ... args)
	{
		std::lock_guard lock(m_mutex);

		auto formattedStr = format_str(frmt, args...);

		if (create_log_file)
			LogToFile(GetDateStr("[%T]") + ' ' + formattedStr);

		Print(formattedStr, log_type);
	}

private:
#ifdef _WIN32
	HANDLE m_stdoutHandle{};
#endif

	std::mutex m_mutex{};
	std::mutex m_closeMutex{};

	std::atomic_bool m_isConsoleClosed = false;

	std::ofstream m_logFile{};
};

}


#define LOGDEBUGF(msg, ...) toad::Application::GetLogger().LogDebug(msg, __VA_ARGS__)
#define LOGERRORF(msg, ...) toad::Application::GetLogger().LogError(msg, __VA_ARGS__) 
#define LOGWARNF(msg, ...) toad::Application::GetLogger().LogWarning(msg, __VA_ARGS__) 
#define LOGDEBUG(msg, ...) toad::Application::GetLogger().LogDebug(msg, nullptr)
#define LOGERROR(msg, ...) toad::Application::GetLogger().LogError(msg, nullptr) 
#define LOGWARN(msg, ...) toad::Application::GetLogger().LogWarning(msg, nullptr) 
