#include "logger.h"

#include <ctime>

Logger::Logger() : logOutput(CONSOLE)
{
#ifdef _DEBUG
	path = "../../../logs.txt";
#else
	path = "logs.txt";
#endif
}

Logger& Logger::getInstance()
{
	static Logger instance;
	return instance;
}

void Logger::setLogOutput(const LogOutput& logOutput)
{
	this->logOutput = logOutput;
}

Logger& Logger::log(const LogLevel& logLevel, const std::string& file, const int& line)
{
	std::time_t now = std::time(nullptr);
	char timestamp[20];
	std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

	std::string level;
	switch (logLevel)
	{
	case INFO:
		level = "INFO";
		break;
	case DEBUG:
		level = "DEBUG";
		break;
	case WARNING:
		level = "WARNING";
		break;
	case CRITICAL:
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
