#pragma once

#include "httplib.h"
#include "subscriptionmanager.h"
#include "websocketserver.h"

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

	void handlePost(const httplib::Request& request, httplib::Response& response);

	void handleCreateAccount(const httplib::Request& request, httplib::Response& response);

	void handleValidateViaEmail(const httplib::Request& request, httplib::Response& response);

	void handleValidateViaSMS(const httplib::Request& request, httplib::Response& response);

	void handleResendValidateSMS(const httplib::Request& request, httplib::Response& response);

	void handleValidate(const httplib::Request& request, httplib::Response& response);

	void handleLogin(const httplib::Request& request, httplib::Response& response);

	void handleGetAdmin(const httplib::Request& request, httplib::Response& response);

	void handleRecoverPasswordViaEmail(const httplib::Request& request, httplib::Response& response);

	void handleRecoverPasswordViaSMS(const httplib::Request& request, httplib::Response& response);

	void handleVerifyResetPasswordToken(const httplib::Request& request, httplib::Response& response);

	void handleResetPassword(const httplib::Request& request, httplib::Response& response);

	void handleResendRecoverPassword(const httplib::Request& request, httplib::Response& response);

	void handleGetSubscriptionVehicles(const httplib::Request& request, httplib::Response& response);

	void handleAddSubscription(const httplib::Request& request, httplib::Response& response);

	void handleDeleteSubscription(const httplib::Request& request, httplib::Response& response);

	void handleGetVehicleHistory(const httplib::Request& request, httplib::Response& response);

	void handleAddVehicle(const httplib::Request& request, httplib::Response& response);

	void handleDeleteVehicle(const httplib::Request& request, httplib::Response& response);

	void handleUpdateAccountInformation(const httplib::Request& request, httplib::Response& response);

	void handleUpdateAccount(const httplib::Request& request, httplib::Response& response);

	void handleValidateUpdateViaEmail(const httplib::Request& request, httplib::Response& response);

	void handleValidateUpdateViaSMS(const httplib::Request& request, httplib::Response& response);

	void handleResendValidateUpdateSMS(const httplib::Request& request, httplib::Response& response);

	void handleValidateUpdate(const httplib::Request& request, httplib::Response& response);

	void handleContact(const httplib::Request& request, httplib::Response& response);

	void handleSubscribeNewsletter(const httplib::Request& request, httplib::Response& response);

	void handleUnsubscribeNewsletter(const httplib::Request& request, httplib::Response& response);

private:
	std::thread thread;
	httplib::Server server;
	std::unique_ptr<WebSocketServer> webSocketServer;
	SubscriptionManager subscriptionManager;
};
