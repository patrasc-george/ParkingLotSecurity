#pragma once

#ifdef _WIN32
#ifdef LOGGER_EXPORTS
#define LOGGER_API __declspec(dllexport)
#else
#define LOGGER_API __declspec(dllimport)
#endif
#elif __linux__
#define LOGGER_API __attribute__((visibility("default")))
#else
#define LOGGER_API
#endif

#define LOG_MESSAGE(logLevel, logOutput) \
    logger.log(logLevel, logOutput, __FILE__, __LINE__)

#include <iostream>
#include <fstream>
#include <sstream>

enum LogLevel
{
	DEBUG,
	INFO,
	WARNING,
	CRITICAL
};

enum LogOutput
{
	NO_OUTPUT,
	CONSOLE,
	TEXT_FILE
};

class LOGGER_API Logger
{
public:
	Logger();

public:
	Logger& log(const LogLevel& logLevel, const LogOutput& logOutput, const std::string& file, const int& line);

	Logger& operator<<(const std::string& message);

	Logger& operator<<(std::ostream& (*manip)(std::ostream&));

private:
	std::string path;
	LogOutput logOutput;
};
