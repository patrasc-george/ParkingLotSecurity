#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httpServer.h"

Server::Server() : vehicleManager(VehicleManager::getInstance())
{
	subscriptionManager = vehicleManager.getSubscriptionManager();

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

	server.Post("/api/validateViaEmail", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->handleValidateViaEmail(request, response);
		});

	server.Post("/api/validateViaSMS", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->handleValidateViaSMS(request, response);
		});

	server.Post("/api/resendValidateSMS", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->handleResendValidateSMS(request, response);
		});

	server.Post("/api/validate", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->handleValidate(request, response);
		});

	server.Post("/api/login", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->handleLogin(request, response);
		});

	server.Post("/api/recoverPasswordViaEmail", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->handleRecoverPasswordViaEmail(request, response);
		});

	server.Post("/api/recoverPasswordViaSMS", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->handleRecoverPasswordViaSMS(request, response);
		});

	server.Post("/api/resendRecoverPassword", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->handleResendRecoverPassword(request, response);
		});

	server.Post("/api/verifyResetPasswordToken", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->handleVerifyResetPasswordToken(request, response);
		});

	server.Post("/api/resetPassword", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->handleResetPassword(request, response);
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

	server.Post("/api/updateAccountInformation", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->handleUpdateAccountInformation(request, response);
		});

	server.Post("/api/updateAccount", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->handleUpdateAccount(request, response);
		});

	server.Post("/api/validateUpdateViaEmail", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->handleValidateUpdateViaEmail(request, response);
		});

	server.Post("/api/validateUpdateViaSMS", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->handleValidateUpdateViaSMS(request, response);
		});

	server.Post("/api/resendValidateUpdateSMS", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->handleResendValidateUpdateSMS(request, response);
		});

	server.Post("/api/validateUpdate", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->handleValidateUpdate(request, response);
		});

	server.Post("/api/contact", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->handleContact(request, response);
		});

	server.Post("/api/subscribeNewsletter", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->handleSubscribeNewsletter(request, response);
		});

	server.Post("/api/unsubscribeNewsletter", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->handleUnsubscribeNewsletter(request, response);
		});

	thread = std::thread([this]() { server.listen("localhost", 8080); });
}

std::string Server::generateToken()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(100000, 999999);
	std::string token = std::to_string(dis(gen));

	return token;
}

void Server::sendEmail(const std::string& email, const std::string& subject, const std::string& content)
{
	std::string pocoEmail(std::getenv("pocoEmail"));
	std::string pocoPassword(std::getenv("pocoPassword"));

	Poco::Net::MailMessage message;
	message.setSender(pocoEmail);
	message.addRecipient(Poco::Net::MailRecipient(Poco::Net::MailRecipient::PRIMARY_RECIPIENT, email));
	message.setSubject("ParkPass: " + subject);
	message.setContent(content);
	message.setContentType("text/plain; charset=UTF-8");

	Poco::Net::SecureSMTPClientSession session("smtp.mail.yahoo.com", 587);
	Poco::SharedPtr<Poco::Net::InvalidCertificateHandler> pCert = new Poco::Net::AcceptCertificateHandler(false);
	Poco::Net::Context::Ptr pContext = new Poco::Net::Context(Poco::Net::Context::CLIENT_USE, "", "", "", Poco::Net::Context::VERIFY_RELAXED);
	Poco::Net::SSLManager::instance().initializeClient(0, pCert, pContext);

	session.open();
	session.startTLS();
	session.login(Poco::Net::SMTPClientSession::AUTH_PLAIN, pocoEmail, pocoPassword);

	session.sendMessage(message);

	session.close();
}

void Server::sendSMS(const std::string& phone, const std::string& content)
{
	std::string twiloSid(std::getenv("twiloSid"));
	std::string twiloToken(std::getenv("twiloToken"));
	std::string twiloPhone(std::getenv("twiloPhone"));

	httplib::SSLClient client("api.twilio.com");
	client.set_basic_auth(twiloSid.c_str(), twiloToken.c_str());

	std::string url = "/2010-04-01/Accounts/" + twiloSid + "/Messages.json";

	httplib::Params params;
	params.emplace("To", "+40" + phone);
	params.emplace("From", twiloPhone);
	params.emplace("Body", content);

	auto res = client.Post(url.c_str(), params);

	std::string str = res->body;
	std::cout << " ";
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
	std::string name;
	std::string lastName;
	std::string	email;
	std::string password;
	std::string phone;
	std::string captchaToken;

	if (request.has_param("name"))
		name = request.get_param_value("name");
	if (request.has_param("lastName"))
		lastName = request.get_param_value("lastName");
	if (request.has_param("email"))
		email = request.get_param_value("email");
	if (request.has_param("password"))
		password = request.get_param_value("password");
	if (request.has_param("phone"))
		phone = request.get_param_value("phone");
	if (request.has_param("captchaToken"))
		captchaToken = request.get_param_value("captchaToken");

	httplib::SSLClient recaptchaClient("www.google.com");
	std::string secretKey(std::getenv("reCaptchaKey"));
	std::string payload = "secret=" + secretKey + "&response=" + captchaToken;

	auto recaptchaResponse = recaptchaClient.Post("/recaptcha/api/siteverify", payload, "application/x-www-form-urlencoded");
	if (!recaptchaResponse || recaptchaResponse->status != 200)
	{
		response.set_content(R"({"success": false})", "application/json");
		return;
	}

	auto recaptchaBody = recaptchaResponse->body;
	Poco::JSON::Parser parser;
	auto recaptchaJson = parser.parse(recaptchaBody).extract<Poco::JSON::Object::Ptr>();
	bool captchaSuccess = recaptchaJson->getValue<bool>("success");

	if (!captchaSuccess)
	{
		response.set_content(R"({"success": false})", "application/json");
		return;
	}

	if (subscriptionManager->getAccountByEmail(email) != nullptr)
	{
		response.set_content(R"({"success": false, "message": "An account with this email address already exists."})", "application/json");
		return;
	}

	if (subscriptionManager->getAccountByPhone(phone) != nullptr)
	{
		response.set_content(R"({"success": false, "message": "An account with this phone number already exists."})", "application/json");
		return;
	}

	subscriptionManager->addTempAccount(name, lastName, email, password, phone);
	response.set_content(R"({"success": true})", "application/json");
}

void Server::handleValidateViaEmail(const httplib::Request& request, httplib::Response& response)
{
	std::string name;
	std::string	email;

	if (request.has_param("name"))
		name = request.get_param_value("name");
	if (request.has_param("email"))
		email = request.get_param_value("email");

	std::string token = generateToken();
	subscriptionManager->setToken(email, token);

	std::string subject = "Validare cont";
	std::string content =
		"Stimata/Stimate, " + name + ",\n\n"
		"Iti multumim ca ai ales ParkPass ca solutie de plata pentru parcarea ta!\n\n"
		"Pentru a finaliza configurarea contului tau, te rugam sa confirmi adresa de e-mail asociata acestuia.\n\n"
		"Pentru a continua, acceseaza linkul de mai jos:\n"
		"http://localhost:4200/validate?token=" + token + "\n\n"
		"Daca nu ai creat un cont ParkPass, te rugam sa ignori acest e-mail.\n\n"
		"Iti multumim si bun venit la ParkPass!\n\n"
		"Cu respect,\n"
		"Echipa ParkPass";
	sendEmail(email, subject, content);

	response.set_content(R"({"success": true})", "application/json");
}

void Server::handleValidateViaSMS(const httplib::Request& request, httplib::Response& response)
{
	std::string	email;
	std::string phone;

	if (request.has_param("email"))
		email = request.get_param_value("email");
	if (request.has_param("phone"))
		phone = request.get_param_value("phone");

	std::string token = generateToken();
	subscriptionManager->setToken(email, token);

	std::string content = "Codul de validare pentru contul tau ParkPass este: " + token;
	sendSMS(phone, content);

	response.set_content(R"({"success": true})", "application/json");
}

void Server::handleResendValidateSMS(const httplib::Request& request, httplib::Response& response)
{
	std::string	email;
	std::string phone;

	if (request.has_param("email"))
		email = request.get_param_value("email");
	if (request.has_param("phone"))
		phone = request.get_param_value("phone");

	std::string token = subscriptionManager->getTempAccountToken(email);
	std::string content = "Codul de validare pentru contul tau ParkPass este: " + token;
	sendSMS(phone, content);

	response.set_content(R"({"success": true})", "application/json");
}

void Server::handleValidate(const httplib::Request& request, httplib::Response& response)
{
	std::string token;

	if (request.has_param("token"))
		token = request.get_param_value("token");

	std::string email = subscriptionManager->addAccount(token);
	if (!email.empty())
	{
		response.set_content("{\"success\": true, \"email\": \"" + email + "\"}", "application/json");
	}
	else
		response.set_content(R"({"success": false})", "application/json");
}

void Server::handleLogin(const httplib::Request& request, httplib::Response& response)
{
	std::string input;
	std::string password;
	bool fromRedirect = false;

	if (request.has_param("input"))
		input = request.get_param_value("input");

	if (request.has_param("password"))
		password = request.get_param_value("password");

	if (request.has_param("fromRedirect"))
		if (request.get_param_value("fromRedirect") == "true")
			fromRedirect = true;

	Account* account;
	if (!fromRedirect)
	{
		account = subscriptionManager->verifyCredentials(input, password);
		if (account == nullptr)
		{
			response.set_content(R"({"success": false})", "application/json");
			return;
		}
	}
	else
		account = subscriptionManager->getAccountByEmail(input);

	std::string name = account->getName();
	std::string lastName = account->getLastName();
	std::string email = account->getEmail();
	std::string phone = account->getPhone();

	std::vector<Subscription>* subscriptions = subscriptionManager->getSubscriptions(*account);

	std::string subscriptionsJson = "[";
	for (int i = 0; i < subscriptions->size(); i++)
	{
		subscriptionsJson += "[\"" + subscriptions->at(i).getName() + "\"]";
		if (i < subscriptions->size() - 1)
			subscriptionsJson += ",";
	}
	subscriptionsJson += "]";

	response.set_content("{\"success\": true, \"name\": \"" + name + "\", \"lastName\": \"" + lastName + "\", \"email\": \"" + email + "\", \"phone\": \"" + phone + "\", \"subscriptionsTable\": " + subscriptionsJson + "}", "application/json");
}

void Server::handleRecoverPasswordViaEmail(const httplib::Request& request, httplib::Response& response)
{
	std::string email;

	if (request.has_param("email"))
		email = request.get_param_value("email");

	Account* account = subscriptionManager->getAccountByEmail(email);

	if (account == nullptr)
	{
		response.set_content(R"({"success": false})", "application/json");
		return;
	}

	std::string token = generateToken();
	subscriptionManager->addTempRecoveredPasswords(email, token);

	std::string subject = "Resetare parola";
	std::string content =
		"Stimata/Stimate, " + account->getName() + ",\n\n"
		"Am primit o solicitare pentru resetarea parolei contului tau ParkPass.\n\n"
		"Pentru a continua te rugam sa accesezi linkul de mai jos:\n"
		"http://localhost:4200/reset-password?token=" + token + "\n\n"
		"Daca nu ai solicitat resetarea parolei, te rugam sa nu accesezi linkul. Pentru siguranta ta, iti recomandam sa verifici activitatea contului pentru a te asigura ca este securizat.\n\n"
		"Iti multumim ca folosesti ParkPass. Suntem aici sa te ajutam cu orice intrebari sau nelamuriri.\n\n"
		"Cu respect,\n"
		"Echipa ParkPass";
	sendEmail(email, subject, content);

	response.set_content(R"({"success": true})", "application/json");
}

void Server::handleRecoverPasswordViaSMS(const httplib::Request& request, httplib::Response& response)
{
	std::string phone;

	if (request.has_param("phone"))
		phone = request.get_param_value("phone");

	Account* account = subscriptionManager->getAccountByPhone(phone);

	if (account == nullptr)
	{
		response.set_content(R"({"success": false})", "application/json");
		return;
	}

	std::string token = generateToken();

	subscriptionManager->addTempRecoveredPasswords(account->getEmail(), token);

	std::string content = "Codul de resetare a parolei pentru contul tau ParkPass este: " + token;
	sendSMS(phone, content);

	response.set_content(R"({"success": true})", "application/json");
}

void Server::handleResendRecoverPassword(const httplib::Request& request, httplib::Response& response)
{
	std::string phone;

	if (request.has_param("phone"))
		phone = request.get_param_value("phone");

	Account* account = subscriptionManager->getAccountByPhone(phone);

	if (account == nullptr)
	{
		response.set_content(R"({"success": false})", "application/json");
		return;
	}

	std::string token = subscriptionManager->getTempRecoveredPasswordToken(account->getEmail());

	std::string content = "Codul de resetare a parolei pentru contul tau ParkPass este: " + token;
	sendSMS(phone, content);

	response.set_content(R"({"success": true})", "application/json");
}

void Server::handleVerifyResetPasswordToken(const httplib::Request& request, httplib::Response& response)
{
	std::string token;

	if (request.has_param("token"))
		token = request.get_param_value("token");

	std::string email = subscriptionManager->verifyTempRecoveredPasswordsToken(token);

	if (!email.empty())
		response.set_content("{\"success\": true, \"email\": \"" + email + "\"}", "application/json");
	else
		response.set_content(R"({"success": false})", "application/json");

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

	Account* account = subscriptionManager->getAccountByEmail(email);
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

	Account* account = subscriptionManager->getAccountByEmail(email);

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

	Account* account = subscriptionManager->getAccountByEmail(email);

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

	Account* account = subscriptionManager->getAccountByEmail(email);
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

	Account* account = subscriptionManager->getAccountByEmail(email);
	Subscription* subscription = subscriptionManager->getSubscription(*account, subscriptionName);
	std::transform(licensePlate.begin(), licensePlate.end(), licensePlate.begin(), ::toupper);

	if (subscription && subscriptionManager->deleteVehicle(*account, *subscription, licensePlate))
		response.set_content(R"({"success": true})", "application/json");
	else
		response.set_content(R"({"success": false})", "application/json");
}

void Server::handleUpdateAccountInformation(const httplib::Request& request, httplib::Response& response)
{
	std::string newName;
	std::string newLastName;
	std::string newPhone;
	std::string email;

	if (request.has_param("newName"))
		newName = request.get_param_value("newName");

	if (request.has_param("newLastName"))
		newLastName = request.get_param_value("newLastName");

	if (request.has_param("newPhone"))
		newPhone = request.get_param_value("newPhone");

	if (request.has_param("email"))
		email = request.get_param_value("email");

	if (!newPhone.empty())
		if (subscriptionManager->getAccountByPhone(newPhone) != nullptr)
		{
			response.set_content(R"({"success": false})", "application/json");
			return;
		}

	if (subscriptionManager->updateAccountInformation(email, newName, newLastName, newPhone))
		response.set_content(R"({"success": true})", "application/json");
	else
		response.set_content(R"({"success": false})", "application/json");
}

void Server::handleUpdateAccount(const httplib::Request& request, httplib::Response& response)
{
	std::string newEmail;
	std::string newPassword;
	std::string email;

	if (request.has_param("newEmail"))
		newEmail = request.get_param_value("newEmail");

	if (request.has_param("newPassword"))
		newPassword = request.get_param_value("newPassword");

	if (request.has_param("email"))
		email = request.get_param_value("email");

	Account* account = subscriptionManager->getAccountByEmail(email);

	if (account == nullptr)
	{
		response.set_content(R"({"success": false})", "application/json");
		return;
	}

	if (!subscriptionManager->addTempUpdatedAccount(email, newEmail, newPassword))
	{
		response.set_content(R"({"success": false})", "application/json");
		return;
	}

	response.set_content(R"({"success": true})", "application/json");
}

void Server::handleValidateUpdateViaEmail(const httplib::Request& request, httplib::Response& response)
{
	std::string name;
	std::string	email;

	if (request.has_param("name"))
		name = request.get_param_value("name");
	if (request.has_param("email"))
		email = request.get_param_value("email");

	std::string token = generateToken();
	subscriptionManager->setUpdateToken(email, token);

	std::string subject = "Actualizare informatii cont";
	std::string content =
		"Stimata/Stimate, " + name + ",\n\n"
		"Am primit o solicitare pentru a actualiza informatiile contului tau ParkPass.\n\n"
		"Pentru a confirma aceste modificari, te rugam sa accesezi linkul de mai jos:\n"
		"http://localhost:4200/validate-update?token=" + token + "\n\n"
		"Daca nu ai solicitat nicio modificare a informatiilor contului, te rugam sa ignori acest e-mail. "
		"Din motive de siguranta, iti recomandam sa verifici periodic activitatea contului tau.\n\n"
		"Iti multumim ca folosesti ParkPass. Suntem aici sa te ajutam cu orice intrebari sau nelamuriri.\n\n"
		"Cu respect,\n"
		"Echipa ParkPass";

	sendEmail(email, subject, content);
	response.set_content(R"({"success": true})", "application/json");
}

void Server::handleValidateUpdateViaSMS(const httplib::Request& request, httplib::Response& response)
{
	std::string	email;
	std::string phone;

	if (request.has_param("email"))
		email = request.get_param_value("email");
	if (request.has_param("phone"))
		phone = request.get_param_value("phone");

	std::string token = generateToken();
	subscriptionManager->setUpdateToken(email, token);

	std::string content = "Codul de validare pentru actualizarea datelor contului tau ParkPass este: " + token;
	sendSMS(phone, content);

	response.set_content(R"({"success": true})", "application/json");
}

void Server::handleResendValidateUpdateSMS(const httplib::Request& request, httplib::Response& response)
{
	std::string	email;
	std::string phone;

	if (request.has_param("email"))
		email = request.get_param_value("email");
	if (request.has_param("phone"))
		phone = request.get_param_value("phone");

	std::string token = subscriptionManager->getTempUpdatedAccountToken(email);
	std::string content = "Codul de validare pentru actualizarea datelor contului tau ParkPass este: " + token;
	sendSMS(phone, content);

	response.set_content(R"({"success": true})", "application/json");
}

void Server::handleValidateUpdate(const httplib::Request& request, httplib::Response& response)
{
	std::string token;

	if (request.has_param("token"))
		token = request.get_param_value("token");

	std::string email = subscriptionManager->updateAccount(token);
	if (!email.empty())
	{
		response.set_content("{\"success\": true, \"email\": \"" + email + "\"}", "application/json");
	}
	else
		response.set_content(R"({"success": false})", "application/json");
}

void Server::handleContact(const httplib::Request& request, httplib::Response& response)
{
	std::string email;
	std::string subject;
	std::string contactMessage;

	if (request.has_param("email"))
		email = request.get_param_value("email");

	if (request.has_param("subject"))
		subject = request.get_param_value("subject");

	if (request.has_param("message"))
		contactMessage = request.get_param_value("message");

	Account* account = subscriptionManager->getAccountByEmail(email);

	std::string content;
	if (account != nullptr)
		content = "Dear " + account->getName() + ",\n\n";
	else
		content = "Dear customer,\n\n";

	content +=
		"Thank you for reaching out to us. This is an automated response to confirm that we have received your message.\n\n"
		"We will get back to you as soon as possible with a reply.\n\n"
		"Below is the message you submitted:\n\n"
		"Subject: " + subject + "\n\n"
		"Message:\n" + contactMessage + "\n\n"
		"If you have any further questions, feel free to contact us again.\n\n"
		"Best regards,\n"
		"The ParkPass Team";

	sendEmail(email, subject, content);
	response.set_content(R"({"success": true})", "application/json");
}

void Server::handleSubscribeNewsletter(const httplib::Request& request, httplib::Response& response)
{
	std::string email;

	if (request.has_param("email"))
		email = request.get_param_value("email");

	if (!subscriptionManager->subscribeNewsletter(email))
	{
		response.set_content(R"({"success": false})", "application/json");
		return;
	}

	std::string subject = "Abonare newsletter";
	std::string content =
		"Salutare,\n\n"
		"Iti multumim ca te-ai abonat la newsletter-ul nostru! Suntem foarte bucurosi ca ai ales sa ramai la curent cu cele mai noi oferte, evenimente si noutati de la ParkPass.\n\n"
		"Vei primi periodic informatii utile despre serviciile noastre, dar si oferte speciale, pentru a-ti face parcarea cat mai simpla si rapida.\n\n"
		"Daca ai intrebari sau sugestii, nu ezita sa ne contactezi. Suntem aici pentru tine!\n\n"
		"Iti multumim inca o data pentru incredere si te asteptam sa descoperi mai multe la ParkPass.\n\n"
		"Cu drag,\n"
		"Echipa ParkPass";

	sendEmail(email, subject, content);

	response.set_content(R"({"success": true})", "application/json");
}

void Server::handleUnsubscribeNewsletter(const httplib::Request& request, httplib::Response& response)
{
	std::string email;

	if (request.has_param("email"))
		email = request.get_param_value("email");

	if (!subscriptionManager->unsubscribeNewsletter(email))
	{
		response.set_content(R"({"success": false})", "application/json");
		return;
	}

	std::string subject = "Dezabonare newsletter";
	std::string content =
		"Salutare,\n\n"
		"Acesta este un mesaj de confirmare a dezabonarii dumneavoastra de la newsletter-ul ParkPass. Regretam sa va vedem plecand, dar respectam decizia dumneavoastra.\n\n"
		"Incepand de acum, nu veti mai primi e-mailuri periodice cu informatii despre serviciile noastre, oferte speciale sau noutati. Totusi, daca va razganditi, va incurajam sa va reabonati prin intermediul site-ului nostru sau sa ne contactati pentru mai multe informatii.\n\n"
		"Daca aveti intrebari, sugestii sau feedback legat de experienta dumneavoastra, va rugam sa nu ezitati sa ne contactati. Suntem aici sa va ajutam!\n\n"
		"Va multumim pentru interesul acordat pana acum si speram sa avem ocazia de a va oferi din nou serviciile noastre in viitor.\n\n"
		"Cu respect,\n"
		"Echipa ParkPass";

	sendEmail(email, subject, content);

	response.set_content(R"({"success": true})", "application/json");
}

Server::~Server()
{
	if (thread.joinable())
	{
		server.stop();
		thread.join();
	}
}
