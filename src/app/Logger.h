#pragma once

#include <Windows.h>
#include <iostream>
#include <shared_mutex>

static inline bool enable_log_file = false;

enum class log_type
{
	Error = 12,		//red
	Log = 9,		//blue
	Success = 10	//green
};

class logger {
private:
	std::shared_mutex mutex;
	std::ofstream logFile;

public:
	logger()
	{
		if (enable_log_file)
			logFile = std::ofstream("log.txt");
	}
	~logger()
	{
		if (enable_log_file)
			if (logFile.is_open()) logFile.close();
	}

	template <typename T>
	void print(log_type type, T msg) {
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

		if (enable_log_file)
			logFile << msg << std::endl;

		std::cout << msg << std::endl;
	}
	template <typename ... Args>
	void print(log_type type, const char* msg, Args ... args) {
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

		if (enable_log_file)
			logFile << msg << std::endl;

		printf(msg, args...);

		std::cout << std::endl;
	}
};

#ifdef _DEBUG
#define log_debug(x) toad::Application::GetLogger().print(log_type::Log, x)
#define log_error(x) toad::Application::GetLogger().print(log_type::Error, x)
#define log_ok(x) toad::Application::GetLogger().print(log_type::Success, x)
#define log_debugf(x, ...) toad::Application::GetLogger().print(log_type::Log, x, __VA_ARGS__)
#define log_errorf(x, ...) toad::Application::GetLogger().print(log_type::Error, x, __VA_ARGS__)
#define log_okf(x, ...) toad::Application::GetLogger().print(log_type::Success, x, __VA_ARGS__)
#else
#define log_debug(x) NULL
#define log_error(x) NULL
#define log_ok(x) NULL
#define log_debugf(x,...) NULL
#define log_errorf(x,...) NULL
#define log_okf(x,...) NULL
#endif

