#include "logger.h"

#include <ctime>

Logger::Logger()
{
#ifdef _DEBUG
	path = "../../../logs.txt";
#else
	path = "logs.txt"
#endif
}

Logger& Logger::log(const LogLevel& logLevel, const LogOutput& logOutput, const std::string& file, const int& line)
{
	this->logOutput = logOutput;

	std::time_t now = std::time(nullptr);
	char timestamp[20];
	std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

	std::string level;
	switch (logLevel)
	{
	case LogLevel::INFO:
		level = "INFO";
		break;
	case LogLevel::DEBUG:
		level = "DEBUG";
		break;
	case LogLevel::WARNING:
		level = "WARNING";
		break;
	case LogLevel::CRITICAL:
		level = "ERROR";
		break;
	}

	std::string output = "[" + std::string(timestamp) + "] [" + file + ":" + std::to_string(line) + "] " + level + ": ";
	*this << output;

	return *this;
}

Logger& Logger::operator<<(const std::string& message)
{
	if (logOutput == LogOutput::CONSOLE)
		std::cout << message;
	else if (logOutput == LogOutput::TEXT_FILE)
	{
		std::ofstream logFile(path, std::ios::app);
		logFile << message;
		logFile.close();
	}

	return *this;
}

Logger& Logger::operator<<(std::ostream& (*manip)(std::ostream&))
{
	if (logOutput == LogOutput::CONSOLE)
		manip(std::cout);
	else if (logOutput == LogOutput::TEXT_FILE)
	{
		std::ofstream logFile(path, std::ios::app);
		manip(logFile);
		logFile.close();
	}

	return *this;
}
