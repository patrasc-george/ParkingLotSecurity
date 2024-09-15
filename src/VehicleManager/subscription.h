#pragma once

#ifdef SUBSCRIPTION_EXPORTS
#define SUBSCRIPTION_API __declspec(dllexport)
#else
#define SUBSCRIPTION_API __declspec(dllimport)
#endif

#include <string>
#include <vector>

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
