#include "subscriptionmanager.h"

#include <chrono>
#include <iomanip>
#include <sstream>

inline std::vector<std::string> split(const std::string& string, const std::string& delimiter)
{
	if (string.empty())
		return std::vector<std::string>();

	std::vector<std::string> substrings;
	std::string::size_type start = 0;
	std::string::size_type end = string.find(delimiter);

	while (end != std::string::npos)
	{
		substrings.push_back(string.substr(start, end - start));
		start = end + delimiter.length();
		end = string.find(delimiter, start);
	}

	substrings.push_back(string.substr(start));
	return substrings;
}

void SubscriptionManager::uploadSubscriptions(const std::string& dataBasePath)
{
	std::vector<std::string> accountsData = databaseManager.getAccounts();

	for (const auto& accountData : accountsData)
	{
		std::vector<std::string> data;

		data = split(accountData, ", ");
		Account account(data[0], data[1]);
		accounts[account] = {};

		std::unordered_map<std::string, std::pair<std::string, std::string>> subscriptionsData = databaseManager.getSubscriptions(account.getEmail());
		for (const auto& subscriptionData : subscriptionsData)
		{
			Subscription subscription(subscriptionData.first);

			data = split(subscriptionData.second.first, ", ");
			for (const auto& dateTime : data)
				subscription.addDateTime(dateTime);

			data = split(subscriptionData.second.second, ", ");
			for (const auto& vehicle : data)
				subscription.addVehicle(vehicle);

			accounts[account].push_back(subscription);
		}
	}
}

bool SubscriptionManager::checkSubscription(const std::string& licensePlate)
{
	for (const auto& account : accounts)
		for (const auto& subscription : account.second)
			for (const auto& vehicle : subscription.getVehicles())
				if (vehicle == licensePlate)
					return true;

	return false;
}

bool SubscriptionManager::verifyCredentials(const std::string& email, const std::string& password)
{
	for (const auto& account : accounts)
		if (account.first.getEmail() == email && account.first.getPassword() == password)
			return true;

	return nullptr;
}

std::map<Account, std::vector<Subscription>> SubscriptionManager::getAccounts() const
{
	return accounts;
}

Account* SubscriptionManager::getAccount(const std::string& email) const
{
	for (const auto& pair : accounts)
		if (pair.first.getEmail() == email)
			return const_cast<Account*>(&pair.first);

	return nullptr;
}

std::vector<Subscription>* SubscriptionManager::getSubscriptions(const Account& account) const
{
	for (const auto& pair : accounts)
		if (pair.first == account)
			return const_cast<std::vector<Subscription>*>(&pair.second);

	return nullptr;
}

Subscription* SubscriptionManager::getSubscription(const Account& account, const std::string& name)
{
	for (const auto& subscription : accounts[account])
		if (subscription.getName() == name)
			return const_cast<Subscription*>(&subscription);

	return nullptr;
}

bool SubscriptionManager::addAccount(const std::string& email, const std::string& password)
{
	if (getAccount(email) != nullptr)
		return false;

	Account account(email, password);
	accounts[account] = std::vector<Subscription>();
	databaseManager.addAccount(email, password);

	return true;
}

bool SubscriptionManager::addSubscription(const Account& account, const std::string& name)
{
	if (getSubscription(account, name) != nullptr)
		return false;

	Subscription subscription(name);
	accounts[account].push_back(subscription);
	databaseManager.addSubscription(account.getEmail(), name);

	return true;
}

bool SubscriptionManager::addVehicle(const Account& account, Subscription& subscription, const std::string& licensePlate)
{
	if (!subscription.addVehicle(licensePlate))
		return false;

	databaseManager.addLicensePlate(account.getEmail(), subscription.getName(), licensePlate);

	return true;
}

bool SubscriptionManager::deleteSubscription(const Account& account, const std::string& name)
{
	auto it = accounts.find(account);
	if (it == accounts.end())
		return false;

	auto& subscriptions = it->second;
	auto subscription = std::remove_if(subscriptions.begin(), subscriptions.end(), [&name](const Subscription& subscription) {
		return subscription.getName() == name;
		});

	if (subscription != subscriptions.end())
	{
		subscriptions.erase(subscription);
		databaseManager.deleteSubscription(account.getEmail(), name);
		return true;
	}

	return false;
}

bool SubscriptionManager::deleteVehicle(const Account& account, Subscription& subscription, const std::string& licensePlate)
{
	if (!subscription.deleteVehicle(licensePlate))
		return false;

	databaseManager.deleteLicensePlate(account.getEmail(), subscription.getName(), licensePlate);

	return true;
}
