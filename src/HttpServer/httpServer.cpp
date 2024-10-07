#include "httpServer.h"

Server::Server() : vehicleManager(VehicleManager::getInstance())
{
	server.Post("/api/endpoint", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->handlePost(request, response);
		});

	server.Post("/api/createSubscription", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->handleCreateSubscription(request, response);
		});

	server.Post("/api/login", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->handleLogin(request, response);
		});

	server.Post("/api/getSubscriptionVehicles", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->handleGetSubscriptionVehicles(request, response);
		});

	server.Post("/api/addSubscription", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->handleAddSubscription(request, response);
		});

	server.Post("/api/deleteSubscription", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->handleDeleteSubscription(request, response);
		});

	server.Post("/api/getVehicleHistory", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->handleGetVehicleHistory(request, response);
		});

	server.Post("/api/addVehicle", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->handleAddVehicle(request, response);
		});

	server.Post("/api/deleteVehicle", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->handleDeleteVehicle(request, response);
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

		std::string savePath = DatabaseManager::getInstance().getPath() + "uploadedQr/" + data.filename;
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
		response.set_content(R"({"success": false})", "application/json");
		return;
	}

	if (subscriptionManager->addAccount(name, password))
	{
		response.status = 200;
		response.set_content(R"({"success": true})", "application/json");
	}
	else
	{
		response.status = 400;
		response.set_content(R"({"success": false})", "application/json");
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
		Account* account = subscriptionManager->getAccount(name);
		std::vector<Subscription>* subscriptions = subscriptionManager->getSubscriptions(*account);

		std::string subscriptionsJson = "[";
		for (int i = 0; i < subscriptions->size(); i++)
		{
			subscriptionsJson += "[\"" + subscriptions->at(i).getName() + "\"]";
			if (i < subscriptions->size() - 1)
				subscriptionsJson += ",";
		}
		subscriptionsJson += "]";

		response.status = 200;
		response.set_content("{\"success\": true, \"subscriptionsTable\": " + subscriptionsJson + "}", "application/json");
	}
	else
	{
		response.status = 401;
		response.set_content(R"({"success": false})", "application/json");
	}
}

void Server::handleGetSubscriptionVehicles(const httplib::Request& request, httplib::Response& response)
{
	std::string name;
	std::string subscriptionName;

	if (request.has_param("name"))
		name = request.get_param_value("name");

	if (request.has_param("subscriptionName"))
		subscriptionName = request.get_param_value("subscriptionName");

	Account* account = subscriptionManager->getAccount(name);
	Subscription* subscription = subscriptionManager->getSubscription(*account, subscriptionName);
	std::vector<std::vector<std::string>> vehiclesTable = vehicleManager.subscriptionVehicles(*subscription);

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

void Server::handleAddSubscription(const httplib::Request& request, httplib::Response& response)
{
	std::string name;
	std::string subscriptionName;

	if (request.has_param("name"))
		name = request.get_param_value("name");

	if (request.has_param("subscriptionName"))
		subscriptionName = request.get_param_value("subscriptionName");

	Account* account = subscriptionManager->getAccount(name);

	if (subscriptionManager->addSubscription(*account, subscriptionName))
	{
		response.status = 200;
		response.set_content(R"({"success": true})", "application/json");
	}
	else
	{
		response.status = 400;
		response.set_content(R"({"success": false})", "application/json");
	}
}

void Server::handleDeleteSubscription(const httplib::Request& request, httplib::Response& response)
{
	std::string name;
	std::string subscriptionName;

	if (request.has_param("name"))
		name = request.get_param_value("name");

	if (request.has_param("subscriptionName"))
		subscriptionName = request.get_param_value("subscriptionName");

	Account* account = subscriptionManager->getAccount(name);

	if (subscriptionManager->deleteSubscription(*account, subscriptionName))
	{
		response.status = 200;
		response.set_content(R"({"success": true})", "application/json");
	}
	else
	{
		response.status = 400;
		response.set_content(R"({"success": false})", "application/json");
	}
}

void Server::handleGetVehicleHistory(const httplib::Request& request, httplib::Response& response)
{
	std::string licensePlate;

	if (request.has_param("licensePlate"))
	{
		licensePlate = request.get_param_value("licensePlate");
		std::vector<std::vector<std::string>> history = vehicleManager.getVehicleHistory(licensePlate);

		std::string historyJson = "[";

		for (int i = 0; i < history.size(); ++i)
		{
			historyJson += "[";

			for (int j = 0; j < history[i].size(); ++j)
			{
				historyJson += "\"" + history[i][j] + "\"";
				if (j < history[i].size() - 1)
					historyJson += ",";
			}

			historyJson += "]";
			if (i < history.size() - 1)
				historyJson += ",";
		}

		historyJson += "]";

		response.status = 200;
		response.set_content("{\"success\": true, \"history\": " + historyJson + "}", "application/json");
	}
	else
	{
		response.status = 400;
		response.set_content(R"({"success": false})", "application/json");
	}
}

void Server::handleAddVehicle(const httplib::Request& request, httplib::Response& response)
{
	std::string name;
	std::string subscriptionName;
	std::string licensePlate;

	if (request.has_param("name"))
		name = request.get_param_value("name");

	if (request.has_param("subscriptionName"))
		subscriptionName = request.get_param_value("subscriptionName");

	if (request.has_param("licensePlate"))
		licensePlate = request.get_param_value("licensePlate");

	Account* account = subscriptionManager->getAccount(name);
	Subscription* subscription = subscriptionManager->getSubscription(*account, subscriptionName);
	std::transform(licensePlate.begin(), licensePlate.end(), licensePlate.begin(), ::toupper);

	if (subscriptionManager->addVehicle(*account, *subscription, licensePlate))
	{
		std::vector<std::vector<std::string>> vehiclesTable = vehicleManager.subscriptionVehicles(*subscription);
		std::string vehiclesJson = "[[";
		for (int i = 0; i < vehiclesTable[vehiclesTable.size() - 1].size(); i++)
		{
			vehiclesJson += "\"" + vehiclesTable[vehiclesTable.size() - 1][i] + "\"";
			if (i < vehiclesTable[vehiclesTable.size() - 1].size() - 1)
				vehiclesJson += ",";
		}
		vehiclesJson += "]]";

		response.status = 200;
		response.set_content("{\"success\": true, \"vehiclesTable\": " + vehiclesJson + "}", "application/json");
	}
	else
	{
		response.status = 400;
		response.set_content(R"({"success": false})", "application/json");
	}
}

void Server::handleDeleteVehicle(const httplib::Request& request, httplib::Response& response)
{
	std::string name;
	std::string subscriptionName;
	std::string licensePlate;

	if (request.has_param("name"))
		name = request.get_param_value("name");

	if (request.has_param("subscriptionName"))
		subscriptionName = request.get_param_value("subscriptionName");

	if (request.has_param("licensePlate"))
		licensePlate = request.get_param_value("licensePlate");

	Account* account = subscriptionManager->getAccount(name);
	Subscription* subscription = subscriptionManager->getSubscription(*account, subscriptionName);
	std::transform(licensePlate.begin(), licensePlate.end(), licensePlate.begin(), ::toupper);

	if (subscription && subscriptionManager->deleteVehicle(*account, *subscription, licensePlate))
	{
		response.status = 200;
		response.set_content(R"({"success": true})", "application/json");
	}
	else
	{
		response.status = 400;
		response.set_content(R"({"success": false})", "application/json");
	}
}
