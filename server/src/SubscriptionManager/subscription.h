#pragma once

#ifdef _WIN32
#ifdef SUBSCRIPTION_EXPORTS
#define SUBSCRIPTION_API __declspec(dllexport)
#else
#define SUBSCRIPTION_API __declspec(dllimport)
#endif
#elif __linux__
#define SUBSCRIPTION_API __attribute__((visibility("default")))
#else
#define SUBSCRIPTION_API
#endif

#include <string>
#include <vector>
#include <algorithm>

/**
 * @class Subscription
 * @brief Represents a subscription with associated vehicles and date-time entries.
 *
 * The `Subscription` class is designed to store information about a subscription, such as its name, associated vehicles (e.g., license plates),
 * and date-time entries (e.g., subscription dates, renewals, or activity logs). It provides methods to add or remove vehicles and date-time entries.
 */
class SUBSCRIPTION_API Subscription
{
public:
	/**
	 * @brief Constructs a Subscription with a specified name.
	 * @details Initializes a subscription object with the provided name. The name could represent the type, category, or owner's name for the subscription.
	 * @param[in] name The name to associate with this subscription.
	 */
	Subscription(const std::string name) : name(name) {}

	/**
	 * @brief Default constructor for Subscription.
	 * @details Initializes a subscription with default values. No name or data is set initially. This constructor is useful when creating a subscription without providing any details at the time of creation.
	 */
	Subscription() = default;

public:
	/**
	 * @brief Gets the subscription name.
	 * @details Returns the name associated with this subscription. This name could represent the user's subscription type or category.
	 * @return The subscription name as a string.
	 */
	std::string getName() const;

	/**
	 * @brief Gets the list of date-time entries.
	 * @details Returns all date-time strings associated with the subscription. These might represent subscription start dates, renewals, or activity logs.
	 * @return A vector containing all date-time strings associated with the subscription.
	 */
	std::vector<std::string> getDateTimes() const;

	/**
	 * @brief Adds a new date-time to the subscription.
	 * @details Appends a date-time string to the list of subscription date-times. This could be used for logging events or recording renewals.
	 * @param[in] dateTime The date-time string to be added to the subscription.
	 * @return void
	 */
	void addDateTime(const std::string& dateTime);

	/**
	 * @brief Gets the list of vehicles associated with the subscription.
	 * @details Returns all vehicles linked to the subscription. This might represent vehicles that are registered under a parking or toll service subscription.
	 * @return A vector containing all vehicle identifiers (e.g., license plates).
	 */
	std::vector<std::string> getVehicles() const;

	/**
	 * @brief Adds a vehicle to the subscription.
	 * @details Adds a new vehicle identifier to the list of vehicles associated with the subscription. If the vehicle is already registered, it is not added again.
	 * @param[in] vehicle The vehicle identifier (e.g., license plate) to add.
	 * @return `true` if the vehicle was added successfully, `false` if the vehicle already existed.
	 */
	bool addVehicle(const std::string& vehicle);

	/**
	 * @brief Deletes a vehicle from the subscription.
	 * @details Removes a vehicle from the subscription if it exists. If the vehicle is not found, the operation fails.
	 * @param[in] vehicle The vehicle identifier to remove.
	 * @return `true` if the vehicle was removed successfully, `false` if the vehicle was not found.
	 */
	bool deleteVehicle(const std::string& vehicle);

private:
	std::string name;
	std::vector<std::string> dateTimes;
	std::vector<std::string> vehicles;
};
