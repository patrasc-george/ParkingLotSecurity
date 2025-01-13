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

void Vehicle::setLicensePlate(const std::string& licensePlate)
{
	this->licensePlate = licensePlate;
}

std::string Vehicle::getDateTime() const
{
	return dateTime;
}

std::string Vehicle::getTicket() const
{
	return ticket;
}

void Vehicle::setTicket(const std::string& ticket)
{
	this->ticket = ticket;
}

std::string Vehicle::getTimeParked() const
{
	return timeParked;
}

void Vehicle::setTimeParked(const std::string& timeParked)
{
	this->timeParked = timeParked;
}

int Vehicle::getTotalAmount() const
{
	return totalAmount;
}

void Vehicle::setTotalAmount(const int& totalAmount)
{
	this->totalAmount = totalAmount;
}

bool Vehicle::getIsPaid()
{
	return isPaid;
}

void Vehicle::setIsPaid()
{
	this->isPaid = true;
}

std::ostream& operator<<(std::ostream& os, const Vehicle& vehicle)
{
	os << vehicle.getId() << std::endl;
	os << vehicle.getPath() << std::endl;
	os << vehicle.getLicensePlate() << std::endl;
	os << vehicle.getDateTime() << std::endl;
	os << vehicle.getTicket() << std::endl;

	if (vehicle.getTimeParked().empty())
		os << "false" << std::endl;
	else
	{
		os << vehicle.getTimeParked() << std::endl;
		os << vehicle.getTotalAmount() << std::endl;
	}

	os << std::endl;

	return os;
}
