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

#define LOG_MESSAGE(logLevel) \
    logger.log(logLevel, __FILE__, __LINE__)

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

/**
 * @class Logger
 * @brief Provides logging functionality with customizable output destinations and log levels.
 *
 * This class implements a singleton logger that can output log messages to the console or a text file.
 * It supports multiple log levels (INFO, DEBUG, WARNING, CRITICAL) and provides methods for logging messages
 * with timestamps, file, and line information. The class allows for dynamic configuration of the log output
 * destination and includes overloaded operators to facilitate logging with stream manipulators.
 */
class LOGGER_API Logger
{
private:
	Logger();

	~Logger() {};

	Logger(const Logger&) = delete;

	Logger& operator=(const Logger&) = delete;

public:
	/**
	 * @brief Returns the singleton instance of the Logger class.
	 * @details This function ensures that only one instance of the Logger class is created
	 *          and it is returned whenever called. It uses the static initialization method.
	 * @return A reference to the singleton instance of the Logger class.
	 */
	static Logger& getInstance();

	/**
	 * @brief Sets the output destination for the log messages.
	 * @details This function allows the user to specify whether the log output should go to the console
	 *          or to a text file.
	 * @param[in] logOutput The log output type (console or text file).
	 * @return void
	 */
	void setLogOutput(const LogOutput& logOutput);

	/**
	 * @brief Logs a message with a specific log level.
	 * @details This function formats and logs a message with a timestamp, file, and line information.
	 *          The log level (INFO, DEBUG, WARNING, CRITICAL) is included in the log message.
	 * @param[in] logLevel The severity level of the log message.
	 * @param[in] file The name of the file where the log was generated.
	 * @param[in] line The line number where the log was generated.
	 * @return A reference to the Logger instance.
	 */
	Logger& log(const LogLevel& logLevel, const std::string& file, const int& line);

	/**
	 * @brief Overloaded operator for logging messages.
	 * @details This operator writes the log message either to the console or a text file
	 *          based on the configured log output destination.
	 * @param[in] message The log message to be written.
	 * @return A reference to the Logger instance.
	 */
	Logger& operator<<(const std::string& message);

	/**
	 * @brief Overloaded operator for handling stream manipulators.
	 * @details This operator allows stream manipulators (like `std::endl`) to be used for logging,
	 *          writing to either the console or a text file depending on the log output destination.
	 * @param[in] manip The stream manipulator to be applied to the log output.
	 * @return A reference to the Logger instance.
	 */
	Logger& operator<<(std::ostream& (*manip)(std::ostream&));

private:
	std::string path;
	LogOutput logOutput;
};
