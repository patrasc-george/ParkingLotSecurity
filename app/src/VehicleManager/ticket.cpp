#include "ticket.h"

std::string Ticket::getId() const
{
	return id;
}

std::string Ticket::getPath() const
{
	return path;
}

std::string Ticket::getLicensePlate() const
{
	return licensePlate;
}

std::string Ticket::getDateTime() const
{
	return dateTime;
}