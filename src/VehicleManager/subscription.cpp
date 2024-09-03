#include "subscription.h"

std::string Subscription::getName() const
{
	return name;
}

std::string Subscription::getPassword() const
{
	return password;
}

std::vector<std::string> Subscription::getVehicles() const
{
	return vehicles;
}

void Subscription::addVehicle(const std::string& vehicle)
{
	vehicles.push_back(vehicle);
}
