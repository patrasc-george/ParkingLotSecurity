#include "Vehicle.h"

int Vehicle::getId() const
{
	return id;
}

std::string Vehicle::getPath() const
{
	return path;
}

std::string Vehicle::getLicensePlate() const
{
	return licensePlate;
}

std::string Vehicle::getTime() const
{
	return time;
}

void Vehicle::setTimeParked(const std::string& stayedTime)
{
	this->timeParked = stayedTime;
}

std::string Vehicle::getTimeParked() const
{
	return timeParked;
}

std::ostream& operator<<(std::ostream& os, const Vehicle& vehicle)
{
	os << vehicle.getId() << std::endl;
	os << vehicle.getPath() << std::endl;
	os << vehicle.getLicensePlate() << std::endl;
	os << vehicle.getTime() << std::endl;
	os << vehicle.getTimeParked() << std::endl;

	if (!vehicle.getTimeParked().empty())
		os << std::endl;

	return os;
}