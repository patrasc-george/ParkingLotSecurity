#include "httplib.h"
#include "vehiclemanager.h"
#include "subscriptionmanager.h"

#include <thread>
#include <iostream>

class Server
{
public:
	Server();
	~Server();

private:
	void handlePost(const httplib::Request& request, httplib::Response& response);
	void handleLogin(const httplib::Request& request, httplib::Response& response);
	void handleCreateSubscription(const httplib::Request& request, httplib::Response& response);

private:
	httplib::Server server;
	std::thread thread;
	VehicleManager& vehicleManager;
	SubscriptionManager* subscriptionManager;
};
