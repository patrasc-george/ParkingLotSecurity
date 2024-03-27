#pragma once

#include <iostream>

class Vehicle
{
public:
	Vehicle() = default;

	Vehicle(const int& id, const std::string& path, const int& ticket, const std::string& licensePlate, const std::string& time)
		: id(id), path(path), ticket(ticket), licensePlate(licensePlate), time(time) {}

public:
	/**
	 * @brief Retrieves the vehicle's ID.
	 * @details The ID is a unique identifier for each vehicle object, typically used to track vehicles within a system.
	 * @return The ID of the vehicle as an integer.
	 */
	int getId() const;

	/**
	 * @brief Retrieves the path associated with the vehicle.
	 * @details The path could represent a file path or URL where information about the vehicle is stored or logged.
	 * @return A string representing the path.
	 */
	std::string getPath() const;

	/**
	 * @brief Retrieves the ticket number of the vehicle.
	 * @details The ticket number is used to identify the vehicle's parking session or other related transactions.
	 * @return The ticket number as an integer.
	 */
	int getTicket() const;

	/**
	 * @brief Retrieves the license plate of the vehicle.
	 * @details The license plate number is crucial for vehicle identification and is often used in logging, tracking, and regulatory compliance.
	 * @return A string representing the license plate.
	 */
	std::string getLicensePlate() const;

	/**
	 * @brief Retrieves the time associated with the vehicle.
	 * @details This could represent the time the vehicle was parked, entered a certain location, or any other significant timestamp.
	 * @return A string representing the time.
	 */
	std::string getTime() const;

	/**
	 * @brief Sets the parked time for the vehicle.
	 * @details Allows for recording the duration that the vehicle has been parked, aiding in time-based calculations or billing.
	 * @param[in] stayedTime The time the vehicle stayed parked.
	 */
	void setTimeParked(const std::string& stayedTime);

	/**
	 * @brief Retrieves the parked time of the vehicle.
	 * @details Useful for audits, billing, or simply tracking how long a vehicle has been parked.
	 * @return A string representing the parked time.
	 */
	std::string getTimeParked() const;

	friend std::ostream& operator<<(std::ostream& os, const Vehicle& vehicle);

private:
	int id;
	std::string path;
	int ticket;
	std::string licensePlate;
	std::string time;
	std::string timeParked;
};

/**
 * @brief Overloads the insertion operator for the Vehicle class to print vehicle details.
 * @details Allows direct output of Vehicle object properties to the standard output or other output streams.
 * @param[out] os The output stream object.
 * @param[in] vehicle The vehicle object to be output.
 * @return A reference to the output stream.
 */
std::ostream& operator<<(std::ostream& os, const Vehicle& vehicle);

