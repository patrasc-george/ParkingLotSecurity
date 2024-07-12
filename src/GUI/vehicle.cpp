#include "Vehicle.h"

int Vehicle::getId() const
{
	return id;
}

std::string Vehicle::getPath() const
{
	return path;
}

int Vehicle::getTicket() const
{
	return ticket;
}

std::string Vehicle::getLicensePlate() const
{
	return licensePlate;
}

std::string Vehicle::getTime() const
{
	return time;
}

void Vehicle::setTimeParked(const std::string& timeParked)
{
	this->timeParked = timeParked;
}

std::string Vehicle::getTimeParked() const
{
	return timeParked;
}

void Vehicle::setTotalAmount(const int& totalAmount)
{
	this->totalAmount = totalAmount;
}

int Vehicle::getTotalAmount() const
{
	return totalAmount;
}

std::ostream& operator<<(std::ostream& os, const Vehicle& vehicle)
{
	os << vehicle.getId() << std::endl;
	os << vehicle.getPath() << std::endl;
	os << vehicle.getTicket() << std::endl;
	os << vehicle.getLicensePlate() << std::endl;
	os << vehicle.getTime() << std::endl;

	if (!vehicle.getTimeParked().empty())
	{
		os << vehicle.getTimeParked() << std::endl;
		os << vehicle.getTotalAmount() << std::endl;
	}

	os << std::endl;

	return os;
}