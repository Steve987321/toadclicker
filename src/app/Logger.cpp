#include "pch.h"
#include "Logger.h"

namespace toad
{

Logger::Logger()
{
#ifdef _WIN32
	m_stdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
#endif

	if (create_log_file)
	{
		m_logFile.open("Toad.log", std::fstream::out);

		LogToFile(GetDateStr("%Y %d %b \n"));
	}
}

Logger::~Logger()
{
	DisposeLogger();
}

void Logger::DisposeLogger()
{
	std::lock_guard lock(m_closeMutex);

	if (m_logFile.is_open())
		m_logFile.close();

	if (m_isConsoleClosed)
		return;

#ifdef _WIN32
	CloseHandle(m_stdoutHandle);
	m_stdoutHandle = nullptr;
#endif

	m_isConsoleClosed = true;
}

void Logger::LogToFile(std::string_view str)
{
	m_logFile << str << std::endl;
}

void Logger::SetConsoleColor(CONSOLE_COLOR col) const
{
#ifdef _WIN32
	SetConsoleTextAttribute(m_stdoutHandle, static_cast<WORD>(col));
#else
	std::cout << "\033[" << std::to_string(static_cast<unsigned short>(col)) << 'm';
#endif
}

}


