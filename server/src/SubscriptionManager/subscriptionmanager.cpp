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

std::string getCurrentTime()
{
	std::time_t now = std::time(nullptr);
	std::tm* localTime = std::localtime(&now);
	std::ostringstream oss;
	oss << std::put_time(localTime, "%d-%m-%Y %H:%M:%S");

	return oss.str();
}

int timeDifference(const std::string& time)
{
	std::string currentTime = getCurrentTime();
	std::tm tmIn = {}, tmOut = {};
	std::istringstream inStr(time);
	std::istringstream outStr(currentTime);

	inStr >> std::get_time(&tmIn, "%d-%m-%Y %H:%M:%S");
	outStr >> std::get_time(&tmOut, "%d-%m-%Y %H:%M:%S");

	auto inTime = std::mktime(&tmIn);
	auto outTime = std::mktime(&tmOut);

	if (inTime == -1 || outTime == -1)
		return 0;

	auto duration = std::difftime(outTime, inTime);
	int minutes = static_cast<int>(duration) / 60;

	return minutes;
}

void removeExpiredToken(std::unordered_map<std::string, std::string>& container, const int& timeIndex, const int& limit)
{
	std::vector<std::string> data;
	int difference;

	for (auto iterator = container.begin(); iterator != container.end(); )
	{
		data = split(iterator->second, ", ");
		difference = timeDifference(data[timeIndex]);

		if (difference >= limit)
			iterator = container.erase(iterator);
		else
			iterator++;
	}
}

SubscriptionManager::SubscriptionManager() : databaseManager(DatabaseManager::getInstance())
{
	databaseManager.initializeDatabase();
	uploadSubscriptions();

	thread = std::thread([this]()
		{
			while (true)
			{
				removeExpiredToken(tempAccounts, 5, 60);
				removeExpiredToken(tempRecoveredPasswords, 1, 60);
				removeExpiredToken(tempUpdatedAccounts, 3, 60);
				std::this_thread::sleep_for(std::chrono::minutes(5));
			}
		});
}

void SubscriptionManager::uploadSubscriptions()
{
	std::vector<std::string> accountsData = databaseManager.getAccounts();

	for (const auto& accountData : accountsData)
	{
		std::vector<std::string> data;

		data = split(accountData, ", ");
		Account account(data[0], data[1], data[2], data[3], data[4]);
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

	newsletter = databaseManager.getNewsletter();
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

Account* SubscriptionManager::verifyCredentials(const std::string& input, const std::string& password)
{
	if (input.find('@') != std::string::npos)
	{
		for (const auto& account : accounts)
			if (account.first.getEmail() == input && account.first.getPassword() == password)
				return const_cast<Account*>(&account.first);
	}
	else
	{
		for (const auto& account : accounts)
			if (account.first.getPhone() == input && account.first.getPassword() == password)
				return const_cast<Account*>(&account.first);
	}

	return nullptr;
}

bool SubscriptionManager::verifyAdminCredentials(const std::string& password) const
{
#ifdef _DEBUG
	if (password == std::getenv("POSTGRES_PASSWORD_DEBUG"))
#else
	if (password == std::getenv("POSTGRES_PASSWORD"))
#endif
		return true;

	return false;
}

bool SubscriptionManager::pay(const std::string& vehicle, std::string& licensePlate, std::string& dateTime, const bool& isTicket)
{
	bool result;
	if (isTicket)
		result = databaseManager.setIsPaid(vehicle, licensePlate, dateTime, isTicket);
	else
		result = databaseManager.setIsPaid(vehicle, licensePlate, dateTime, isTicket);

	return result;
}

std::map<Account, std::vector<Subscription>> SubscriptionManager::getAccounts() const
{
	return accounts;
}

std::unordered_set<std::string> SubscriptionManager::getEmails() const
{
	std::unordered_set<std::string> emails;

	for (const auto& account : accounts)
		emails.insert(account.first.getEmail());

	return emails;
}

std::vector<std::string> SubscriptionManager::getTempAccount(const std::string& token) const
{
	for (const auto& account : tempAccounts)
	{
		std::vector<std::string> data = split(account.second, ", ");
		if (data[6] == token)
			return data;
	}

	return std::vector<std::string>();
}

std::string SubscriptionManager::getTempAccountToken(const std::string& email)
{
	if (tempAccounts.find(email) == tempAccounts.end())
		return "";

	std::vector<std::string> data = split(tempAccounts[email], ", ");

	return data[6];
}

std::string SubscriptionManager::getTempRecoveredPasswordToken(const std::string& email)
{
	if (tempRecoveredPasswords.find(email) == tempRecoveredPasswords.end())
		return "";

	std::vector<std::string> data = split(tempRecoveredPasswords[email], ", ");

	return data[0];
}

std::string SubscriptionManager::getTempUpdatedAccountToken(const std::string& email)
{
	if (tempUpdatedAccounts.find(email) == tempUpdatedAccounts.end())
		return "";

	std::vector<std::string> data = split(tempUpdatedAccounts[email], ", ");

	return data[4];
}

std::vector<std::string> SubscriptionManager::getTempUpdatedAccount(const std::string& token)
{
	for (const auto& account : tempUpdatedAccounts)
	{
		std::vector<std::string> data = split(account.second, ", ");
		if (data[4] == token)
			return data;
	}

	return std::vector<std::string>();
}

Account* SubscriptionManager::getAccountByEmail(const std::string& email) const
{
	for (const auto& pair : accounts)
		if (pair.first.getEmail() == email)
			return const_cast<Account*>(&pair.first);

	return nullptr;
}

Account* SubscriptionManager::getAccountByPhone(const std::string& phone) const
{
	for (const auto& pair : accounts)
		if (pair.first.getPhone() == phone)
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


std::vector<std::vector<std::string>> SubscriptionManager::getSubscriptionVehicles(const Subscription& subscription)
{
	std::vector<std::vector<std::string>> subscribedVehicles(subscription.getVehicles().size());

	for (int i = 0; i < subscription.getVehicles().size(); i++)
	{
		std::string licensePlate = subscription.getVehicles()[i];
		int totalSeconds = 0;
		int payment = 0;
		bool status = false;

		subscribedVehicles[i].push_back(licensePlate);

		std::string lastActivity = databaseManager.getLastVehicleActivity(licensePlate);
		std::vector<std::string> data = split(lastActivity, ", ");

		subscribedVehicles[i].push_back(data[0]);
		subscribedVehicles[i].push_back(data[1]);
		subscribedVehicles[i].push_back(data[2]);
		subscribedVehicles[i].push_back(data[3]);

		subscribedVehicles[i].push_back(databaseManager.getTotalTimeParked(licensePlate));

		subscribedVehicles[i].push_back(std::to_string(databaseManager.getPayment(licensePlate)) + " RON");

		if (!data[0].empty() && data[2].empty())
			subscribedVehicles[i].push_back("Active");
		else
			subscribedVehicles[i].push_back("Inactive");

	}

	return subscribedVehicles;
}

std::vector<std::vector<std::string>> SubscriptionManager::getVehicleHistory(const std::string& licensePlate, std::string& totalTimeParked, int& payment)
{
	std::vector<std::vector<std::string>> history;
	std::vector<std::string> data = databaseManager.getVehicleHistory(licensePlate);

	for (int i = 0; i < data.size(); i++)
	{
		history.push_back(std::vector<std::string>());
		std::vector<std::string> vehicle = split(data[i], ", ");

		history.back().push_back(vehicle[0]);
		history.back().push_back(vehicle[1]);
		history.back().push_back(vehicle[2]);
		history.back().push_back(vehicle[3]);
	}

	totalTimeParked = databaseManager.getTotalTimeParked(licensePlate);
	payment = databaseManager.getPayment(licensePlate);

	return history;
}

void SubscriptionManager::addTempAccount(const std::string& name, const std::string& lastName, const std::string& email, const std::string& password, const std::string& phone)
{
	tempAccounts[email] = name + ", " + lastName + ", " + email + ", " + password + ", " + phone + ", " + getCurrentTime();
}

bool SubscriptionManager::setToken(const std::string& email, const std::string& token)
{
	if (tempAccounts.find(email) == tempAccounts.end())
		return false;

	tempAccounts[email] += ", " + token + ", " + "false";

	return true;
}

std::string SubscriptionManager::addAccount(const std::string& token)
{
	std::vector<std::string> data = getTempAccount(token);

	if (data.empty())
		return "";

	Account account(data[0], data[1], data[2], data[3], data[4]);
	if (data[7] == "false")
	{
		accounts[account] = std::vector<Subscription>();
		databaseManager.addAccount(data[0], data[1], data[2], data[3], data[4]);

		tempAccounts[data[2]] = data[0] + ", " + data[1] + ", " + data[2] + ", " + data[3] + ", " + data[4] + ", " + data[5] + ", " + data[6] + ", " + "true";
	}
	else
		tempAccounts.erase(data[2]);

	return account.getEmail();
}

bool SubscriptionManager::addTempRecoveredPasswords(const std::string& email, const std::string& token)
{
	if (getAccountByEmail(email) == nullptr)
		return false;

	tempRecoveredPasswords[email] = token + ", " + getCurrentTime();

	return true;
}

std::string SubscriptionManager::verifyTempRecoveredPasswordsToken(const std::string& token)
{
	std::string email;
	for (const auto& tempRecoveredPassword : tempRecoveredPasswords)
	{
		std::vector<std::string> data = split(tempRecoveredPassword.second, ", ");

		if (data[0] == token)
		{
			email = tempRecoveredPassword.first;
			return email;
		}
	}

	return "";
}

bool SubscriptionManager::addTempUpdatedAccount(const std::string& email, const std::string& newEmail, const std::string& newPassword)
{
	if (!newEmail.empty() && getAccountByEmail(newEmail) != nullptr)
		return false;

	tempUpdatedAccounts[email] = email + ", " + newEmail + ", " + newPassword + ", " + getCurrentTime();

	return true;
}

bool SubscriptionManager::setUpdateToken(const std::string& email, const std::string& token)
{
	if (tempUpdatedAccounts.find(email) == tempUpdatedAccounts.end())
		return false;

	tempUpdatedAccounts[email] += ", " + token + ", " + "false";

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

bool SubscriptionManager::subscribeNewsletter(const std::string& email)
{
	if (newsletter.find(email) != newsletter.end())
		return false;

	newsletter.insert(email);
	databaseManager.subscribeNewsletter(email);

	return true;
}

bool SubscriptionManager::unsubscribeNewsletter(const std::string& email)
{
	if (newsletter.find(email) == newsletter.end())
		return false;

	newsletter.erase(email);
	databaseManager.unsubscribeNewsletter(email);

	return true;
}

bool SubscriptionManager::updateAccountPassword(const std::string& email, const std::string& newPassword)
{
	if (tempRecoveredPasswords.find(email) == tempRecoveredPasswords.end())
		return false;

	tempRecoveredPasswords.erase(email);

	Account* account = getAccountByEmail(email);
	account->setPassword(newPassword);

	databaseManager.setPassword(email, newPassword);

	return true;
}

bool SubscriptionManager::updateAccountInformation(const std::string& email, const std::string& name, const std::string& lastName, const std::string& phone)
{
	Account* account = getAccountByEmail(email);

	if (account == nullptr)
		return false;

	account->setName(name);
	account->setLastName(lastName);
	account->setPhone(phone);

	databaseManager.setName(email, name);
	databaseManager.setLastName(email, lastName);
	databaseManager.setPhone(email, phone);

	return true;
}

std::string SubscriptionManager::updateAccount(const std::string& token)
{
	std::vector<std::string> data = getTempUpdatedAccount(token);

	if (data.empty())
		return "";

	if (data[5] == "true")
	{
		tempUpdatedAccounts.erase(data[0]);

		if (!data[1].empty())
			return data[1];

		return data[0];
	}

	std::string email = data[0];
	Account* account = getAccountByEmail(email);

	account->setPassword(data[2]);
	account->setEmail(data[1]);

	databaseManager.setPassword(email, data[2]);
	databaseManager.setEmail(email, data[1]);

	tempUpdatedAccounts[email] = data[0] + ", " + data[1] + ", " + data[2] + ", " + data[3] + ", " + data[4] + ", " + "true";

	return account->getEmail();
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

SubscriptionManager::~SubscriptionManager()
{
	if (thread.joinable())
		thread.join();
}
