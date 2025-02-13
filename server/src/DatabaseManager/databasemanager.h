#pragma once

#ifdef _WIN32
#ifdef DATABASEMANAGER_EXPORTS
#define DATABASEMANAGER_API __declspec(dllexport)
#else
#define DATABASEMANAGER_API __declspec(dllimport)
#endif
#elif __linux__
#define DATABASEMANAGER_API __attribute__((visibility("default")))
#else
#define DATABASEMANAGER_API
#endif

#include "logger.h"

#include <libpq-fe.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

/**
 * @class DatabaseManager
 * @brief Manages database connections and operations related to vehicles, accounts, payments, subscriptions, and newsletters.
 *
 * The `DatabaseManager` class provides a set of methods to interact with a PostgreSQL database, allowing for the management
 * of vehicles, accounts, subscriptions, and other related records. It uses the Singleton design pattern to ensure that only
 * one instance of the class exists. The class offers functionality for initializing the database, retrieving and modifying
 * records related to vehicles, accounts, and subscriptions, as well as managing newsletter subscriptions.
 * It also incorporates a `Logger` instance for logging various operations and errors that occur during database interactions.
 * The methods cover a wide range of tasks, from querying and updating vehicle data to adding and removing subscriptions and
 * handling user account information.
 *
 * The class ensures data consistency, handles errors gracefully with logging, and provides mechanisms for interacting with
 * the database efficiently, including adding, updating, and deleting various records across multiple tables.
 */
class DATABASEMANAGER_API DatabaseManager
{
private:
	/**
	 * @brief Constructor for the DatabaseManager class.
	 * @details This constructor initializes the `DatabaseManager` object by getting an instance of the `Logger` class.
	 *          It uses the `Logger::getInstance()` method to retrieve the singleton instance of the logger, which is then assigned to the `logger` member variable.
	 *          This ensures that the `DatabaseManager` has access to logging functionality for error and info messages throughout its lifecycle.
	 */
	DatabaseManager() : logger(Logger::getInstance()) {};

	/**
	 * @brief Destructor for the DatabaseManager class.
	 * @details This destructor is responsible for closing the database connection when the `DatabaseManager` object is destroyed.
	 *          It also logs the closing of the database connection and the program shutdown.
	 */
	~DatabaseManager();

public:
	/**
	 * @brief Returns the singleton instance of the DatabaseManager class.
	 * @details This function ensures that only one instance of the DatabaseManager class exists throughout the application.
	 *          It utilizes a static instance to guarantee a single, reusable object, providing access to database-related operations.
	 * @return A reference to the single DatabaseManager instance.
	 */
	static DatabaseManager& getInstance();

	/**
	 * @brief Initializes the connection to the database and creates necessary tables if they do not exist.
	 * @details This function establishes a connection to the database using different URLs depending on the build configuration (debug or release).
	 *          It then attempts to create several tables for vehicles, accounts, payments, subscription data, and newsletters.
	 *          If any of these operations fail, an error message is logged, and the function returns false. If successful, it logs that the database
	 *          was initialized properly and returns true.
	 * @return Returns true if the database was initialized successfully, otherwise false.
	 */
	bool initializeDatabase();

	/**
	 * @brief Retrieves a list of all vehicles stored in the database.
	 * @details This function queries the database for all vehicles and fetches their associated information (ID, image path, license plate, etc.).
	 *          The results are then parsed, formatted into a string, and stored in a vector.
	 *          If the query fails, an error message is logged, and an empty vector is returned.
	 * @return A vector containing strings with details about each vehicle.
	 */
	std::vector<std::string> getVehicles();

	/**
	 * @brief Retrieves the last recorded activity for a specific vehicle based on its license plate.
	 * @details This function queries the database to fetch the most recent activity (ticket, date/time, time parked, and total amount) of a vehicle.
	 *          If no data is found or if the vehicle's activity doesn't exist, it returns a default activity string.
	 *          In case of an error while fetching the data, a log message is generated, and a default string is returned.
	 * @param[in] vehicleLicensePlate The license plate number of the vehicle whose activity is to be fetched.
	 * @return A string representing the last vehicle activity (ticket, date/time, etc.), or a default string if no activity is found.
	 */
	std::string getLastVehicleActivity(const std::string& vehicleLicensePlate);

	/**
	 * @brief Retrieves the total parking time for a specific vehicle based on its license plate.
	 * @details This function queries the database to fetch the parking time for a vehicle identified by its license plate.
	 *          The time is accumulated by summing the individual parked durations (hours, minutes, seconds) across multiple records for the vehicle.
	 *          If no parking time data is found or if an error occurs while fetching the data, it returns a default time string of "00:00:00".
	 * @param[in] vehicleLicensePlate The license plate number of the vehicle whose total parking time is to be retrieved.
	 * @return A string representing the total parking time in the format "HH:MM:SS".
	 */
	std::string getTotalTimeParked(const std::string& vehicleLicensePlate);

	/**
	 * @brief Retrieves the total payment for a specific vehicle based on its license plate.
	 * @details This function queries the database to fetch the total amount paid for parking, calculated based on the records for the specified vehicle.
	 *          It sums up the total amount for all parking sessions associated with the vehicle.
	 *          If no payment data is found or if an error occurs, it returns a default payment amount of 0.
	 * @param[in] vehicleLicensePlate The license plate number of the vehicle whose payment total is to be retrieved.
	 * @return The total amount paid for parking, as an integer value.
	 */
	int getPayment(const std::string& vehicleLicensePlate);

	/**
	 * @brief Retrieves a list of all accounts stored in the database.
	 * @details This function queries the database for all accounts and fetches the associated account details such as name, last name, email, password, and phone number.
	 *          The results are then parsed, formatted into a string, and stored in a vector.
	 *          If the query fails, an error message is logged, and an empty vector is returned.
	 * @return A vector containing strings with details about each account (name, last name, email, password, and phone).
	 */
	std::vector<std::string> getAccounts();

	/**
	 * @brief Retrieves the list of all email addresses in the newsletter.
	 * @details This function queries the database for all email addresses subscribed to the newsletter.
	 *          The emails are stored in an unordered set to ensure uniqueness and then returned to the caller.
	 *          If the query fails, an error message is logged, and an empty set is returned.
	 * @return An unordered set containing the emails subscribed to the newsletter.
	 */
	std::unordered_set<std::string> getNewsletter();

	/**
	 * @brief Retrieves the subscription details for a specific user based on their email.
	 * @details This function first queries the database to fetch all distinct subscription names associated with the provided email.
	 *          Then, it fetches the payment dates and the license plates associated with each subscription for that user.
	 *          The data is stored in an unordered map where the key is the subscription name, and the value is a pair containing the payment dates and license plates.
	 *          If an error occurs during the query or no data is found, it returns an empty map.
	 * @param[in] email The email of the user whose subscriptions are to be retrieved.
	 * @return An unordered map where each key is a subscription name and each value is a pair of strings:
	 *         the first string is the list of payment dates, and the second string is the list of associated license plates.
	 */
	std::unordered_map<std::string, std::pair<std::string, std::string>> DatabaseManager::getSubscriptions(const std::string& email);

	/**
	 * @brief Retrieves the parking history of a specific vehicle based on its license plate.
	 * @details This function queries the database for the parking history of the vehicle identified by its license plate.
	 *          The function returns a vector containing strings with the parking session details (ticket, date, time parked, and total amount) in reverse chronological order.
	 *          If an error occurs during the query, it logs the error and returns an empty vector.
	 * @param[in] vehicleLicensePlate The license plate number of the vehicle whose history is to be retrieved.
	 * @return A vector of strings, each representing a parking session, including the ticket, date/time, time parked, and total amount.
	 */
	std::vector<std::string> DatabaseManager::getVehicleHistory(const std::string& vehicleLicensePlate);

	/**
	 * @brief Checks if a specific vehicle's parking fee has been paid based on its license plate.
	 * @details This function first verifies if the license plate exists in the database. Then, it queries the database for the latest record associated with the vehicle's license plate to check the payment status.
	 *          It returns true if the parking fee has been paid ("is_paid" is true), and false otherwise. If the vehicle's license plate is not found or an error occurs, it returns false.
	 * @param[in] vehicleLicensePlate The license plate number of the vehicle whose payment status is to be checked.
	 * @return A boolean value: true if the parking fee has been paid, false otherwise.
	 */
	bool DatabaseManager::getIsPaid(const std::string& vehicleLicensePlate);

	/**
	 * @brief Sets the payment status for a vehicle based on its license plate or ticket.
	 * @details This function first checks whether the vehicle has a ticket or is identified by its license plate,
	 *          then retrieves the latest entry in the database that matches either the ticket or license plate.
	 *          After confirming the vehicle's information, it updates the "is_paid" field for the corresponding record
	 *          to mark it as paid. The license plate and date/time of the vehicle are returned through reference parameters.
	 *          If any error occurs during the query or update, the function returns false; otherwise, it returns true.
	 * @param[in] vehicle The vehicle's ticket or license plate number.
	 * @param[out] licensePlate The license plate of the vehicle is updated.
	 * @param[out] dateTime The date and time when the vehicle's data was fetched is updated.
	 * @param[in] isTicket Boolean indicating whether the query should search by ticket (true) or by license plate (false).
	 * @return A boolean indicating whether the payment status was successfully updated.
	 */
	bool DatabaseManager::setIsPaid(const std::string& vehicle, std::string& licensePlate, std::string& dateTime, const bool& isTicket);

	/**
	 * @brief Updates the name of an account based on the user's email.
	 * @details This function checks if the new name is not empty and updates the "name" field in the "accounts" table for the given email.
	 *          If an error occurs during the update, it logs the error message.
	 * @param[in] email The email address of the account to update.
	 * @param[in] newName The new name to set for the account.
	 */
	void DatabaseManager::setName(const std::string& email, const std::string& newName);

	/**
	 * @brief Updates the last name of an account based on the user's email.
	 * @details This function checks if the new last name is not empty and updates the "last_name" field in the "accounts" table for the given email.
	 *          If an error occurs during the update, it logs the error message.
	 * @param[in] email The email address of the account to update.
	 * @param[in] newLastName The new last name to set for the account.
	 */
	void DatabaseManager::setLastName(const std::string& email, const std::string& newLastName);

	/**
	 * @brief Updates the email address of an account and associated records in related tables.
	 * @details This function first checks if the old email exists in the database. Then, it creates a new account with the new email
	 *          and transfers all the information (name, last name, password, phone) from the old account to the new one.
	 *          It also updates the user's subscription payments and license plates to reflect the new email.
	 *          Afterward, it deletes the old account from the database. If any errors occur during any of the database operations,
	 *          the function logs the errors.
	 * @param[in] email The old email address of the account to be updated.
	 * @param[in] newEmail The new email address to assign to the account.
	 */
	void DatabaseManager::setEmail(const std::string& email, const std::string& newEmail);

	/**
	 * @brief Updates the password of an account based on the user's email.
	 * @details This function checks if the new password is not empty and then updates the "password" field in the "accounts" table
	 *          for the provided email address. If an error occurs during the update, the function logs the error.
	 * @param[in] email The email address of the account whose password needs to be updated.
	 * @param[in] newPassword The new password to set for the account.
	 */
	void DatabaseManager::setPassword(const std::string& email, const std::string& newPassword);

	/**
	 * @brief Updates the phone number of an account based on the user's email.
	 * @details This function checks if the new phone number is not empty and then updates the "phone" field in the "accounts" table
	 *          for the provided email address. If an error occurs during the update, the function logs the error.
	 * @param[in] email The email address of the account whose phone number needs to be updated.
	 * @param[in] newPhone The new phone number to set for the account.
	 */
	void DatabaseManager::setPhone(const std::string& email, const std::string& newPhone);

	/**
	 * @brief Adds a new vehicle to the "vehicles" table.
	 * @details This function inserts a new vehicle into the database with its associated details such as image path, license plate,
	 *          date/time of entry, ticket number, parking time, total amount, and payment status. If any error occurs during the
	 *          insertion, the function logs the error and throws a runtime exception.
	 * @param[in] id The unique identifier for the vehicle (though not used in the SQL query).
	 * @param[in] imagePath The path to the image representing the vehicle.
	 * @param[in] licensePlate The license plate of the vehicle.
	 * @param[in] dateTime The date and time when the vehicle's entry was recorded.
	 * @param[in] ticket The ticket number associated with the vehicle.
	 * @param[in] timeParked The amount of time the vehicle has been parked.
	 * @param[in] totalAmount The total amount due for parking.
	 * @param[in] isPaid A flag indicating whether the vehicle's parking fee has been paid.
	 * @throws std::runtime_error If an error occurs while inserting the vehicle into the database.
	 */
	void DatabaseManager::addVehicle(const int& id, const std::string& imagePath, const std::string& licensePlate, const std::string& dateTime, const std::string& ticket, const std::string& timeParked, const std::string& totalAmount, const std::string& isPaid);

	/**
	 * @brief Adds a new account to the "accounts" table.
	 * @details This function inserts a new account with the given details such as name, last name, email, password, and phone number.
	 *          If any error occurs during the insertion, the function logs the error.
	 * @param[in] name The name of the account holder.
	 * @param[in] lastName The last name of the account holder.
	 * @param[in] email The email address for the account.
	 * @param[in] password The password for the account.
	 * @param[in] phone The phone number of the account holder.
	 */
	void DatabaseManager::addAccount(const std::string& name, const std::string& lastName, const std::string& email, const std::string& password, const std::string& phone);

	/**
	 * @brief Adds a new subscription and links it to a payment record.
	 * @details This function creates a new subscription payment entry in the "payments" table, retrieves the payment ID, and
	 *          inserts a record into the "subscription_payments" table to link the subscription with the payment. The function
	 *          logs any errors that occur during the process.
	 * @param[in] email The email address of the account to link to the subscription.
	 * @param[in] name The name of the subscription to link to the payment.
	 */
	void DatabaseManager::addSubscription(const std::string& email, const std::string& name);

	/**
	 * @brief Adds a license plate to the database and links it to the given subscription.
	 * @details This function inserts the provided license plate number into the "license_plates" table. After the insertion, it retrieves
	 *          the last inserted ID and links the license plate to the subscription associated with the provided email and subscription name.
	 *          If an error occurs during any of the steps, the function logs the error.
	 * @param[in] email The email address of the user whose subscription the license plate will be linked to.
	 * @param[in] name The name of the subscription that the license plate will be associated with.
	 * @param[in] licensePlate The license plate number to insert and link to the subscription.
	 */
	void DatabaseManager::addLicensePlate(const std::string& email, const std::string& name, const std::string& licensePlate);

	/**
	 * @brief Subscribes the given email address to the newsletter.
	 * @details This function inserts the provided email address into the "newsletter" table to subscribe the user to the newsletter.
	 *          If an error occurs during the insertion, the function logs the error.
	 * @param[in] email The email address to be subscribed to the newsletter.
	 */
	void DatabaseManager::subscribeNewsletter(const std::string& email);

	/**
	 * @brief Unsubscribes the given email address from the newsletter.
	 * @details This function removes the provided email address from the "newsletter" table, effectively unsubscribing the user from the newsletter.
	 *          If an error occurs during the deletion, the function logs the error.
	 * @param[in] email The email address to be unsubscribed from the newsletter.
	 */
	void DatabaseManager::unsubscribeNewsletter(const std::string& email);

	/**
	 * @brief Deletes a subscription and related records for the specified email and subscription name.
	 * @details This function performs multiple operations to delete a subscription and its related records from the database:
	 *          - Retrieves the payment IDs and license plate IDs linked to the subscription.
	 *          - Deletes records from the "subscription_payments" and "subscription_license_plates" tables.
	 *          - Deletes the corresponding entries from the "payments" and "license_plates" tables.
	 *          If any error occurs during any of the operations, the function logs the error and halts the process.
	 * @param[in] email The email address of the user whose subscription and related records are to be deleted.
	 * @param[in] name The name of the subscription to delete.
	 */
	void DatabaseManager::deleteSubscription(const std::string& email, const std::string& name);

	/**
	 * @brief Deletes a license plate from the database and removes the link between the license plate and its subscription.
	 * @details This function first retrieves the ID of the given license plate from the `license_plates` table.
	 *          If the license plate is found, it proceeds to:
	 *          - Delete the link between the license plate and the subscription in the `subscription_license_plates` table.
	 *          - Delete the license plate record from the `license_plates` table.
	 *          If any of the steps fail, the function logs an error message.
	 * @param[in] email The email address of the user whose subscription the license plate is linked to.
	 * @param[in] name The name of the subscription the license plate is associated with.
	 * @param[in] licensePlate The license plate number to be deleted.
	 */
	void deleteLicensePlate(const std::string& email, const std::string& name, const std::string& licensePlate);

private:
	PGconn* conn;
	Logger& logger;
};
