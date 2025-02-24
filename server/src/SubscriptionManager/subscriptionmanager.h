#pragma once

#ifdef _WIN32
#ifdef SUBSCRIPTIONMANAGER_EXPORTS
#define SUBSCRIPTIONMANAGER_API __declspec(dllexport)
#else
#define SUBSCRIPTIONMANAGER_API __declspec(dllimport)
#endif
#elif __linux__
#define SUBSCRIPTIONMANAGER_API __attribute__((visibility("default")))
#else
#define SUBSCRIPTIONMANAGER_API
#endif

#include "account.h"
#include "subscription.h"
#include "databasemanager.h"

#include <fstream>
#include <thread>
#include <map>
#include <unordered_set>

/**
 * @class SubscriptionManager
 * @brief Manages subscriptions, accounts, vehicles, and related operations for the system.
 *
 * The `SubscriptionManager` class provides functionality for managing subscriptions, accounts, and vehicles within the system.
 * It allows for tasks such as adding and removing subscriptions, verifying account credentials, handling vehicle subscriptions,
 * processing payments, and more. The class also handles temporary account-related data such as recovery tokens, account updates,
 * and temporary account creation. It is designed to interact with a database for data persistence and updates, and it uses background
 * threads to periodically perform maintenance tasks such as removing expired tokens.
 *
 * The class includes methods for retrieving account details, adding subscriptions, managing vehicles, and handling user and admin
 * operations. It also allows for newsletter subscriptions, password recovery, and account updates, ensuring that the internal data
 * remains consistent and synchronized with the database.
 */
class SUBSCRIPTIONMANAGER_API SubscriptionManager
{
public:
	/**
	 * @brief Constructs a SubscriptionManager and initializes the database and subscriptions.
	 * @details This constructor initializes the `SubscriptionManager` by setting up the `DatabaseManager` instance and calling the `initializeDatabase` function.
	 *          It then uploads the subscription data from the database into the internal data structures.
	 *          A background thread is launched that periodically removes expired tokens from various lists (temporary accounts, recovered passwords, and updated accounts) every 5 minutes.
	 *          The thread uses the `removeExpiredToken` function to handle the removal process.
	 */
	SubscriptionManager();

	/**
	 * @brief Destructor for the `SubscriptionManager` class.
	 * @details This destructor ensures that if a background thread is running, it is properly joined before the object is destroyed. It helps in preventing any
	 *          undefined behavior that might arise from an unjoined thread when the `SubscriptionManager` object goes out of scope.
	 */
	~SubscriptionManager();

public:

	/**
	 * @brief Uploads subscriptions from the database to internal memory.
	 * @details This function retrieves all account data and associated subscription data from the database and stores it into the internal `accounts` container.
	 *          Each account is created using the data retrieved, and corresponding subscription information (including dateTimes and vehicles) is added to each account's subscription list.
	 *          This process ensures the internal subscription data is always up-to-date with the database.
	 * @return void
	 */
	void uploadSubscriptions();

	/**
	 * @brief Checks if a given vehicle has an active subscription.
	 * @details This function checks whether a vehicle, identified by its license plate, has an active subscription by iterating over all accounts and their associated subscriptions.
	 * @param[in] licensePlate The license plate of the vehicle to check.
	 * @return Returns true if the vehicle is found in any active subscription, otherwise false.
	 */
	bool checkSubscription(const std::string& licensePlate);

	/**
	 * @brief Verifies the credentials of an account based on input and password.
	 * @details This function attempts to verify the provided credentials by matching either the email or phone number along with the password against the stored account data.
	 * @param[in] input The input (email or phone) to verify.
	 * @param[in] password The password associated with the account.
	 * @return Returns a pointer to the matched account if the credentials are valid, otherwise nullptr.
	 */
	Account* verifyCredentials(const std::string& input, const std::string& password);

	/**
	 * @brief Verifies the admin credentials.
	 * @details This function verifies if the provided password matches the stored admin credentials. The password is retrieved from environment variables depending on the build configuration (debug or release).
	 * @param[in] password The password to verify.
	 * @return Returns true if the password matches the stored admin credentials, otherwise false.
	 */
	bool verifyAdminCredentials(const std::string& password) const;

	/**
	 * @brief Marks a vehicle as paid for a specific parking session.
	 * @details This function marks a parking session as paid in the database for the specified vehicle, license plate, and date/time.
	 * @param[in] vehicle The vehicle associated with the parking session.
	 * @param[in,out] licensePlate The license plate of the vehicle.
	 * @param[in,out] dateTime The date and time of the parking session.
	 * @param[in] isTicket A boolean indicating whether the payment is related to a ticket.
	 * @return Returns true if the payment was successfully processed, otherwise false.
	 */
	bool pay(const std::string& vehicle, std::string& licensePlate, std::string& dateTime, const bool& isTicket = false);

public:
	/**
	 * @brief Retrieves the entire map of accounts with their associated subscriptions.
	 * @details This function returns a `std::map` that contains all accounts stored in the `SubscriptionManager` along with their respective subscriptions.
	 *          Each entry in the map associates an `Account` object with a list of `Subscription` objects. This allows the `SubscriptionManager` to manage multiple
	 *          subscriptions for each account and provides an easy way to access account-subscription relationships.
	 * @return Returns a map of `Account` objects as keys and corresponding `Subscription` objects as values.
	 */
	std::map<Account, std::vector<Subscription>> getAccounts() const;

	/**
	 * @brief Retrieves all unique email addresses from the stored accounts.
	 * @details This function iterates through the stored accounts in the `SubscriptionManager` and extracts the email addresses, collecting them
	 *          in a `std::unordered_set` to ensure that only unique email addresses are stored. This is useful for quickly checking whether
	 *          an email address already exists in the system and for generating reports or performing other email-related operations.
	 * @return Returns a set of email addresses from all stored accounts.
	 */
	std::unordered_set<std::string> getEmails() const;

	/**
	 * @brief Retrieves a temporary account associated with a specific token.
	 * @details This function searches the `tempAccounts` map for an entry matching the provided token. If a matching token is found,
	 *          it returns the corresponding account data as a vector of strings. This is typically used to retrieve temporary account information
	 *          before the account is fully registered or processed. If no matching token is found, an empty vector is returned.
	 * @param[in] token The token associated with the temporary account.
	 * @return Returns a vector containing the temporary account data, or an empty vector if no matching token is found.
	 */
	std::vector<std::string> getTempAccount(const std::string& token) const;

	/**
	 * @brief Retrieves the token associated with a temporary account's email.
	 * @details This function checks if a temporary account exists for the provided email in the `tempAccounts` map. If found,
	 *          it retrieves the associated token from the stored data. This token is typically used for verifying or processing the
	 *          temporary account before it is fully activated or registered. If no account exists for the given email, an empty string is returned.
	 * @param[in] email The email associated with the temporary account.
	 * @return Returns the token associated with the temporary account, or an empty string if no account is found for the given email.
	 */
	std::string getTempAccountToken(const std::string& email);

	/**
	 * @brief Retrieves the token associated with a temporary recovered password for a given email.
	 * @details This function searches the `tempRecoveredPasswords` map for the specified email and retrieves the associated token.
	 *          This token is used to process or validate the password recovery request. If no entry is found for the given email,
	 *          an empty string is returned.
	 * @param[in] email The email address for which the recovery token is requested.
	 * @return Returns the token associated with the password recovery process for the given email, or an empty string if not found.
	 */
	std::string getTempRecoveredPasswordToken(const std::string& email);

	/**
	 * @brief Retrieves the token associated with a temporary updated account for a given email.
	 * @details This function searches the `tempUpdatedAccounts` map for the specified email and retrieves the associated token.
	 *          The token is used to verify or process account updates. If no entry is found for the given email, an empty string is returned.
	 * @param[in] email The email address associated with the temporary account update.
	 * @return Returns the token for the updated account, or an empty string if no entry is found.
	 */
	std::string getTempUpdatedAccountToken(const std::string& email);

	/**
	 * @brief Retrieves the temporary account information associated with a specific token.
	 * @details This function searches through the `tempUpdatedAccounts` map to find a temporary account that matches the provided token.
	 *          If a match is found, the associated account data is returned as a vector of strings. If no match is found, an empty vector is returned.
	 * @param[in] token The token used to search for the temporary updated account.
	 * @return Returns the temporary account data if found, otherwise an empty vector.
	 */
	std::vector<std::string> getTempUpdatedAccount(const std::string& token);

	/**
	 * @brief Retrieves an `Account` object by its email.
	 * @details This function searches the `accounts` map for the specified email. If an account with the given email is found,
	 *          it returns a pointer to the `Account` object. Otherwise, it returns a `nullptr`, indicating that no matching account exists.
	 * @param[in] email The email address associated with the account.
	 * @return Returns a pointer to the `Account` object if found, otherwise `nullptr`.
	 */
	Account* getAccountByEmail(const std::string& email) const;

	/**
	 * @brief Retrieves an `Account` object by its phone number.
	 * @details This function searches the `accounts` map for the specified phone number. If an account with the given phone number is found,
	 *          it returns a pointer to the `Account` object. If no account is found, it returns `nullptr`.
	 * @param[in] phone The phone number associated with the account.
	 * @return Returns a pointer to the `Account` object if found, otherwise `nullptr`.
	 */
	Account* getAccountByPhone(const std::string& phone) const;

	/**
	 * @brief Retrieves the list of subscriptions associated with a specific account.
	 * @details This function searches for the account in the `accounts` map and retrieves the associated list of subscriptions.
	 *          If the account is found, it returns a pointer to the list of `Subscription` objects associated with that account. If no account is found,
	 *          it returns `nullptr`.
	 * @param[in] account The `Account` object for which subscriptions are being retrieved.
	 * @return Returns a pointer to the list of `Subscription` objects for the specified account, or `nullptr` if the account is not found.
	 */
	std::vector<Subscription>* getSubscriptions(const Account& account) const;

	/**
	 * @brief Retrieves a specific `Subscription` object associated with an account.
	 * @details This function searches the list of subscriptions for the specified `Account` object and returns the subscription
	 *          matching the provided name. If a match is found, a pointer to the `Subscription` object is returned. If no matching subscription
	 *          is found, it returns `nullptr`.
	 * @param[in] account The `Account` object to search for the subscription.
	 * @param[in] name The name of the subscription being searched for.
	 * @return Returns a pointer to the `Subscription` object if found, otherwise `nullptr`.
	 */
	Subscription* getSubscription(const Account& account, const std::string& name);

	/**
	 * @brief Retrieves a list of subscribed vehicles for a specific subscription.
	 * @details This function generates a list of vehicles associated with the provided `Subscription` object, along with details
	 *          about each vehicle's parking activity and payment status. It uses data from the `databaseManager` to retrieve the
	 *          last known activity and total parking time for each vehicle, as well as the payment status.
	 * @param[in] subscription The `Subscription` object for which vehicle data is being retrieved.
	 * @return Returns a list of vectors, where each inner vector contains data related to a specific vehicle, such as license plate, activity status, payment status, etc.
	 */
	std::vector<std::vector<std::string>> getSubscriptionVehicles(const Subscription& subscription);

	/**
	* @brief Retrieves the history of a specific vehicle.
	* @details This function retrieves the history of a vehicle, identified by its license plate, including details such as the activity history and
	*          associated timestamps. Additionally, it retrieves the total time the vehicle has been parked and the total payment associated with the vehicle.
	*          The data is returned in a 2D vector, with each row representing an entry in the history.
	* @param[in] licensePlate The license plate of the vehicle whose history is being retrieved.
	* @param[out] totalTimeParked The total time the vehicle has been parked, to be populated by the function.
	* @param[out] payment The total payment associated with the vehicle, to be populated by the function.
	* @return Returns a 2D vector containing the vehicle's history with details such as activity type and timestamps.
	*/
	std::vector<std::vector<std::string>> getVehicleHistory(const std::string& licensePlate, std::string& totalTimeParked, int& payment);

	/**
	 * @brief Adds a temporary account to the system.
	 * @details This function adds a new temporary account to the `tempAccounts` map using the provided details (name, last name, email, password, phone).
	 *          The account data is stored as a comma-separated string, along with the current timestamp indicating when the account was created.
	 *          The email is used as the key for storing the temporary account.
	 * @param[in] name The first name of the account holder.
	 * @param[in] lastName The last name of the account holder.
	 * @param[in] email The email address of the account holder (used as the key).
	 * @param[in] password The password of the account holder.
	 * @param[in] phone The phone number of the account holder.
	 * @return void
	 */
	void addTempAccount(const std::string& name, const std::string& lastName, const std::string& email, const std::string& password, const std::string& phone);

	/**
	 * @brief Sets a token for a temporary account.
	 * @details This function associates a provided token with a temporary account, identified by the given email address.
	 *          If the email is found in the `tempAccounts` map, the token is appended to the corresponding account data.
	 *          The token is followed by a "false" value indicating that the account is not yet verified. If the email is not found,
	 *          the function returns `false`.
	 * @param[in] email The email address of the temporary account.
	 * @param[in] token The token to be set for the account.
	 * @return Returns `true` if the token is successfully set, `false` if the email is not found in the `tempAccounts` map.
	 */
	bool setToken(const std::string& email, const std::string& token);

	/**
	 * @brief Adds a new account after verifying the temporary account's token.
	 * @details This function verifies if a temporary account exists for the given token. If the token is valid, it creates a new `Account`
	 *          object with the data from the temporary account, adds it to the `accounts` map, and updates the database with the new account information.
	 *          The temporary account data is then marked as verified, and the account is fully registered. If the token is invalid, an empty string is returned.
	 * @param[in] token The token associated with the temporary account to be added.
	 * @return Returns the email of the newly created account if successful, otherwise returns an empty string.
	 */
	std::string addAccount(const std::string& token);

	/**
	 * @brief Adds a temporary recovered password entry.
	 * @details This function adds a temporary recovered password entry for the specified email and token. It first verifies that the account
	 *          exists for the given email. If the account is found, the recovery token is stored in the `tempRecoveredPasswords` map,
	 *          associated with the email address. The current time is also stored as part of the recovery information.
	 * @param[in] email The email address of the account requesting the password recovery.
	 * @param[in] token The token associated with the password recovery process.
	 * @return Returns `true` if the recovery entry is successfully added, `false` if the account does not exist.
	 */
	bool addTempRecoveredPasswords(const std::string& email, const std::string& token);

	/**
	 * @brief Verifies and retrieves the email associated with a recovery token.
	 * @details This function searches for the provided token in the `tempRecoveredPasswords` map. If a matching token is found,
	 *          the associated email address is returned. If no matching token is found, an empty string is returned, indicating an invalid token.
	 * @param[in] token The token to verify.
	 * @return Returns the email address associated with the token if found, otherwise returns an empty string.
	 */
	std::string verifyTempRecoveredPasswordsToken(const std::string& token);

	/**
	 * @brief Adds a temporary account update entry for a given email.
	 * @details This function stores a temporary update request for an account's email address, including the new email and password.
	 *          It first checks if the new email is valid and does not already exist in the system. If the new email is valid and no conflicts exist,
	 *          the temporary update is stored in the `tempUpdatedAccounts` map. If the new email already exists, the function returns `false`.
	 * @param[in] email The email address of the account to update.
	 * @param[in] newEmail The new email address for the account.
	 * @param[in] newPassword The new password for the account.
	 * @return Returns `true` if the temporary update entry is successfully added, `false` if the new email already exists.
	 */
	bool addTempUpdatedAccount(const std::string& email, const std::string& newEmail, const std::string& newPassword);

	/**
	 * @brief Sets an update token for a temporary account update.
	 * @details This function associates a provided update token with a temporary account update entry, identified by the email address.
	 *          If the email is found in the `tempUpdatedAccounts` map, the token is appended to the corresponding data, followed by a "false" value
	 *          indicating that the update is not yet confirmed. If the email is not found, the function returns `false`.
	 * @param[in] email The email address of the account update.
	 * @param[in] token The token to be set for the account update.
	 * @return Returns `true` if the token is successfully set, `false` if the email is not found in the `tempUpdatedAccounts` map.
	 */
	bool setUpdateToken(const std::string& email, const std::string& token);

	/**
	 * @brief Adds a new subscription for a specific account.
	 * @details This function checks if the given account already has the specified subscription. If the account does not already have the subscription,
	 *          a new `Subscription` object is created and added to the account's list of subscriptions. The new subscription is also saved to the database.
	 *          If the subscription already exists for the account, the function returns `false`.
	 * @param[in] account The account to which the subscription is being added.
	 * @param[in] name The name of the subscription to be added.
	 * @return Returns `true` if the subscription is successfully added, `false` if the subscription already exists for the account.
	 */
	bool addSubscription(const Account& account, const std::string& name);

	/**
	 * @brief Adds a vehicle to a subscription for a specific account.
	 * @details This function attempts to add a new vehicle (identified by its license plate) to a given subscription for the specified account.
	 *          If the vehicle is successfully added to the subscription, the license plate is also saved to the database.
	 *          If the vehicle already exists in the subscription, the function returns `false`.
	 * @param[in] account The account whose subscription is being updated.
	 * @param[in] subscription The subscription to which the vehicle is being added.
	 * @param[in] licensePlate The license plate of the vehicle to be added.
	 * @return Returns `true` if the vehicle is successfully added, `false` if the vehicle already exists in the subscription.
	 */
	bool addVehicle(const Account& account, Subscription& subscription, const std::string& licensePlate);

	/**
	 * @brief Subscribes an email to the newsletter.
	 * @details This function adds the provided email to the `newsletter` set if it is not already subscribed. The email is also stored in the database.
	 *          If the email is already subscribed, the function returns `false`. Otherwise, it returns `true` to indicate successful subscription.
	 * @param[in] email The email address to be subscribed to the newsletter.
	 * @return Returns `true` if the email is successfully subscribed, `false` if the email is already subscribed.
	 */
	bool subscribeNewsletter(const std::string& email);

	/**
	 * @brief Unsubscribes an email from the newsletter.
	 * @details This function removes the provided email from the `newsletter` set and updates the database to reflect the unsubscription.
	 *          If the email is not found in the subscription list, the function returns `false`. Otherwise, it returns `true`.
	 * @param[in] email The email address to be unsubscribed from the newsletter.
	 * @return Returns `true` if the email is successfully unsubscribed, `false` if the email was not subscribed.
	 */
	bool unsubscribeNewsletter(const std::string& email);

	/**
	 * @brief Updates the password for an account after verifying the recovery token.
	 * @details This function allows an account's password to be updated using a temporary password recovery token. If the token is valid, the account's password
	 *          is updated to the provided new password. The recovery entry for the email is then removed from `tempRecoveredPasswords` map, and the new password
	 *          is saved in the database.
	 * @param[in] email The email address of the account to update.
	 * @param[in] newPassword The new password to be set for the account.
	 * @return Returns `true` if the password is successfully updated, `false` if the token is not valid.
	 */
	bool updateAccountPassword(const std::string& email, const std::string& newPassword);

	/**
	 * @brief Updates the personal information for an account.
	 * @details This function updates the name, last name, and phone number of an account, if the account exists. The new information is saved to both
	 *          the internal data structure and the database. If the account is not found, the function returns `false`.
	 * @param[in] email The email address of the account to update.
	 * @param[in] name The new name to set for the account.
	 * @param[in] lastName The new last name to set for the account.
	 * @param[in] phone The new phone number to set for the account.
	 * @return Returns `true` if the account information is successfully updated, `false` if the account does not exist.
	 */
	bool updateAccountInformation(const std::string& email, const std::string& name, const std::string& lastName, const std::string& phone);

	/**
	 * @brief Updates an account's information, such as email and password.
	 * @details This function updates the account information based on the provided token. If the account has already been verified (indicated by "true"),
	 *          the email and password are updated, and the changes are persisted to the database. If the update has already been marked as "true", the entry
	 *          is erased from the temporary update list and the updated email is returned.
	 * @param[in] token The token associated with the temporary account update request.
	 * @return Returns the email of the updated account if successful, otherwise returns an empty string if the token is invalid or the update is not needed.
	 */
	std::string updateAccount(const std::string& token);

	/**
	 * @brief Deletes a subscription from a specific account.
	 * @details This function attempts to remove a subscription from an account. It searches for the subscription by name within the account's subscriptions.
	 *          If the subscription is found, it is removed from the account's list of subscriptions and deleted from the database.
	 * @param[in] account The account from which the subscription is being deleted.
	 * @param[in] name The name of the subscription to be deleted.
	 * @return Returns `true` if the subscription is successfully deleted, `false` if the subscription is not found.
	 */
	bool deleteSubscription(const Account& account, const std::string& name);

	/**
	 * @brief Deletes a vehicle from a specific subscription of an account.
	 * @details This function removes a vehicle (identified by its license plate) from a subscription. If the vehicle is successfully deleted from the subscription,
	 *          the corresponding license plate is also deleted from the database.
	 * @param[in] account The account whose subscription the vehicle belongs to.
	 * @param[in] subscription The subscription from which the vehicle is being removed.
	 * @param[in] licensePlate The license plate of the vehicle to be deleted.
	 * @return Returns `true` if the vehicle is successfully deleted, `false` if the vehicle does not exist in the subscription.
	 */
	bool deleteVehicle(const Account& account, Subscription& subscription, const std::string& licensePlate);

private:
	DatabaseManager& databaseManager;
	std::thread thread;
	std::unordered_set<std::string> newsletter;
	std::map<Account, std::vector<Subscription>> accounts;
	std::unordered_map<std::string, std::string> tempAccounts;
	std::unordered_map<std::string, std::string> tempRecoveredPasswords;
	std::unordered_map<std::string, std::string> tempUpdatedAccounts;
};
