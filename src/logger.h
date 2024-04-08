#pragma once

#include <Windows.h>
#include <iostream>
#include <shared_mutex>

enum class log_type
{
	Error = 12,		//red
	Log = 9,		//blue
	Success = 10	//green
};

// handles console and file logging 
class Logger 
{
public:
	Logger()
	{
		logFile = std::ofstream("log.txt");
	}
	~Logger()
	{
		if (logFile.is_open()) 
			logFile.close();
	}

	template <typename T>
	void Print(log_type type, T msg) {
		std::unique_lock<std::shared_mutex> lock(mutex);
		static HANDLE h_console = GetStdHandle(STD_OUTPUT_HANDLE);

		switch (type) {
		case log_type::Error:
			SetConsoleTextAttribute(h_console, (WORD)type);
			std::cout << "[!]";
			break;
		case log_type::Success:
			SetConsoleTextAttribute(h_console, (WORD)type);
			std::cout << "[+]";
			break;
		case log_type::Log:
			SetConsoleTextAttribute(h_console, (WORD)type);
			std::cout << "[.]";
			break;
		}

		if (type == log_type::Log) { SetConsoleTextAttribute(h_console, 8); } //gray
		else SetConsoleTextAttribute(h_console, 15); // white

#ifdef _DEBUG
		logFile << msg << std::endl;
#endif
		std::cout << msg << std::endl;
	}

	template <typename ... Args>
	void Print(log_type type, const char* msg, Args ... args) {
		std::unique_lock<std::shared_mutex> lock(mutex);
		static HANDLE h_console = GetStdHandle(STD_OUTPUT_HANDLE);

		switch (type) {
		case log_type::Error:
			SetConsoleTextAttribute(h_console, (WORD)type);
			std::cout << "[!]";
			break;
		case log_type::Success:
			SetConsoleTextAttribute(h_console, (WORD)type);
			std::cout << "[+]";
			break;
		case log_type::Log:
			SetConsoleTextAttribute(h_console, (WORD)type);
			std::cout << "[.]";
			break;
		}

		if (type == log_type::Log) { SetConsoleTextAttribute(h_console, 8); } //gray
		else SetConsoleTextAttribute(h_console, 15); // white

		logFile << msg << std::endl;
		printf(msg, args...);

		std::cout << std::endl;
	}

private:
	std::shared_mutex mutex;
	std::ofstream logFile;

};

#ifdef _DEBUG
inline auto p_log = std::make_unique<Logger>();
#define LOG_DEBUG(x) p_log->Print(log_type::Log, x)
#define LOG_ERROR(x) p_log->Print(log_type::Error, x)
#define LOG_OK(x) p_log->Print(log_type::Success, x)
#define LOG_DEBUGF(x, ...) p_log->Print(log_type::Log, x, __VA_ARGS__)
#define LOG_ERRORF(x, ...) p_log->Print(log_type::Error, x, __VA_ARGS__)
#define LOG_OKF(x, ...) p_log->Print(log_type::Success, x, __VA_ARGS__)
#else
#define LOG_DEBUG(x) NULL
#define LOG_ERROR(x) NULL
#define LOG_OK(x) NULL
#define LOG_DEBUGF(x,...) NULL
#define LOG_ERRORF(x,...) NULL
#define LOG_OKF(x,...) NULL
#endif

