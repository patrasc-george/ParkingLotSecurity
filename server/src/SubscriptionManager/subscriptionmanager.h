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

class SUBSCRIPTIONMANAGER_API SubscriptionManager
{
public:
	SubscriptionManager();

	~SubscriptionManager();

public:
	void uploadSubscriptions();

	bool checkSubscription(const std::string& licensePlate);

	Account* verifyCredentials(const std::string& input, const std::string& password);

	bool verifyAdminCredentials(const std::string& password) const;

	bool pay(const std::string& vehicle, std::string& licensePlate, std::string& dateTime, const bool& isTicket = false);

public:
	std::map<Account, std::vector<Subscription>> getAccounts() const;

	std::unordered_set<std::string> getEmails() const;

	std::vector<std::string> getTempAccount(const std::string& token) const;

	std::string getTempAccountToken(const std::string& email);

	std::string getTempRecoveredPasswordToken(const std::string& email);

	std::string getTempUpdatedAccountToken(const std::string& email);

	std::vector<std::string> getTempUpdatedAccount(const std::string& token);

	Account* getAccountByEmail(const std::string& email) const;

	Account* getAccountByPhone(const std::string& phone) const;

	std::vector<Subscription>* getSubscriptions(const Account& account) const;

	Subscription* getSubscription(const Account& account, const std::string& name);

	std::vector<std::vector<std::string>> getSubscriptionVehicles(const Subscription& subscription);

	std::vector<std::vector<std::string>> getVehicleHistory(const std::string& licensePlate, std::string& totalTimeParked, int& payment);

	void addTempAccount(const std::string& name, const std::string& lastName, const std::string& email, const std::string& password, const std::string& phone);

	bool setToken(const std::string& email, const std::string& token);

	std::string addAccount(const std::string& token);

	bool addTempRecoveredPasswords(const std::string& email, const std::string& token);

	std::string verifyTempRecoveredPasswordsToken(const std::string& token);

	bool addTempUpdatedAccount(const std::string& email, const std::string& newEmail, const std::string& newPassword);

	bool setUpdateToken(const std::string& email, const std::string& token);

	bool addSubscription(const Account& account, const std::string& name);

	bool addVehicle(const Account& account, Subscription& subscription, const std::string& licensePlate);

	bool subscribeNewsletter(const std::string& email);

	bool unsubscribeNewsletter(const std::string& email);

	bool updateAccountPassword(const std::string& email, const std::string& newPassword);

	bool updateAccountInformation(const std::string& email, const std::string& name, const std::string& lastName, const std::string& phone);

	std::string updateAccount(const std::string& token);

	bool deleteSubscription(const Account& account, const std::string& name);

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
