#pragma once

#ifdef SUBSCRIPTIONMANAGER_EXPORTS
#define SUBSCRIPTIONMANAGER_API __declspec(dllexport)
#else
#define SUBSCRIPTIONMANAGER_API __declspec(dllimport)
#endif

#include "subscription.h"

#include <fstream>

class SUBSCRIPTIONMANAGER_API SubscriptionManager
{
public:
	void uploadSubscriptions(const std::string& dataBasePath);

	bool checkSubscription(const std::string& licensePlate);

	Subscription find(const std::string& name);

	bool addSubscription(const std::string& name, const std::string& password);

	bool verifyCredentials(const std::string& name, const std::string& password);

public:
	std::vector<Subscription> getSubscriptions() const;

private:
	std::string dataBasePath;
	std::vector<Subscription> subscriptions;
};
