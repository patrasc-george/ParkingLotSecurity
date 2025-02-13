#pragma once

#include <iostream>

/**
 * @class Vehicle
 * @brief Represents a vehicle in the parking management system.
 *
 * This class encapsulates the details of a vehicle, including its unique ID, path, license plate number,
 * entry time, ticket information, parking duration, total amount due for parking, and payment status.
 * It provides methods for getting and setting these attributes, as well as updating the vehicle's payment status.
 * The class also overloads the stream insertion operator to enable printing of vehicle details.
 */
class Vehicle
{
public:
	/**
	 * @brief Default constructor for the Vehicle class.
	 * @details Initializes the vehicle with default values.
	 */
	Vehicle() = default;

	/**
	 * @brief Constructs a Vehicle object with specific attributes.
	 * @param[in] id The unique identifier for the vehicle.
	 * @param[in] path The path associated with the vehicle.
	 * @param[in] licensePlate The license plate number of the vehicle.
	 * @param[in] dateTime The date and time when the vehicle entered the parking lot.
	 */
	Vehicle(const int& id, const std::string& path, const std::string& licensePlate, const std::string& dateTime)
		: id(id), path(path), licensePlate(licensePlate), dateTime(dateTime), timeParked(""), totalAmount(0), isPaid(false) {}

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
	 * @brief Retrieves the license plate of the vehicle.
	 * @details The license plate number is crucial for vehicle identification and is often used in logging, tracking, and regulatory compliance.
	 * @return A string representing the license plate.
	 */
	std::string getLicensePlate() const;

	/**
	 * @brief Sets the license plate of the vehicle.
	 * @details This function allows you to update the license plate of the vehicle.
	 * @param[in] licensePlate The new license plate number.
	 */
	void setLicensePlate(const std::string& licensePlate);

	/**
	 * @brief Retrieves the date and time associated with the vehicle.
	 * @details This represents the time the vehicle entered the parking lot or any other significant timestamp.
	 * @return A string representing the date and time of entry.
	 */
	std::string getDateTime() const;

	/**
	 * @brief Retrieves the ticket number associated with the vehicle.
	 * @details The ticket number identifies the vehicle's parking session or any related transactions.
	 * @return A string representing the ticket number.
	 */
	std::string getTicket() const;

	/**
	 * @brief Sets the ticket number for the vehicle.
	 * @details This function allows you to assign a new ticket number to the vehicle.
	 * @param[in] ticket The new ticket number.
	 */
	void setTicket(const std::string& ticket);

	/**
	 * @brief Retrieves the time the vehicle has been parked.
	 * @details This is the duration the vehicle has been parked, useful for billing and tracking.
	 * @return A string representing the parked time.
	 */
	std::string getTimeParked() const;

	/**
	 * @brief Sets the time the vehicle has been parked.
	 * @details This function allows you to record how long the vehicle has been parked.
	 * @param[in] timeParked The time the vehicle has been parked.
	 */
	void setTimeParked(const std::string& timeParked);

	/**
	 * @brief Retrieves the total amount to be paid for parking.
	 * @details This represents the total fee for parking based on time or any other billing factor.
	 * @return The total amount as an integer value.
	 */
	int getTotalAmount() const;

	/**
	 * @brief Sets the total amount to be paid for parking.
	 * @details This function allows you to set the parking fee.
	 * @param[in] totalAmount The total amount for parking in monetary units (e.g., RON).
	 */
	void setTotalAmount(const int& totalAmount);

	/**
	 * @brief Retrieves whether the parking fee has been paid.
	 * @details Returns a boolean value indicating whether the vehicle has paid for parking.
	 * @return `true` if the parking fee has been paid, `false` otherwise.
	 */
	bool getIsPaid();

	/**
	 * @brief Marks the vehicle as having paid for parking.
	 * @details This function updates the `isPaid` flag to `true`, indicating that the parking fee has been settled.
	 */
	void setIsPaid();

	friend std::ostream& operator<<(std::ostream& os, const Vehicle& vehicle);

private:
	int id;
	std::string path;
	std::string licensePlate;
	std::string dateTime;
	std::string ticket;
	std::string timeParked;
	int totalAmount;
	bool isPaid;
};

/**
 * @brief Overloads the insertion operator for the Vehicle class to print vehicle details.
 * @details Allows direct output of Vehicle object properties to the standard output or other output streams.
 * @param[out] os The output stream object.
 * @param[in] vehicle The vehicle object to be output.
 * @return A reference to the output stream.
 */
std::ostream& operator<<(std::ostream& os, const Vehicle& vehicle);
