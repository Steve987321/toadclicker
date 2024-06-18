#pragma once

#include <Windows.h>
#include <ShlObj.h>

#include <iostream>
#include <shared_mutex>
#include <filesystem>

#include "Helpers.h"

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
		logFile = std::ofstream(GetDocumentsFolder() / "Toad.log");

		LogToFile(toad::get_date_str("%Y %d %b \n"));
	}
	~Logger()
	{
		if (logFile.is_open()) 
			logFile.close();
	}

	template <typename T>
	void Print(log_type type, T msg) {
		std::unique_lock<std::shared_mutex> lock(mutex);

		PrintLogType(type);
		std::cout << ' ';

		std::stringstream ss;
		ss << msg;
		LogStr(ss.str());
	}

	template <typename ... Args>
	void Print(log_type type, std::string_view msg, Args ... args) {
		std::unique_lock<std::shared_mutex> lock(mutex);
		
		PrintLogType(type);
		std::cout << ' ';

#ifdef _DEBUG
		int format_str_size = snprintf(nullptr, 0, msg.data(), args...);
		if (format_str_size <= 0)
		{
			Print(log_type::Error, "[Logger] Error formatting string %s", msg);
			return;
		}
#endif 
		memset(format_buffer, '\0', sizeof(format_buffer));
		snprintf(format_buffer, sizeof(format_buffer), msg.data(), args...);
		LogStr(format_buffer);
	}

	// Logs to file.
	// Writes str and new line
	void LogToFile(const std::string_view str)
	{
		logFile << str << std::endl;
	}

private:
	// logs to file and prints in debug
	void LogStr(std::string_view str)
	{
		LogToFile(str);
		std::cout << str << std::endl;
	}

	std::filesystem::path Logger::GetDocumentsFolder()
	{
		CHAR documents[MAX_PATH];
		HRESULT res = SHGetFolderPathA(nullptr, CSIDL_PERSONAL, nullptr, SHGFP_TYPE_CURRENT, documents);
		if (res == S_OK)
		{
			return documents;
		}
		return "";
	}

	void PrintLogType(log_type type)
	{
		static HANDLE h_console = GetStdHandle(STD_OUTPUT_HANDLE);

		switch (type)
		{
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

		if (type == log_type::Log)
			SetConsoleTextAttribute(h_console, 8);  //gray
		else
			SetConsoleTextAttribute(h_console, 15); // white
	}

private:
	std::shared_mutex mutex;
	std::ofstream logFile;

	char format_buffer[256];
};

inline auto p_log = std::make_unique<Logger>();

#define LOG_DEBUG(x) p_log->Print(log_type::Log, x)
#define LOG_ERROR(x) p_log->Print(log_type::Error, x)
#define LOG_OK(x) p_log->Print(log_type::Success, x)
#define LOG_DEBUGF(x, ...) p_log->Print(log_type::Log, x, __VA_ARGS__)
#define LOG_ERRORF(x, ...) p_log->Print(log_type::Error, x, __VA_ARGS__)
#define LOG_OKF(x, ...) p_log->Print(log_type::Success, x, __VA_ARGS__)
