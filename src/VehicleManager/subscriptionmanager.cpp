#include "subscriptionmanager.h"

#include <chrono>
#include <iomanip>
#include <sstream>

void SubscriptionManager::uploadSubscriptions(const std::string& dataBasePath)
{
	this->dataBasePath = dataBasePath;

	std::ifstream readFile(dataBasePath + "subscriptions.txt");
	std::ofstream writeFile(dataBasePath + "subscriptions.txt", std::ios::app);

	std::vector<std::string> subscritionData;
	std::string line;
	while (std::getline(readFile, line))
		if (line.empty())
		{
			Subscription subscription(subscritionData[0], subscritionData[1], subscritionData[2]);

			for (int i = 3; i < subscritionData.size(); i++)
				subscription.addVehicle(subscritionData[i]);

			subscriptions.push_back(subscription);
			subscritionData.clear();
		}
		else
			subscritionData.push_back(line);

	readFile.close();
	writeFile.close();
}

bool SubscriptionManager::checkSubscription(const std::string& licensePlate)
{
	for (const auto& subscription : subscriptions)
		for (const auto& vehicle : subscription.getVehicles())
			if (vehicle == licensePlate)
				return true;

	return false;
}

Subscription SubscriptionManager::find(const std::string& name)
{
	for (const auto& subscription : subscriptions)
		if (subscription.getName() == name)
			return subscription;

	return Subscription();
}

bool SubscriptionManager::addSubscription(const std::string& name, const std::string& password)
{
	std::ofstream writeFile(dataBasePath + "subscriptions.txt", std::ios::app);

	if (find(name).getName() != "")
		return false;

	auto now = std::chrono::system_clock::now();
	auto toTimeT = std::chrono::system_clock::to_time_t(now);

	std::stringstream ss;
	ss << std::put_time(std::localtime(&toTimeT), "%d-%m-%Y %X");

	std::string dateTime = ss.str();

	Subscription subscription(name, password, dateTime);
	subscriptions.push_back(subscription);

	writeFile << name << std::endl;
	writeFile << password << std::endl;
	writeFile << dateTime << std::endl;

	writeFile.close();

	return true;
}

bool SubscriptionManager::verifyCredentials(const std::string& name, const std::string& password)
{
	for (const auto& subscription : subscriptions)
		if (subscription.getName() == name && subscription.getPassword() == password)
			return true;

	return false;
}

std::vector<Subscription> SubscriptionManager::getSubscriptions() const
{
	return subscriptions;
}
