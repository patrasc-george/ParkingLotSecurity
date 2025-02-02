#pragma once

#ifdef _WIN32
#ifdef SUBSCRIPTION_EXPORTS
#define SUBSCRIPTION_API __declspec(dllexport)
#else
#define SUBSCRIPTION_API __declspec(dllimport)
#endif
#elif __linux__
#define SUBSCRIPTION_API __attribute__((visibility("default")))
#else
#define SUBSCRIPTION_API
#endif

#include <string>
#include <vector>
#include <algorithm>

class SUBSCRIPTION_API Subscription
{
public:
	Subscription(const std::string name) : name(name) {}

	Subscription() = default;

public:
	std::string getName() const;

	std::vector<std::string> getDateTimes() const;

	void addDateTime(const std::string& dateTime);

	std::vector<std::string> getVehicles() const;

	bool addVehicle(const std::string& vehicle);

	bool deleteVehicle(const std::string& vehicle);

private:
	std::string name;
	std::vector<std::string> dateTimes;
	std::vector<std::string> vehicles;
};
