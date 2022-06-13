#pragma once

#include <Windows.h>
#include <iostream>
#include <shared_mutex>

enum class log_type_t
{
	Error = 12,		//red
	Log = 9,		//blue
	Success = 10	//green
};

class logger {
private:
	std::shared_mutex mutex;

public:
	template <typename T>
	void print(log_type_t type, T msg) {
		std::unique_lock<std::shared_mutex> lock(mutex);
		static HANDLE h_console = GetStdHandle(STD_OUTPUT_HANDLE);

		switch (type) {
		case log_type_t::Error:
			SetConsoleTextAttribute(h_console, (WORD)type);
			std::cout << "[!]";
			break;
		case log_type_t::Success:
			SetConsoleTextAttribute(h_console, (WORD)type);
			std::cout << "[+]";
			break;
		case log_type_t::Log:
			SetConsoleTextAttribute(h_console, (WORD)type);
			std::cout << "[.]";
			break;
		}

		if (type == log_type_t::Log) { SetConsoleTextAttribute(h_console, 8); } //gray
		else SetConsoleTextAttribute(h_console, 15); // white

		std::cout << msg << std::endl;
	}
	template <typename ... Args>
	void print(log_type_t type, const char* msg, Args ... args) {
		std::unique_lock<std::shared_mutex> lock(mutex);
		static HANDLE h_console = GetStdHandle(STD_OUTPUT_HANDLE);

		switch (type) {
		case log_type_t::Error:
			SetConsoleTextAttribute(h_console, (WORD)type);
			std::cout << "[!]";
			break;
		case log_type_t::Success:
			SetConsoleTextAttribute(h_console, (WORD)type);
			std::cout << "[+]";
			break;
		case log_type_t::Log:
			SetConsoleTextAttribute(h_console, (WORD)type);
			std::cout << "[.]";
			break;
		}

		if (type == log_type_t::Log) { SetConsoleTextAttribute(h_console, 8); } //gray
		else SetConsoleTextAttribute(h_console, 15); // white

		printf(msg, args...);

		std::cout << std::endl;
	}
};

#ifdef _DEBUG	

inline auto p_log = std::make_unique<logger>();

#define log_debug(x) p_log->print(log_type_t::Log, x)
#define log_error(x) p_log->print(log_type_t::Error, x)
#define log_ok(x) p_log->print(log_type_t::Success, x)
#define log_debugf(x, ...) p_log->print(log_type_t::Log, x, __VA_ARGS__)
#define log_errorf(x, ...) p_log->print(log_type_t::Error, x, __VA_ARGS__)
#define log_okf(x, ...) p_log->print(log_type_t::Success, x, __VA_ARGS__)
#else
#define log_debug(x) NULL
#define log_error(x) NULL
#define log_ok(x) NULL
#define log_debugf(x,...) NULL
#define log_errorf(x,...) NULL
#define log_okf(x,...) NULL
#endif

