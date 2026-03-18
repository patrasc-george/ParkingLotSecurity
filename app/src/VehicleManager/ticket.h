#pragma once

#include <iostream>

class Ticket
{
public:
	Ticket() = default;

	Ticket(const std::string& id, const std::string& path, const std::string& licensePlate, const std::string& dateTime)
		: id(id), path(path), licensePlate(licensePlate), dateTime(dateTime) {}

public:
	std::string getId() const;

	std::string getPath() const;

	std::string getLicensePlate() const;

	std::string getDateTime() const;

private:
	std::string id;
	std::string path;
	std::string licensePlate;
	std::string dateTime;
};