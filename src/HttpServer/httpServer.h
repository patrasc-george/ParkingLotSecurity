#include "httplib.h"
#include "vehiclemanager.h"
#include "subscriptionmanager.h"

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

class Server
{
public:
	Server();

	~Server();

private:
	void handlePost(const httplib::Request& request, httplib::Response& response);

	void handleCreateAccount(const httplib::Request& request, httplib::Response& response);

	void handleValidate(const httplib::Request& request, httplib::Response& response);

	void handleLogin(const httplib::Request& request, httplib::Response& response);

	void handleRecoverPassword(const httplib::Request& request, httplib::Response& response);

	void handleResetPassword(const httplib::Request& request, httplib::Response& response);

	void handleGetSubscriptionVehicles(const httplib::Request& request, httplib::Response& response);

	void handleAddSubscription(const httplib::Request& request, httplib::Response& response);

	void handleDeleteSubscription(const httplib::Request& request, httplib::Response& response);

	void handleGetVehicleHistory(const httplib::Request& request, httplib::Response& response);

	void handleAddVehicle(const httplib::Request& request, httplib::Response& response);

	void handleDeleteVehicle(const httplib::Request& request, httplib::Response& response);

	void handleUpdateName(const httplib::Request& request, httplib::Response& response);

	void handleUpdatePassword(const httplib::Request& request, httplib::Response& response);

	void handleUpdatePhone(const httplib::Request& request, httplib::Response& response);

private:
	httplib::Server server;
	std::thread thread;
	VehicleManager& vehicleManager;
	SubscriptionManager* subscriptionManager;
	Poco::UUIDGenerator uuidGenerator;
	Poco::Net::SecureSMTPClientSession session;
	Poco::SharedPtr<Poco::Net::InvalidCertificateHandler> pCert;
	Poco::Net::Context::Ptr pContext;
};
