#pragma once

#include <iostream>

class Vehicle
{
public:
	Vehicle() = default;

	Vehicle(const int& id, const std::string& path, const std::string& licensePlate, const std::string& time)
		: id(id), path(path), licensePlate(licensePlate), time(time) {}

	int getId() const;

	std::string getPath() const;

	std::string getLicensePlate() const;

	std::string getTime() const;

	void setTimeParked(const std::string& stayedTime);

	std::string getTimeParked() const;

	friend std::ostream& operator<<(std::ostream& os, const Vehicle& v);

private:
	int id;
	std::string path;
	std::string licensePlate;
	std::string time;
	std::string timeParked;
};
