#include "subscription.h"

std::string Subscription::getName() const
{
	return name;
}

std::vector<std::string> Subscription::getDateTimes() const
{
	return dateTimes;
}

void Subscription::addDateTime(const std::string& dateTime)
{
	dateTimes.push_back(dateTime);
}

std::vector<std::string> Subscription::getVehicles() const
{
	return vehicles;
}

bool Subscription::addVehicle(const std::string& vehicle)
{
	if (!vehicles.empty())
	{
		auto it = std::find(vehicles.begin(), vehicles.end(), vehicle);

		if (it != vehicles.end())
			return false;
	}

	vehicles.push_back(vehicle);
	return true;
}

bool Subscription::deleteVehicle(const std::string& vehicle)
{
	auto it = std::find(vehicles.begin(), vehicles.end(), vehicle);

	if (it == vehicles.end())
		return false;

	vehicles.erase(it);
	return true;
}
