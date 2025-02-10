#pragma once

#include "httplib.h"
#include "subscriptionmanager.h"
#include "websocketserver.h"
#include "logger.h"

#include <thread>
#include <iostream>
#include <Poco/Net/SMTPClientSession.h>
#include <Poco/Net/MailMessage.h>
#include <Poco/Net/SSLManager.h>
#include <Poco/Net/ConsoleCertificateHandler.h>
#include <Poco/Net/AcceptCertificateHandler.h>
#include <Poco/Net/SecureStreamSocket.h>
#include <Poco/Net/SecureSMTPClientSession.h>
#include <Poco/Net/DNS.h>
#include <Poco/Net/MailMessage.h>
#include <Poco/AutoPtr.h>
#include <Poco/UUIDGenerator.h>
#include <Poco/DigestEngine.h>
#include <Poco/SHA1Engine.h>
#include <Poco/Random.h>
#include <Poco/RandomStream.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>
#include <Poco/JSON/Object.h>

class HttpServer
{
public:
	HttpServer();

	~HttpServer();

private:
	std::string generateToken();

	void sendEmail(const std::string& email, const std::string& subject, const std::string& content);

	void sendSMS(const std::string& phone, const std::string& content);

	void post(const httplib::Request& request, httplib::Response& response);

	void createAccount(const httplib::Request& request, httplib::Response& response);

	void validateViaEmail(const httplib::Request& request, httplib::Response& response);

	void validateViaSMS(const httplib::Request& request, httplib::Response& response);

	void resendValidateSMS(const httplib::Request& request, httplib::Response& response);

	void validate(const httplib::Request& request, httplib::Response& response);

	void login(const httplib::Request& request, httplib::Response& response);

	void getAdmin(const httplib::Request& request, httplib::Response& response);

	void recoverPasswordViaEmail(const httplib::Request& request, httplib::Response& response);

	void recoverPasswordViaSMS(const httplib::Request& request, httplib::Response& response);

	void verifyResetPasswordToken(const httplib::Request& request, httplib::Response& response);

	void resetPassword(const httplib::Request& request, httplib::Response& response);

	void resendRecoverPassword(const httplib::Request& request, httplib::Response& response);

	void getSubscriptionVehicles(const httplib::Request& request, httplib::Response& response);

	void addSubscription(const httplib::Request& request, httplib::Response& response);

	void deleteSubscription(const httplib::Request& request, httplib::Response& response);

	void getVehicleHistory(const httplib::Request& request, httplib::Response& response);

	void addVehicle(const httplib::Request& request, httplib::Response& response);

	void deleteVehicle(const httplib::Request& request, httplib::Response& response);

	void updateAccountInformation(const httplib::Request& request, httplib::Response& response);

	void updateAccount(const httplib::Request& request, httplib::Response& response);

	void validateUpdateViaEmail(const httplib::Request& request, httplib::Response& response);

	void validateUpdateViaSMS(const httplib::Request& request, httplib::Response& response);

	void resendValidateUpdateSMS(const httplib::Request& request, httplib::Response& response);

	void validateUpdate(const httplib::Request& request, httplib::Response& response);

	void contact(const httplib::Request& request, httplib::Response& response);

	void subscribeNewsletter(const httplib::Request& request, httplib::Response& response);

	void unsubscribeNewsletter(const httplib::Request& request, httplib::Response& response);

private:
	std::thread thread;
	httplib::Server server;
	std::unique_ptr<WebSocketServer> webSocketServer;
	SubscriptionManager subscriptionManager;
	Logger logger;
	std::string key;
};
