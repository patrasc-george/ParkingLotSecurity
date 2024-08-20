#include "httpServer.h"

Server::Server() : vehicleManager(VehicleManager::getInstance())
{
	server.Post("/api/endpoint", [this](const httplib::Request& req, httplib::Response& res) {
		res.set_header("Access-Control-Allow-Origin", "*");
		res.set_header("Access-Control-Allow-Methods", "POST");
		res.set_header("Access-Control-Allow-Headers", "Content-Type");

		this->handlePost(req, res);
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
	bool paymentSuccess = false;

	if (request.has_param("licensePlate"))
	{
		auto licensePlate = request.get_param_value("licensePlate");

		std::transform(licensePlate.begin(), licensePlate.end(), licensePlate.begin(), [](unsigned char c) {
			return std::toupper(c); });

		if (vehicleManager.pay(licensePlate))
			paymentSuccess = true;
		else
		{
			response.status = 400;
			response.set_content("false", "text/plain");
			return;
		}
	}
	else if (request.has_file("qrCodeImage"))
	{
		const auto& file = request.get_file_value("qrCodeImage");

		std::string savePath = vehicleManager.getDataBasePath() + "uploadedQr/" + file.filename;
		std::ofstream ofs(savePath, std::ios::binary);
		ofs << file.content;
		ofs.close();

		if (vehicleManager.pay(savePath, true))
			paymentSuccess = true;
		else
		{
			response.status = 400;
			response.set_content("false", "text/plain");
			return;
		}
	}

	if (paymentSuccess)
		response.set_content("true", "text/plain");
	else
	{
		response.status = 400;
		response.set_content("false", "text/plain");
	}
}
