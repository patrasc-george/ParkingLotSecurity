#include "httpServer.h"

Server::Server() : vehicleManager(VehicleManager::getInstance())
{
	server.Post("/api/endpoint", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->handlePost(request, response);
		});

	server.Post("/api/login", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->handleLogin(request, response);
		});

	server.Post("/api/createSubscription", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->handleCreateSubscription(request, response);
		});

	thread = std::thread([this]() { server.listen("localhost", 8080); });
}

Server::~Server()
{
	if (thread.joinable())
	{
		server.stop();
		thread.join();
	}
}

void Server::handlePost(const httplib::Request& request, httplib::Response& response)
{
	std::string licensePlate;
	std::string dateTime;

	if (request.has_param("licensePlate"))
	{
		auto data = request.get_param_value("licensePlate");

		std::transform(data.begin(), data.end(), data.begin(), [](unsigned char c) {
			return std::toupper(c); });

		if (!vehicleManager.pay(data, licensePlate, dateTime))
		{
			response.status = 400;
			response.set_content(R"({"success": false})", "application/json");
			return;
		}
	}
	else if (request.has_file("qrCodeImage"))
	{
		auto data = request.get_file_value("qrCodeImage");

		std::string savePath = vehicleManager.getDataBasePath() + "uploadedQr/" + data.filename;
		std::ofstream ofs(savePath, std::ios::binary);
		ofs << data.content;
		ofs.close();

		if (!vehicleManager.pay(savePath, licensePlate, dateTime, true))
		{
			response.status = 400;
			response.set_content(R"({"success": false})", "application/json");
			return;
		}
	}

	if (!licensePlate.empty() && !dateTime.empty())
		response.set_content("{ \"success\": true, \"licensePlate\": \"" + licensePlate + "\", \"dateTime\": \"" + dateTime + "\" }", "application/json");
	else
	{
		response.status = 400;
		response.set_content(R"({"success": false})", "application/json");
	}
}

void Server::handleCreateSubscription(const httplib::Request& request, httplib::Response& response)
{
	std::string name;
	std::string password;

	subscriptionManager = vehicleManager.getSubscriptionManager();

	if (request.has_param("name"))
		name = request.get_param_value("name");

	if (request.has_param("password"))
		password = request.get_param_value("password");

	if (name.empty() || password.empty())
	{
		response.status = 400;
		response.set_content(R"({"success": false, "message": "Name and password are required."})", "application/json");
		return;
	}

	if (subscriptionManager->addSubscription(name, password))
	{
		response.status = 200;
		response.set_content(R"({"success": true, "message": "Subscription created successfully."})", "application/json");
	}
	else
	{
		response.status = 400;
		response.set_content(R"({"success": false, "message": "Failed to create subscription."})", "application/json");
	}
}

void Server::handleLogin(const httplib::Request& request, httplib::Response& response)
{
	std::string name;
	std::string password;

	subscriptionManager = vehicleManager.getSubscriptionManager();

	if (request.has_param("name"))
		name = request.get_param_value("name");

	if (request.has_param("password"))
		password = request.get_param_value("password");

	if (subscriptionManager->verifyCredentials(name, password))
	{
		Subscription subscription = subscriptionManager->find(name);
		std::vector<std::vector<std::string>> vehiclesTable = vehicleManager.subscriptionVehicles(subscription);

		std::string vehiclesJson = "[";
		for (int i = 0; i < vehiclesTable.size(); i++)
		{
			vehiclesJson += "[";
			for (int j = 0; j < vehiclesTable[i].size(); j++)
			{
				vehiclesJson += "\"" + vehiclesTable[i][j] + "\"";
				if (j < vehiclesTable[i].size() - 1)
					vehiclesJson += ",";
			}
			vehiclesJson += "]";
			if (i < vehiclesTable.size() - 1)
				vehiclesJson += ",";
		}
		vehiclesJson += "]";

		response.status = 200;
		response.set_content("{\"success\": true, \"vehiclesTable\": " + vehiclesJson + "}", "application/json");
	}
	else
	{
		response.status = 401;
		response.set_content(R"({"success": false, "message": "Invalid credentials."})", "application/json");
	}
}
