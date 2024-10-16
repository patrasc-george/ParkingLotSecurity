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

	void handleCreateSubscription(const httplib::Request& request, httplib::Response& response);

	void handleLogin(const httplib::Request& request, httplib::Response& response);

	void handleGetSubscriptionVehicles(const httplib::Request& request, httplib::Response& response);

	void handleAddSubscription(const httplib::Request& request, httplib::Response& response);

	void handleDeleteSubscription(const httplib::Request& request, httplib::Response& response);

	void handleGetVehicleHistory(const httplib::Request& request, httplib::Response& response);

	void handleAddVehicle(const httplib::Request& request, httplib::Response& response);

	void handleDeleteVehicle(const httplib::Request& request, httplib::Response& response);

	void handleUpdateName(const httplib::Request& request, httplib::Response& response);

	void handleUpdatePassword(const httplib::Request& request, httplib::Response& response);

private:
	httplib::Server server;
	std::thread thread;
	VehicleManager& vehicleManager;
	SubscriptionManager* subscriptionManager;
};
