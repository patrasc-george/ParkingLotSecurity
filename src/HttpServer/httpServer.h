#include "httplib.h"
#include "vehiclemanager.h"

#include <thread>
#include <iostream>

class Server
{
public:
	Server();
	~Server();

private:
	void handlePost(const httplib::Request& req, httplib::Response& res);

private:
	httplib::Server server;
	std::thread thread;
	VehicleManager& vehicleManager;
};
