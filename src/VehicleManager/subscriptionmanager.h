#pragma once

#ifdef SUBSCRIPTIONMANAGER_EXPORTS
#define SUBSCRIPTIONMANAGER_API __declspec(dllexport)
#else
#define SUBSCRIPTIONMANAGER_API __declspec(dllimport)
#endif

#include "account.h"
#include "subscription.h"
#include "databasemanager.h"

#include <fstream>
#include <map>


class SUBSCRIPTIONMANAGER_API SubscriptionManager
{
public:
	SubscriptionManager() : databaseManager(DatabaseManager::getInstance()) {}

public:
	void uploadSubscriptions(const std::string& dataBasePath);

	bool checkSubscription(const std::string& licensePlate);

	bool verifyCredentials(const std::string& name, const std::string& password);

public:
	std::map<Account, std::vector<Subscription>> getAccounts() const;

	Account* getAccount(const std::string& email) const;

	std::vector<Subscription>* getSubscriptions(const Account& account) const;

	Subscription* getSubscription(const Account& account, const std::string& name);

	bool addAccount(const std::string& name, const std::string& password);

	bool addSubscription(const Account& account, const std::string& name);

	bool addVehicle(const Account& account, Subscription& subscription, const std::string& licensePlate);

	bool deleteSubscription(const Account& account, const std::string& name);

	bool deleteVehicle(const Account& account, Subscription& subscription, const std::string& licensePlate);

private:
	DatabaseManager& databaseManager;
	std::string dataBasePath;
	std::map<Account, std::vector<Subscription>> accounts;
};
