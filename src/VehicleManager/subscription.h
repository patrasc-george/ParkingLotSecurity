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
	Subscription(const std::string& name, const std::string& password, const std::string& dateTime)
		: name(name), password(password), dateTime(dateTime) {}

	Subscription() = default;

public:
	std::string getName() const;

	std::string getPassword() const;

	std::vector<std::string> getVehicles() const;

	void addVehicle(const std::string& vehicle);

private:
	std::string name;
	std::string password;
	std::string dateTime;
	std::vector<std::string> vehicles;
};
