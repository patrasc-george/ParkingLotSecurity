﻿#include "httpserver.h"
#include "qrcodedetection.h"

#include <nlohmann/json.hpp>

HttpServer::HttpServer() : logger(Logger::getInstance())
{
#ifdef _DEBUG
	key = "";
	siteUrl = "http://localhost:4200";
#else
	key = std::getenv("POSTGRES_PASSWORD");
	siteUrl = std::getenv("SITE_URL");
#endif

	thread = std::thread([this]()
		{
			try
			{
				boost::asio::io_context ioContext;
				unsigned short port = 9002;

				LOG_MESSAGE(INFO) << "WebSocket server started on port " + std::to_string(port) + "." << std::endl;

				webSocketServer = std::make_unique<WebSocketServer>(ioContext, port);
				webSocketServer->start();
			}
			catch (const std::exception& error)
			{
				std::cerr << "Error: " << error.what() << std::endl;
			}
		});

	server.Post("/api/endpoint", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->post(request, response);
		});

	server.Post("/api/createAccount", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->createAccount(request, response);
		});

	server.Post("/api/validateViaEmail", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->validateViaEmail(request, response);
		});

	server.Post("/api/validateViaSMS", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->validateViaSMS(request, response);
		});

	server.Post("/api/resendValidateSMS", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->resendValidateSMS(request, response);
		});

	server.Post("/api/validate", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->validate(request, response);
		});

	server.Post("/api/login", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->login(request, response);
		});

	server.Post("/api/recoverPasswordViaEmail", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->recoverPasswordViaEmail(request, response);
		});

	server.Post("/api/recoverPasswordViaSMS", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->recoverPasswordViaSMS(request, response);
		});

	server.Post("/api/resendRecoverPassword", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->resendRecoverPassword(request, response);
		});

	server.Post("/api/verifyResetPasswordToken", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->verifyResetPasswordToken(request, response);
		});

	server.Post("/api/resetPassword", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->resetPassword(request, response);
		});

	server.Post("/api/getSubscriptionVehicles", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->getSubscriptionVehicles(request, response);
		});

	server.Post("/api/addSubscription", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->addSubscription(request, response);
		});

	server.Post("/api/deleteSubscription", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->deleteSubscription(request, response);
		});

	server.Post("/api/getVehicleHistory", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->getVehicleHistory(request, response);
		});

	server.Post("/api/addVehicle", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->addVehicle(request, response);
		});

	server.Post("/api/deleteVehicle", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->deleteVehicle(request, response);
		});

	server.Post("/api/updateAccountInformation", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->updateAccountInformation(request, response);
		});

	server.Post("/api/updateAccount", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->updateAccount(request, response);
		});

	server.Post("/api/validateUpdateViaEmail", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->validateUpdateViaEmail(request, response);
		});

	server.Post("/api/validateUpdateViaSMS", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->validateUpdateViaSMS(request, response);
		});

	server.Post("/api/resendValidateUpdateSMS", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->resendValidateUpdateSMS(request, response);
		});

	server.Post("/api/validateUpdate", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->validateUpdate(request, response);
		});

	server.Post("/api/contact", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->contact(request, response);
		});

	server.Post("/api/subscribeNewsletter", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->subscribeNewsletter(request, response);
		});

	server.Post("/api/unsubscribeNewsletter", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->unsubscribeNewsletter(request, response);
		});

	server.Post("/api/getAdmin", [this](const httplib::Request& request, httplib::Response& response) {
		response.set_header("Access-Control-Allow-Origin", "*");
		response.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
		response.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->getAdmin(request, response);
		});

	LOG_MESSAGE(INFO) << "HTTP server started on port 8080." << std::endl;
	server.listen("0.0.0.0", 8080);
}

std::string HttpServer::generateToken()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(100000, 999999);
	std::string token = std::to_string(dis(gen));

	return token;
}

bool HttpServer::sendEmail(const std::string& email, const std::string& subject, const std::string& content)
{
	std::string pocoEmail(std::getenv("POCO_EMAIL"));
	std::string pocoPassword(std::getenv("POCO_PASSWORD"));

	if (pocoEmail.empty() || pocoPassword.empty())
	{
		LOG_MESSAGE(CRITICAL) << "Missing POCO email credentials." << std::endl;
		return false;
	}

	try
	{
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
	catch (const Poco::Exception& ex)
	{
		LOG_MESSAGE(CRITICAL) << "Failed to send email to: " << email << ". Exception: " << ex.displayText() << std::endl;
		return false;
	}

	return true;
}

bool HttpServer::sendSMS(const std::string& phone, const std::string& content)
{
	std::string twiloSid(std::getenv("TWILO_SID"));
	std::string twiloToken(std::getenv("TWILO_TOKEN"));
	std::string twiloPhone(std::getenv("TWILO_PHONE"));

	if (twiloSid.empty() || twiloToken.empty() || twiloPhone.empty())
	{
		LOG_MESSAGE(CRITICAL) << "Missing Twilio credentials (SID, Token, or Phone)." << std::endl;
		return false;
	}

	httplib::SSLClient client("api.twilio.com");
	client.set_basic_auth(twiloSid.c_str(), twiloToken.c_str());

	std::string url = "/2010-04-01/Accounts/" + twiloSid + "/Messages.json";

	httplib::Params params;
	params.emplace("To", "+40" + phone);
	params.emplace("From", twiloPhone);
	params.emplace("Body", content);

	auto res = client.Post(url.c_str(), params);

	if (!res || res->status != 200)
	{
		LOG_MESSAGE(CRITICAL) << "Failed to send SMS to: " << phone << ". Response status: " << (res ? std::to_string(res->status) : "-1") << std::endl;
		return false;
	}

	return true;
}

void HttpServer::post(const httplib::Request& request, httplib::Response& response)
{
	nlohmann::json responseJson;
	std::string licensePlate;
	std::string dateTime;
	std::string requestKey;

	if (request.has_param("key"))
		requestKey = request.get_param_value("key");

	if (requestKey != key)
	{
		LOG_MESSAGE(CRITICAL) << "Invalid API key received." << std::endl;
		responseJson = {
			{"success", false}
		};

		response.set_content(responseJson.dump(), "application/json");
		return;
	}

	if (request.has_param("licensePlate"))
	{
		auto data = request.get_param_value("licensePlate");

		std::transform(data.begin(), data.end(), data.begin(), [](unsigned char c) {
			return std::toupper(c);
			});

		if (!subscriptionManager.pay(data, licensePlate, dateTime))
		{
			responseJson = {
				{"success", false},
				{"message", "The vehicle was not found. Please upload the QR code."}
			};

			response.set_content(responseJson.dump(), "application/json");
			return;
		}
	}
	else if (request.has_file("qrCodeImage"))
	{
		auto data = request.get_file_value("qrCodeImage");

		std::vector<unsigned char> imageData(data.content.begin(), data.content.end());

		QRCode qr;
		std::string ticket = qr.decodeQR(imageData);

		if (!subscriptionManager.pay(ticket, licensePlate, dateTime, true))
		{
			responseJson = {
				{"success", false},
				{"message", "The vehicle was not found. Please upload the QR code again."}
			};

			response.set_content(responseJson.dump(), "application/json");
			return;
		}
	}

	if (!licensePlate.empty() && !dateTime.empty())
	{
		responseJson = {
			{"success", true},
			{"message", "The vehicle with license plate number " + licensePlate + " was found. The parking fee has been paid. The vehicle entered the parking lot on " + dateTime + "."},
			{"licensePlate", licensePlate},
			{"dateTime", dateTime}
		};

		response.set_content(responseJson.dump(), "application/json");
	}
	else
	{
		LOG_MESSAGE(CRITICAL) << "Missing required data." << std::endl;
		responseJson = {
			{"success", false}
		};

		response.set_content(responseJson.dump(), "application/json");
	}
}

void HttpServer::createAccount(const httplib::Request& request, httplib::Response& response)
{
	nlohmann::json responseJson;
	std::string name;
	std::string lastName;
	std::string email;
	std::string password;
	std::string phone;
	std::string captchaToken;

	if (!request.has_param("key") || request.get_param_value("key") != key)
	{
		LOG_MESSAGE(CRITICAL) << "Invalid API key received." << std::endl;
		responseJson = {
			{"success", false}
		};
		response.set_content(responseJson.dump(), "application/json");
		return;
	}

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
	std::string secretKey(std::getenv("RECAPTCHA_KEY"));
	std::string payload = "secret=" + secretKey + "&response=" + captchaToken;

	auto recaptchaResponse = recaptchaClient.Post("/recaptcha/api/siteverify", payload, "application/x-www-form-urlencoded");
	if (!recaptchaResponse || recaptchaResponse->status != 200)
	{
		std::string statusMessage = (recaptchaResponse != nullptr) ? std::to_string(recaptchaResponse->status) : "null response";
		LOG_MESSAGE(CRITICAL) << "Failed to verify captcha with status: " << statusMessage << std::endl;
		responseJson = {
			{"success", false}
		};

		response.set_content(responseJson.dump(), "application/json");
		return;
	}

	auto recaptchaBody = recaptchaResponse->body;
	Poco::JSON::Parser parser;
	auto recaptchaJson = parser.parse(recaptchaBody).extract<Poco::JSON::Object::Ptr>();
	bool captchaSuccess = recaptchaJson->getValue<bool>("success");

	if (!captchaSuccess)
	{
		LOG_MESSAGE(CRITICAL) << "Captcha verification failed." << std::endl;
		responseJson = {
			{"success", false}
		};

		response.set_content(responseJson.dump(), "application/json");
		return;
	}

	if (subscriptionManager.getAccountByEmail(email) != nullptr)
	{
		responseJson = {
			{"success", false},
			{"message", "An account with this email address already exists."}
		};
		response.set_content(responseJson.dump(), "application/json");
		return;
	}

	if (subscriptionManager.getAccountByPhone(phone) != nullptr)
	{
		responseJson = {
			{"success", false},
			{"message", "An account with this phone number already exists."}
		};
		response.set_content(responseJson.dump(), "application/json");
		return;
	}

	subscriptionManager.addTempAccount(name, lastName, email, password, phone);

	responseJson = {
		{"success", true}
	};
	response.set_content(responseJson.dump(), "application/json");
}

void HttpServer::validateViaEmail(const httplib::Request& request, httplib::Response& response)
{
	nlohmann::json responseJson;
	std::string name;
	std::string email;

	if (!request.has_param("key") || request.get_param_value("key") != key)
	{
		LOG_MESSAGE(CRITICAL) << "Invalid API key received." << std::endl;
		responseJson = {
			{"success", false}
		};
		response.set_content(responseJson.dump(), "application/json");
		return;
	}

	if (request.has_param("name"))
		name = request.get_param_value("name");
	if (request.has_param("email"))
		email = request.get_param_value("email");

	std::string token = generateToken();
	subscriptionManager.setToken(email, token);

	std::string subject = "Validare cont";
	std::string content =
		"Stimata/Stimate, " + name + ",\n\n"
		"Iti multumim ca ai ales ParkPass ca solutie de plata pentru parcarea ta!\n\n"
		"Pentru a finaliza configurarea contului tau, te rugam sa confirmi adresa de e-mail asociata acestuia.\n\n"
		"Pentru a continua, acceseaza linkul de mai jos:\n"
		+ siteUrl + "/validate?token=" + token + "\n\n"
		"Daca nu ai creat un cont ParkPass, te rugam sa ignori acest e-mail.\n\n"
		"Iti multumim si bun venit la ParkPass!\n\n"
		"Cu respect,\n"
		"Echipa ParkPass";

	if (!sendEmail(email, subject, content))
	{
		LOG_MESSAGE(CRITICAL) << "Failed to send validation email to: " << email << std::endl;
		responseJson = {
			{"success", false},
			{"message", "Failed to send validation email. Please try again later."}
		};
		response.set_content(responseJson.dump(), "application/json");
		return;
	}

	responseJson = {
		{"success", true}
	};
	response.set_content(responseJson.dump(), "application/json");
}

void HttpServer::validateViaSMS(const httplib::Request& request, httplib::Response& response)
{
	nlohmann::json responseJson;
	std::string email;
	std::string phone;

	if (!request.has_param("key") || request.get_param_value("key") != key)
	{
		LOG_MESSAGE(CRITICAL) << "Invalid API key received." << std::endl;
		responseJson = {
			{"success", false}
		};
		response.set_content(responseJson.dump(), "application/json");
		return;
	}

	if (request.has_param("email"))
		email = request.get_param_value("email");
	if (request.has_param("phone"))
		phone = request.get_param_value("phone");

	std::string token = generateToken();
	subscriptionManager.setToken(email, token);

	std::string content = "Codul de validare pentru contul tau ParkPass este: " + token;
	if (!sendSMS(phone, content))
	{
		LOG_MESSAGE(CRITICAL) << "Failed to send validation SMS to: " << phone << std::endl;
		responseJson = {
			{"success", false},
			{"message", "Failed to send validation SMS. Please try again later."}
		};
		response.set_content(responseJson.dump(), "application/json");
		return;
	}

	responseJson = {
		{"success", true}
	};
	response.set_content(responseJson.dump(), "application/json");
}

void HttpServer::resendValidateSMS(const httplib::Request& request, httplib::Response& response)
{
	nlohmann::json responseJson;
	std::string email;
	std::string phone;

	if (!request.has_param("key") || request.get_param_value("key") != key)
	{
		LOG_MESSAGE(CRITICAL) << "Invalid API key received." << std::endl;
		responseJson = {
			{"success", false}
		};
		response.set_content(responseJson.dump(), "application/json");
		return;
	}

	if (request.has_param("email"))
		email = request.get_param_value("email");
	if (request.has_param("phone"))
		phone = request.get_param_value("phone");

	std::string token = subscriptionManager.getTempAccountToken(email);

	std::string content = "Codul de validare pentru contul tau ParkPass este: " + token;
	if (!sendSMS(phone, content))
	{
		LOG_MESSAGE(CRITICAL) << "Failed to resend validation SMS to: " << phone << std::endl;
		responseJson = {
			{"success", false},
			{"message", "Failed to resend validation SMS. Please try again later."}
		};
		response.set_content(responseJson.dump(), "application/json");
		return;
	}

	responseJson = {
		{"success", true}
	};
	response.set_content(responseJson.dump(), "application/json");
}

void HttpServer::validate(const httplib::Request& request, httplib::Response& response)
{
	nlohmann::json responseJson;
	std::string token;

	if (!request.has_param("key") || request.get_param_value("key") != key)
	{
		LOG_MESSAGE(CRITICAL) << "Invalid API key received." << std::endl;
		responseJson = {
			{"success", false}
		};
		response.set_content(responseJson.dump(), "application/json");
		return;
	}

	if (request.has_param("token"))
		token = request.get_param_value("token");

	std::string email = subscriptionManager.addAccount(token);
	if (!email.empty())
	{
		responseJson = {
			{"success", true},
			{"email", email}
		};
		response.set_content(responseJson.dump(), "application/json");
	}
	else
	{
		LOG_MESSAGE(CRITICAL) << "Account validation failed for token: " << token << std::endl;
		responseJson = {
			{"success", false}
		};
		response.set_content(responseJson.dump(), "application/json");
	}
}

void HttpServer::login(const httplib::Request& request, httplib::Response& response)
{
	nlohmann::json responseJson;
	std::string input;
	std::string password;
	bool fromRedirect = false;

	if (!request.has_param("key") || request.get_param_value("key") != key)
	{
		LOG_MESSAGE(CRITICAL) << "Invalid API key received in login." << std::endl;
		responseJson = {
			{"success", false}
		};
		response.set_content(responseJson.dump(), "application/json");
		return;
	}

	if (request.has_param("input"))
		input = request.get_param_value("input");

	if (request.has_param("password"))
		password = request.get_param_value("password");

	if (request.has_param("fromRedirect"))
		if (request.get_param_value("fromRedirect") == "true")
			fromRedirect = true;

	if (input == "admin")
	{
		if (subscriptionManager.verifyAdminCredentials(password))
		{
			responseJson = {
				{"success", true}
			};
			response.set_content(responseJson.dump(), "application/json");
		}
		else
		{
			responseJson = {
				{"success", false}
			};
			response.set_content(responseJson.dump(), "application/json");
		}
		return;
	}

	Account* account;
	if (fromRedirect)
		account = subscriptionManager.getAccountByEmail(input);
	else
	{
		account = subscriptionManager.verifyCredentials(input, password);
		if (account == nullptr)
		{
			responseJson = {
				{"success", false}
			};
			response.set_content(responseJson.dump(), "application/json");
			return;
		}
	}

	std::string name = account->getName();
	std::string lastName = account->getLastName();
	std::string email = account->getEmail();
	std::string phone = account->getPhone();

	std::vector<Subscription>* subscriptions = subscriptionManager.getSubscriptions(*account);
	nlohmann::json subscriptionsJson = nlohmann::json::array();

	for (const auto& subscription : *subscriptions)
		subscriptionsJson.push_back(nlohmann::json::array({ subscription.getName() }));

	responseJson = {
		{"success", true},
		{"name", name},
		{"lastName", lastName},
		{"email", email},
		{"phone", phone},
		{"subscriptionsTable", subscriptionsJson}
	};

	response.set_content(responseJson.dump(), "application/json");
}

void HttpServer::getAdmin(const httplib::Request& request, httplib::Response& response)
{
	nlohmann::json responseJson;

	if (!request.has_param("key") || request.get_param_value("key") != key)
	{
		LOG_MESSAGE(CRITICAL) << "Invalid API key received." << std::endl;

		responseJson = {
			{"success", false}
		};

		response.set_content(responseJson.dump(), "application/json");
		return;
	}

	std::unordered_set<std::string> emails = subscriptionManager.getEmails();
	nlohmann::json emailsJson = nlohmann::json::array();
	for (const auto& email : emails)
		emailsJson.push_back(email);

	responseJson = {
		{"success", true},
		{"emailsTable", emailsJson}
	};

	response.set_content(responseJson.dump(), "application/json");
}

void HttpServer::recoverPasswordViaEmail(const httplib::Request& request, httplib::Response& response)
{
	nlohmann::json responseJson;
	std::string email;

	if (!request.has_param("key") || request.get_param_value("key") != key)
	{
		LOG_MESSAGE(CRITICAL) << "Invalid API key received." << std::endl;

		responseJson = {
			{"success", false}
		};
		response.set_content(responseJson.dump(), "application/json");
		return;
	}

	if (request.has_param("email"))
		email = request.get_param_value("email");

	Account* account = subscriptionManager.getAccountByEmail(email);

	if (account == nullptr)
	{
		responseJson = {
			{"success", false}
		};
		response.set_content(responseJson.dump(), "application/json");
		return;
	}

	std::string token = generateToken();
	subscriptionManager.addTempRecoveredPasswords(email, token);

	std::string subject = "Resetare parola";
	std::string content =
		"Stimata/Stimate, " + account->getName() + ",\n\n"
		"Am primit o solicitare pentru resetarea parolei contului tau ParkPass.\n\n"
		"Pentru a continua te rugam sa accesezi linkul de mai jos:\n"
		+ siteUrl + "/reset-password?token=" + token + "\n\n"
		"Daca nu ai solicitat resetarea parolei, te rugam sa nu accesezi linkul. Pentru siguranta ta, iti recomandam sa verifici activitatea contului pentru a te asigura ca este securizat.\n\n"
		"Iti multumim ca folosesti ParkPass. Suntem aici sa te ajutam cu orice intrebari sau nelamuriri.\n\n"
		"Cu respect,\n"
		"Echipa ParkPass";

	sendEmail(email, subject, content);

	responseJson = {
		{"success", true}
	};
	response.set_content(responseJson.dump(), "application/json");
}

void HttpServer::recoverPasswordViaSMS(const httplib::Request& request, httplib::Response& response)
{
	nlohmann::json responseJson;
	std::string phone;

	if (!request.has_param("key") || request.get_param_value("key") != key)
	{
		LOG_MESSAGE(CRITICAL) << "Invalid API key received." << std::endl;

		responseJson = {
			{"success", false}
		};
		response.set_content(responseJson.dump(), "application/json");
		return;
	}

	if (request.has_param("phone"))
		phone = request.get_param_value("phone");

	Account* account = subscriptionManager.getAccountByPhone(phone);

	if (account == nullptr)
	{
		responseJson = {
			{"success", false}
		};
		response.set_content(responseJson.dump(), "application/json");
		return;
	}

	std::string token = generateToken();
	subscriptionManager.addTempRecoveredPasswords(account->getEmail(), token);

	std::string content = "Codul de resetare a parolei pentru contul tau ParkPass este: " + token;
	sendSMS(phone, content);

	responseJson = {
		{"success", true}
	};
	response.set_content(responseJson.dump(), "application/json");
}

void HttpServer::resendRecoverPassword(const httplib::Request& request, httplib::Response& response)
{
	nlohmann::json responseJson;
	std::string phone;

	if (!request.has_param("key") || request.get_param_value("key") != key)
	{
		LOG_MESSAGE(CRITICAL) << "Invalid API key received." << std::endl;

		responseJson = {
			{"success", false}
		};
		response.set_content(responseJson.dump(), "application/json");
		return;
	}

	if (request.has_param("phone"))
		phone = request.get_param_value("phone");

	Account* account = subscriptionManager.getAccountByPhone(phone);

	if (account == nullptr)
	{

		responseJson = {
			{"success", false}
		};
		response.set_content(responseJson.dump(), "application/json");
		return;
	}

	std::string token = subscriptionManager.getTempRecoveredPasswordToken(account->getEmail());

	std::string content = "Codul de resetare a parolei pentru contul tau ParkPass este: " + token;
	sendSMS(phone, content);

	responseJson = { {"success", true} };
	response.set_content(responseJson.dump(), "application/json");
}

void HttpServer::verifyResetPasswordToken(const httplib::Request& request, httplib::Response& response)
{
	nlohmann::json responseJson;
	std::string token;

	if (!request.has_param("key") || request.get_param_value("key") != key)
	{
		LOG_MESSAGE(CRITICAL) << "Invalid API key received." << std::endl;

		responseJson = {
			{"success", false}
		};

		response.set_content(responseJson.dump(), "application/json");
		return;
	}

	if (request.has_param("token"))
		token = request.get_param_value("token");

	std::string email = subscriptionManager.verifyTempRecoveredPasswordsToken(token);

	if (!email.empty())
	{
		responseJson = {
			{"success", true},
			{"email", email}
		};

		response.set_content(responseJson.dump(), "application/json");
	}
	else
	{
		LOG_MESSAGE(CRITICAL) << "Invalid token received." << std::endl;

		responseJson = {
			{"success", false}
		};

		response.set_content(responseJson.dump(), "application/json");
	}
}

void HttpServer::resetPassword(const httplib::Request& request, httplib::Response& response)
{
	nlohmann::json responseJson;
	std::string email;
	std::string newPassword;

	if (!request.has_param("key") || request.get_param_value("key") != key)
	{
		LOG_MESSAGE(CRITICAL) << "Invalid API key received." << std::endl;

		responseJson = {
			{"success", false}
		};

		response.set_content(responseJson.dump(), "application/json");
		return;
	}

	if (request.has_param("email"))
		email = request.get_param_value("email");

	if (request.has_param("newPassword"))
		newPassword = request.get_param_value("newPassword");

	if (subscriptionManager.updateAccountPassword(email, newPassword))
	{
		responseJson = {
			{"success", true}
		};

		response.set_content(responseJson.dump(), "application/json");
	}
	else
	{
		responseJson = {
			{"success", false}
		};

		response.set_content(responseJson.dump(), "application/json");
	}
}

void HttpServer::getSubscriptionVehicles(const httplib::Request& request, httplib::Response& response)
{
	nlohmann::json responseJson;
	std::string email;
	std::string subscriptionName;

	if (!request.has_param("key") || request.get_param_value("key") != key)
	{
		LOG_MESSAGE(CRITICAL) << "Invalid API key received." << std::endl;

		responseJson = {
			{"success", false}
		};

		response.set_content(responseJson.dump(), "application/json");
		return;
	}

	if (request.has_param("email"))
		email = request.get_param_value("email");

	if (request.has_param("subscriptionName"))
		subscriptionName = request.get_param_value("subscriptionName");

	Account* account = subscriptionManager.getAccountByEmail(email);
	Subscription* subscription = subscriptionManager.getSubscription(*account, subscriptionName);
	std::vector<std::vector<std::string>> vehiclesTable = subscriptionManager.getSubscriptionVehicles(*subscription);

	nlohmann::json vehiclesJson = nlohmann::json::array();
	for (const auto& vehicle : vehiclesTable)
	{
		nlohmann::json vehicleArray = nlohmann::json::array();

		for (const auto& data : vehicle)
			vehicleArray.push_back(data);

		vehiclesJson.push_back(vehicleArray);
	}

	responseJson = {
		{"success", true},
		{"vehiclesTable", vehiclesJson}
	};

	response.set_content(responseJson.dump(), "application/json");
}

void HttpServer::addSubscription(const httplib::Request& request, httplib::Response& response)
{
	nlohmann::json responseJson;
	std::string email;
	std::string subscriptionName;

	if (!request.has_param("key") || request.get_param_value("key") != key)
	{
		LOG_MESSAGE(CRITICAL) << "Invalid API key received." << std::endl;

		responseJson = {
			{"success", false}
		};

		response.set_content(responseJson.dump(), "application/json");
		return;
	}

	if (request.has_param("email"))
		email = request.get_param_value("email");

	if (request.has_param("subscriptionName"))
		subscriptionName = request.get_param_value("subscriptionName");

	Account* account = subscriptionManager.getAccountByEmail(email);

	if (subscriptionManager.addSubscription(*account, subscriptionName))
	{
		responseJson = {
			{"success", true}
		};

		response.set_content(responseJson.dump(), "application/json");
	}
	else
	{
		responseJson = {
			{"success", false}
		};

		response.set_content(responseJson.dump(), "application/json");
	}
}

void HttpServer::deleteSubscription(const httplib::Request& request, httplib::Response& response)
{
	nlohmann::json responseJson;
	std::string email;
	std::string subscriptionName;

	if (!request.has_param("key") || request.get_param_value("key") != key)
	{
		LOG_MESSAGE(CRITICAL) << "Invalid API key received." << std::endl;

		responseJson = {
			{"success", false}
		};

		response.set_content(responseJson.dump(), "application/json");
		return;
	}

	if (request.has_param("email"))
		email = request.get_param_value("email");

	if (request.has_param("subscriptionName"))
		subscriptionName = request.get_param_value("subscriptionName");

	Account* account = subscriptionManager.getAccountByEmail(email);

	if (subscriptionManager.deleteSubscription(*account, subscriptionName))
	{
		responseJson = {
			{"success", true}
		};

		response.set_content(responseJson.dump(), "application/json");
	}
	else
	{
		responseJson = {
			{"success", false}
		};

		response.set_content(responseJson.dump(), "application/json");
	}
}

void HttpServer::getVehicleHistory(const httplib::Request& request, httplib::Response& response)
{
	nlohmann::json responseJson;
	std::string licensePlate;

	if (!request.has_param("key") || request.get_param_value("key") != key)
	{
		LOG_MESSAGE(CRITICAL) << "Invalid API key received." << std::endl;

		responseJson = {
			{"success", false}
		};

		response.set_content(responseJson.dump(), "application/json");
		return;
	}

	if (request.has_param("licensePlate"))
		licensePlate = request.get_param_value("licensePlate");

	std::string totalTimeParked;
	int payment;
	std::vector<std::vector<std::string>> history = subscriptionManager.getVehicleHistory(licensePlate, totalTimeParked, payment);

	nlohmann::json historyJson = nlohmann::json::array();

	for (const auto& data : history)
	{
		nlohmann::json historyArray = nlohmann::json::array();

		for (const auto& value : data)
			historyArray.push_back(value);

		historyJson.push_back(historyArray);
	}

	responseJson = {
		{"success", true},
		{"history", historyJson},
		{"totalTimeParked", totalTimeParked},
		{"payment", std::to_string(payment)}
	};

	response.set_content(responseJson.dump(), "application/json");
}

void HttpServer::addVehicle(const httplib::Request& request, httplib::Response& response)
{
	nlohmann::json responseJson;
	std::string email;
	std::string subscriptionName;
	std::string licensePlate;

	if (!request.has_param("key") || request.get_param_value("key") != key)
	{
		LOG_MESSAGE(CRITICAL) << "Invalid API key received." << std::endl;

		responseJson = {
			{"success", false}
		};

		response.set_content(responseJson.dump(), "application/json");
		return;
	}

	if (request.has_param("email"))
		email = request.get_param_value("email");

	if (request.has_param("subscriptionName"))
		subscriptionName = request.get_param_value("subscriptionName");

	if (request.has_param("licensePlate"))
		licensePlate = request.get_param_value("licensePlate");

	Account* account = subscriptionManager.getAccountByEmail(email);
	Subscription* subscription = subscriptionManager.getSubscription(*account, subscriptionName);
	std::transform(licensePlate.begin(), licensePlate.end(), licensePlate.begin(), ::toupper);

	if (subscriptionManager.addVehicle(*account, *subscription, licensePlate))
	{
		std::vector<std::vector<std::string>> vehiclesTable = subscriptionManager.getSubscriptionVehicles(*subscription);
		nlohmann::json vehiclesJson = nlohmann::json::array();

		if (!vehiclesTable.empty())
		{
			nlohmann::json lastVehicle = nlohmann::json::array();

			for (const auto& item : vehiclesTable.back())
				lastVehicle.push_back(item);

			vehiclesJson.push_back(lastVehicle);
		}

		responseJson = {
			{"success", true},
			{"vehiclesTable", vehiclesJson}
		};

		response.set_content(responseJson.dump(), "application/json");
	}
	else
	{
		responseJson = {
			{"success", false}
		};

		response.set_content(responseJson.dump(), "application/json");
	}
}

void HttpServer::deleteVehicle(const httplib::Request& request, httplib::Response& response)
{
	nlohmann::json responseJson;
	std::string email;
	std::string subscriptionName;
	std::string licensePlate;

	if (!request.has_param("key") || request.get_param_value("key") != key)
	{
		LOG_MESSAGE(CRITICAL) << "Invalid API key received." << std::endl;

		responseJson = {
			{"success", false}
		};

		response.set_content(responseJson.dump(), "application/json");
		return;
	}

	if (request.has_param("email"))
		email = request.get_param_value("email");

	if (request.has_param("subscriptionName"))
		subscriptionName = request.get_param_value("subscriptionName");

	if (request.has_param("licensePlate"))
		licensePlate = request.get_param_value("licensePlate");

	Account* account = subscriptionManager.getAccountByEmail(email);
	Subscription* subscription = subscriptionManager.getSubscription(*account, subscriptionName);
	std::transform(licensePlate.begin(), licensePlate.end(), licensePlate.begin(), ::toupper);

	if (subscription && subscriptionManager.deleteVehicle(*account, *subscription, licensePlate))
	{
		responseJson = {
			{"success", true}
		};

		response.set_content(responseJson.dump(), "application/json");
	}
	else
	{
		responseJson = {
			{"success", false}
		};

		response.set_content(responseJson.dump(), "application/json");

		LOG_MESSAGE(CRITICAL) << "Failed to delete vehicle with license plate " << licensePlate << " for subscription: " << subscriptionName << std::endl;
	}
}

void HttpServer::updateAccountInformation(const httplib::Request& request, httplib::Response& response)
{
	nlohmann::json responseJson;
	std::string newName;
	std::string newLastName;
	std::string newPhone;
	std::string email;

	if (!request.has_param("key") || request.get_param_value("key") != key)
	{
		LOG_MESSAGE(CRITICAL) << "Invalid API key received." << std::endl;

		responseJson = {
			{"success", false}
		};

		response.set_content(responseJson.dump(), "application/json");
		return;
	}

	if (request.has_param("newName"))
		newName = request.get_param_value("newName");

	if (request.has_param("newLastName"))
		newLastName = request.get_param_value("newLastName");

	if (request.has_param("newPhone"))
		newPhone = request.get_param_value("newPhone");

	if (request.has_param("email"))
		email = request.get_param_value("email");

	if (!newPhone.empty())
		if (subscriptionManager.getAccountByPhone(newPhone) != nullptr)
		{
			responseJson = {
				{"success", false}
			};

			response.set_content(responseJson.dump(), "application/json");

			return;
		}

	if (subscriptionManager.updateAccountInformation(email, newName, newLastName, newPhone))
	{
		responseJson = {
			{"success", true}
		};

		response.set_content(responseJson.dump(), "application/json");
	}
	else
	{
		responseJson = {
			{"success", false}
		};

		response.set_content(responseJson.dump(), "application/json");

		LOG_MESSAGE(CRITICAL) << "Failed to update account information for email: " << email << std::endl;
	}
}

void HttpServer::updateAccount(const httplib::Request& request, httplib::Response& response)
{
	nlohmann::json responseJson;
	std::string newEmail;
	std::string newPassword;
	std::string email;
	std::string admin;

	if (!request.has_param("key") || request.get_param_value("key") != key)
	{
		LOG_MESSAGE(CRITICAL) << "Invalid API key received." << std::endl;

		responseJson = {
			{"success", false}
		};

		response.set_content(responseJson.dump(), "application/json");
		return;
	}

	if (request.has_param("newEmail"))
		newEmail = request.get_param_value("newEmail");

	if (request.has_param("newPassword"))
		newPassword = request.get_param_value("newPassword");

	if (request.has_param("email"))
		email = request.get_param_value("email");

	if (request.has_param("admin"))
		admin = request.get_param_value("admin");

	Account* account = subscriptionManager.getAccountByEmail(email);

	if (account == nullptr)
	{

		responseJson = {
			{"success", false}
		};

		response.set_content(responseJson.dump(), "application/json");
		return;
	}

	if (!subscriptionManager.addTempUpdatedAccount(email, newEmail, newPassword))
	{
		responseJson = {
			{"success", false}
		};

		response.set_content(responseJson.dump(), "application/json");
		return;
	}

	if (admin == "true")
	{
		std::string token = generateToken();
		subscriptionManager.setUpdateToken(email, token);

		std::string accountEmail = subscriptionManager.updateAccount(token);
	}

	responseJson = {
		{"success", true}
	};

	response.set_content(responseJson.dump(), "application/json");
}

void HttpServer::validateUpdateViaEmail(const httplib::Request& request, httplib::Response& response)
{
	nlohmann::json responseJson;
	std::string name;
	std::string email;

	if (!request.has_param("key") || request.get_param_value("key") != key)
	{
		LOG_MESSAGE(CRITICAL) << "Invalid API key received." << std::endl;

		responseJson = {
			{"success", false}
		};

		response.set_content(responseJson.dump(), "application/json");
		return;
	}

	if (request.has_param("name"))
		name = request.get_param_value("name");
	if (request.has_param("email"))
		email = request.get_param_value("email");

	std::string token = generateToken();
	subscriptionManager.setUpdateToken(email, token);

	std::string subject = "Actualizare informatii cont";
	std::string content =
		"Stimata/Stimate, " + name + ",\n\n"
		"Am primit o solicitare pentru a actualiza informatiile contului tau ParkPass.\n\n"
		"Pentru a confirma aceste modificari, te rugam sa accesezi linkul de mai jos:\n"
		+ siteUrl + "/validate-update?token=" + token + "\n\n"
		"Daca nu ai solicitat nicio modificare a informatiilor contului, te rugam sa ignori acest e-mail. "
		"Din motive de siguranta, iti recomandam sa verifici periodic activitatea contului tau.\n\n"
		"Iti multumim ca folosesti ParkPass. Suntem aici sa te ajutam cu orice intrebari sau nelamuriri.\n\n"
		"Cu respect,\n"
		"Echipa ParkPass";

	sendEmail(email, subject, content);

	responseJson = {
		{"success", true}
	};

	response.set_content(responseJson.dump(), "application/json");
}

void HttpServer::validateUpdateViaSMS(const httplib::Request& request, httplib::Response& response)
{
	nlohmann::json responseJson;
	std::string email;
	std::string phone;

	if (!request.has_param("key") || request.get_param_value("key") != key)
	{
		LOG_MESSAGE(CRITICAL) << "Invalid API key received." << std::endl;

		responseJson = {
			{"success", false}
		};

		response.set_content(responseJson.dump(), "application/json");
		return;
	}

	if (request.has_param("email"))
		email = request.get_param_value("email");
	if (request.has_param("phone"))
		phone = request.get_param_value("phone");

	std::string token = generateToken();
	subscriptionManager.setUpdateToken(email, token);

	std::string content = "Codul de validare pentru actualizarea datelor contului tau ParkPass este: " + token;
	sendSMS(phone, content);

	responseJson = {
		{"success", true}
	};

	response.set_content(responseJson.dump(), "application/json");
}

void HttpServer::resendValidateUpdateSMS(const httplib::Request& request, httplib::Response& response)
{
	nlohmann::json responseJson;
	std::string email;
	std::string phone;

	if (!request.has_param("key") || request.get_param_value("key") != key)
	{
		LOG_MESSAGE(CRITICAL) << "Invalid API key received." << std::endl;

		responseJson = {
			{"success", false}
		};

		response.set_content(responseJson.dump(), "application/json");
		return;
	}

	if (request.has_param("email"))
		email = request.get_param_value("email");
	if (request.has_param("phone"))
		phone = request.get_param_value("phone");

	std::string token = subscriptionManager.getTempUpdatedAccountToken(email);
	if (token.empty())
	{
		LOG_MESSAGE(CRITICAL) << "No update token found for email: " << email << std::endl;

		responseJson = {
			{"success", false}
		};

		response.set_content(responseJson.dump(), "application/json");
		return;
	}

	std::string content = "Codul de validare pentru actualizarea datelor contului tau ParkPass este: " + token;
	sendSMS(phone, content);

	responseJson = {
		{"success", true}
	};

	response.set_content(responseJson.dump(), "application/json");
}
void HttpServer::validateUpdate(const httplib::Request& request, httplib::Response& response)
{
	nlohmann::json responseJson;
	std::string token;

	if (!request.has_param("key") || request.get_param_value("key") != key)
	{
		LOG_MESSAGE(CRITICAL) << "Invalid API key received." << std::endl;

		responseJson = {
			{"success", false}
		};

		response.set_content(responseJson.dump(), "application/json");
		return;
	}

	if (request.has_param("token"))
		token = request.get_param_value("token");

	std::string email = subscriptionManager.updateAccount(token);
	if (!email.empty())
	{
		responseJson = {
			{"success", true},
			{"email", email}
		};

		response.set_content(responseJson.dump(), "application/json");
	}
	else
	{
		LOG_MESSAGE(CRITICAL) << "No update token found for email: " << email << std::endl;

		responseJson = {
			{"success", false}
		};

		response.set_content(responseJson.dump(), "application/json");
	}
}

void HttpServer::contact(const httplib::Request& request, httplib::Response& response)
{
	nlohmann::json responseJson;
	std::string email;
	std::string subject;
	std::string contactMessage;

	if (!request.has_param("key") || request.get_param_value("key") != key)
	{
		LOG_MESSAGE(CRITICAL) << "Invalid API key received." << std::endl;

		responseJson = {
			{"success", false}
		};

		response.set_content(responseJson.dump(), "application/json");
		return;
	}

	if (request.has_param("email"))
		email = request.get_param_value("email");

	if (request.has_param("subject"))
		subject = request.get_param_value("subject");

	if (request.has_param("message"))
		contactMessage = request.get_param_value("message");

	Account* account = subscriptionManager.getAccountByEmail(email);

	std::string content;
	if (account != nullptr)
		content = "Stimata/Stimate, " + account->getName() + ",\n\n";
	else
		content = "Stimata/Stimate client,\n\n";

	content +=
		"Va multumim ca ne-ati contactat. Acesta este un raspuns automat pentru a confirma ca am primit mesajul dumneavoastra.\n\n"
		"Va vom raspunde cat mai curand posibil.\n\n"
		"Mai jos este mesajul pe care l-ati trimis:\n\n"
		"Subiect: " + subject + "\n\n"
		"Mesaj:\n" + contactMessage + "\n\n"
		"Daca aveti intrebari suplimentare, nu ezitati sa ne contactati din nou.\n\n"
		"Cu respect,\n"
		"Echipa ParkPass";

	sendEmail(email, subject, content);
	responseJson = {
		{"success", true}
	};

	response.set_content(responseJson.dump(), "application/json");
}

void HttpServer::subscribeNewsletter(const httplib::Request& request, httplib::Response& response)
{
	nlohmann::json responseJson;
	std::string email;

	if (!request.has_param("key") || request.get_param_value("key") != key)
	{
		LOG_MESSAGE(CRITICAL) << "Invalid API key received." << std::endl;

		responseJson = {
			{"success", false}
		};

		response.set_content(responseJson.dump(), "application/json");
		return;
	}

	if (request.has_param("email"))
		email = request.get_param_value("email");

	if (!subscriptionManager.subscribeNewsletter(email))
	{
		responseJson = {
			{"success", false}
		};

		response.set_content(responseJson.dump(), "application/json");
		return;
	}

	std::string subject = "Abonare newsletter";
	std::string content =
		"Salutare,\n\n"
		"Iti multumim ca te-ai abonat la newsletter-ul nostru! Suntem foarte bucurosi ca ai ales sa ramai la curent cu cele mai noi oferte, evenimente si noutati de la ParkPass.\n\n"
		"Vei primi periodic informatii utile despre serviciile noastre, dar si oferte speciale, pentru a-ti face parcarea cat mai simpla si rapida.\n\n"
		"Daca ai intrebari sau sugestii, nu ezita sa ne contactezi. Suntem aici pentru tine!\n\n"
		"Iti multumim inca o data pentru incredere si te asteptam sa descoperi mai multe la ParkPass.\n\n"
		"Cu respect,\n"
		"Echipa ParkPass";

	sendEmail(email, subject, content);

	responseJson = {
		{"success", true}
	};

	response.set_content(responseJson.dump(), "application/json");
}

void HttpServer::unsubscribeNewsletter(const httplib::Request& request, httplib::Response& response)
{
	nlohmann::json responseJson;
	std::string email;

	if (!request.has_param("key") || request.get_param_value("key") != key)
	{
		LOG_MESSAGE(CRITICAL) << "Invalid API key received." << std::endl;

		responseJson = {
			{"success", false}
		};

		response.set_content(responseJson.dump(), "application/json");
		return;
	}

	if (request.has_param("email"))
		email = request.get_param_value("email");

	if (!subscriptionManager.unsubscribeNewsletter(email))
	{
		responseJson = {
			{"success", false}
		};

		response.set_content(responseJson.dump(), "application/json");
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

	responseJson = {
		{"success", true}
	};

	response.set_content(responseJson.dump(), "application/json");
}

HttpServer::~HttpServer()
{
	if (thread.joinable())
	{
		server.stop();
		thread.join();
	}
}
