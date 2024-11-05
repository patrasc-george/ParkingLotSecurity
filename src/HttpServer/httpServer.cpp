#include "httpServer.h"

Server::Server() : vehicleManager(VehicleManager::getInstance()), session("smtp.mail.yahoo.com", 587)
{
	subscriptionManager = vehicleManager.getSubscriptionManager();

	pCert = new Poco::Net::AcceptCertificateHandler(false);
	pContext = new Poco::Net::Context(Poco::Net::Context::CLIENT_USE, "", "", "", Poco::Net::Context::VERIFY_RELAXED);
	Poco::Net::SSLManager::instance().initializeClient(0, pCert, pContext);

	session.open();
	session.startTLS();
	session.login(Poco::Net::SMTPClientSession::AUTH_PLAIN, "patrasc_george@yahoo.com", "xksjsjusdlteprrc");

	server.Post("/api/endpoint", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->handlePost(request, response);
		});

	server.Post("/api/createAccount", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->handleCreateAccount(request, response);
		});

	server.Post("/api/validate", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->handleValidate(request, response);
		});

	server.Options("/api/validate", [](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		response.status = 200;
		});

	server.Post("/api/login", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->handleLogin(request, response);
		});

	server.Post("/api/recoverPassword", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->handleRecoverPassword(request, response);
		});

	server.Post("/api/resetPassword", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->handleResetPassword(request, response);
		});

	server.Options("/api/resetPassword", [](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		response.status = 200;
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

	server.Post("/api/updateName", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->handleUpdateName(request, response);
		});

	server.Post("/api/updatePassword", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->handleUpdatePassword(request, response);
		});

	server.Post("/api/updatePhone", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->handleUpdatePhone(request, response);
		});

	thread = std::thread([this]() { server.listen("localhost", 8080); });
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
			response.set_content(R"({"success": false, "message": "The vehicle was not found. Please upload the QR code."})", "application/json");
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
			response.set_content(R"({"success": false, "message": "The vehicle was not found. Please upload the QR code again."})", "application/json");
			return;
		}
	}

	if (!licensePlate.empty() && !dateTime.empty())
		response.set_content("{ \"success\": true, \"message\": \"The vehicle with license plate number " + licensePlate + " was found. The parking fee has been paid. The vehicle entered the parking lot on " + dateTime + ".\", \"licensePlate\": \"" + licensePlate + "\", \"dateTime\": \"" + dateTime + "\" }", "application/json");
	else
		response.set_content(R"({"success": false})", "application/json");
}

void Server::handleCreateAccount(const httplib::Request& request, httplib::Response& response)
{
	std::string name, email, password, phone;

	if (request.has_param("name"))
		name = request.get_param_value("name");
	if (request.has_param("email"))
		email = request.get_param_value("email");
	if (request.has_param("password"))
		password = request.get_param_value("password");
	if (request.has_param("phone"))
		phone = request.get_param_value("phone");

	std::string token = uuidGenerator.createRandom().toString();

	if (!subscriptionManager->addTempAccount(name, email, password, phone, token))
	{
		response.set_content(R"({"success": false, "message": "An account with this email address already exists."})", "application/json");
		return;
	}

	std::string content = "Hello, " + name + ",\n\nThank you for registering!\n\nTo complete the setup of your account, please validate your email address by clicking the link below: http://localhost:4200/validate?token=" + token;

	Poco::Net::MailMessage message;
	message.setSender("patrasc_george@yahoo.com");
	message.addRecipient(Poco::Net::MailRecipient(Poco::Net::MailRecipient::PRIMARY_RECIPIENT, email));
	message.setSubject("Validate Account");
	message.setContent(content);
	message.setContentType("text/plain; charset=UTF-8");

	session.sendMessage(message);

	response.set_content(R"({"success": true})", "application/json");
}

void Server::handleValidate(const httplib::Request& request, httplib::Response& response)
{
	std::string token;

	if (request.has_param("token"))
		token = request.get_param_value("token");

	if (subscriptionManager->addAccount(token))
		response.set_content(R"({"success": true})", "application/json");
	else
		response.set_content(R"({"success": false})", "application/json");
}

void Server::handleLogin(const httplib::Request& request, httplib::Response& response)
{
	std::string email;
	std::string password;

	if (request.has_param("email"))
		email = request.get_param_value("email");

	if (request.has_param("password"))
		password = request.get_param_value("password");

	if (subscriptionManager->verifyCredentials(email, password))
	{
		Account* account = subscriptionManager->getAccount(email);
		std::vector<Subscription>* subscriptions = subscriptionManager->getSubscriptions(*account);

		std::string subscriptionsJson = "[";
		for (int i = 0; i < subscriptions->size(); i++)
		{
			subscriptionsJson += "[\"" + subscriptions->at(i).getName() + "\"]";
			if (i < subscriptions->size() - 1)
				subscriptionsJson += ",";
		}
		subscriptionsJson += "]";

		response.set_content("{\"success\": true, \"subscriptionsTable\": " + subscriptionsJson + "}", "application/json");
	}
	else
		response.set_content(R"({"success": false})", "application/json");
}

void Server::handleRecoverPassword(const httplib::Request& request, httplib::Response& response)
{
	std::string email;

	if (request.has_param("email"))
		email = request.get_param_value("email");

	Account* account = subscriptionManager->getAccount(email);

	if (account == nullptr)
	{
		response.set_content(R"({"success": false})", "application/json");
		return;
	}

	subscriptionManager->addTempPassword(email);

	std::string content = "Hello, " + account->getName() + ",\n\nTo reset the password for your account, please click the link below: http://localhost:4200/reset-password?email=" + email;

	Poco::Net::MailMessage message;
	message.setSender("patrasc_george@yahoo.com");
	message.addRecipient(Poco::Net::MailRecipient(Poco::Net::MailRecipient::PRIMARY_RECIPIENT, email));
	message.setSubject("Reset Password");
	message.setContent(content);
	message.setContentType("text/plain; charset=UTF-8");

	session.sendMessage(message);

	response.set_content(R"({"success": true})", "application/json");
}


void Server::handleResetPassword(const httplib::Request& request, httplib::Response& response)
{
	std::string email;
	std::string newPassword;

	if (request.has_param("email"))
		email = request.get_param_value("email");

	if (request.has_param("newPassword"))
		newPassword = request.get_param_value("newPassword");

	if (subscriptionManager->updateAccountPassword(email, newPassword))
		response.set_content(R"({"success": true})", "application/json");
	else
		response.set_content(R"({"success": false})", "application/json");
}

void Server::handleGetSubscriptionVehicles(const httplib::Request& request, httplib::Response& response)
{
	std::string email;
	std::string subscriptionName;

	if (request.has_param("email"))
		email = request.get_param_value("email");

	if (request.has_param("subscriptionName"))
		subscriptionName = request.get_param_value("subscriptionName");

	Account* account = subscriptionManager->getAccount(email);
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

	response.set_content("{\"success\": true, \"vehiclesTable\": " + vehiclesJson + "}", "application/json");
}

void Server::handleAddSubscription(const httplib::Request& request, httplib::Response& response)
{
	std::string email;
	std::string subscriptionName;

	if (request.has_param("email"))
		email = request.get_param_value("email");

	if (request.has_param("subscriptionName"))
		subscriptionName = request.get_param_value("subscriptionName");

	Account* account = subscriptionManager->getAccount(email);

	if (subscriptionManager->addSubscription(*account, subscriptionName))
		response.set_content(R"({"success": true})", "application/json");
	else
		response.set_content(R"({"success": false})", "application/json");
}

void Server::handleDeleteSubscription(const httplib::Request& request, httplib::Response& response)
{
	std::string email;
	std::string subscriptionName;

	if (request.has_param("email"))
		email = request.get_param_value("email");

	if (request.has_param("subscriptionName"))
		subscriptionName = request.get_param_value("subscriptionName");

	Account* account = subscriptionManager->getAccount(email);

	if (subscriptionManager->deleteSubscription(*account, subscriptionName))
		response.set_content(R"({"success": true})", "application/json");
	else
		response.set_content(R"({"success": false})", "application/json");
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

		response.set_content("{\"success\": true, \"history\": " + historyJson + "}", "application/json");
	}
	else
		response.set_content(R"({"success": false})", "application/json");
}

void Server::handleAddVehicle(const httplib::Request& request, httplib::Response& response)
{
	std::string email;
	std::string subscriptionName;
	std::string licensePlate;

	if (request.has_param("email"))
		email = request.get_param_value("email");

	if (request.has_param("subscriptionName"))
		subscriptionName = request.get_param_value("subscriptionName");

	if (request.has_param("licensePlate"))
		licensePlate = request.get_param_value("licensePlate");

	Account* account = subscriptionManager->getAccount(email);
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

		response.set_content("{\"success\": true, \"vehiclesTable\": " + vehiclesJson + "}", "application/json");
	}
	else
		response.set_content(R"({"success": false})", "application/json");
}

void Server::handleDeleteVehicle(const httplib::Request& request, httplib::Response& response)
{
	std::string email;
	std::string subscriptionName;
	std::string licensePlate;

	if (request.has_param("email"))
		email = request.get_param_value("email");

	if (request.has_param("subscriptionName"))
		subscriptionName = request.get_param_value("subscriptionName");

	if (request.has_param("licensePlate"))
		licensePlate = request.get_param_value("licensePlate");

	Account* account = subscriptionManager->getAccount(email);
	Subscription* subscription = subscriptionManager->getSubscription(*account, subscriptionName);
	std::transform(licensePlate.begin(), licensePlate.end(), licensePlate.begin(), ::toupper);

	if (subscription && subscriptionManager->deleteVehicle(*account, *subscription, licensePlate))
		response.set_content(R"({"success": true})", "application/json");
	else
		response.set_content(R"({"success": false})", "application/json");
}

void Server::handleUpdateName(const httplib::Request& request, httplib::Response& response)
{
	std::string email;
	std::string currentPassword;
	std::string newEmail;

	if (request.has_param("email"))
		email = request.get_param_value("email");

	if (request.has_param("currentPassword"))
		currentPassword = request.get_param_value("currentPassword");

	if (request.has_param("newEmail"))
		newEmail = request.get_param_value("newEmail");

	if (!subscriptionManager->verifyCredentials(email, currentPassword))
	{
		response.set_content(R"({"success": false, "message": "The current password is incorrect."})", "application/json");
		return;
	}

	if (subscriptionManager->getAccount(newEmail) == nullptr)
	{
		subscriptionManager->updateAccountEmail(email, newEmail);
		response.set_content(R"({"success": true})", "application/json");
	}
	else
		response.set_content(R"({"success": false, "message": "Email already exists."})", "application/json");
}

void Server::handleUpdatePassword(const httplib::Request& request, httplib::Response& response)
{
	std::string email;
	std::string currentPassword;
	std::string newPassword;

	if (request.has_param("email"))
		email = request.get_param_value("email");

	if (request.has_param("currentPassword"))
		currentPassword = request.get_param_value("currentPassword");

	if (request.has_param("newPassword"))
		newPassword = request.get_param_value("newPassword");

	if (subscriptionManager->verifyCredentials(email, currentPassword))
	{
		subscriptionManager->updateAccountPassword(email, newPassword);
		response.set_content(R"({"success": true})", "application/json");
	}
	else
		response.set_content(R"({"success": false})", "application/json");
}

void Server::handleUpdatePhone(const httplib::Request& request, httplib::Response& response)
{
	std::string email;
	std::string currentPassword;
	std::string newPhone;

	if (request.has_param("email"))
		email = request.get_param_value("email");

	if (request.has_param("currentPassword"))
		currentPassword = request.get_param_value("currentPassword");

	if (request.has_param("newPhone"))
		newPhone = request.get_param_value("newPhone");

	if (subscriptionManager->verifyCredentials(email, currentPassword))
	{
		subscriptionManager->updateAccountPhone(email, newPhone);
		response.set_content(R"({"success": true})", "application/json");
	}
	else
		response.set_content(R"({"success": false})", "application/json");
}

Server::~Server()
{
	if (thread.joinable())
	{
		server.stop();
		thread.join();
	}

	session.close();
}
