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
	std::string licensePlate;
	std::string dateTime;
	std::string jsonResponse;

	if (request.has_param("licensePlate"))
	{
		auto data = request.get_param_value("licensePlate");

		std::transform(data.begin(), data.end(), data.begin(), [](unsigned char c) {
			return std::toupper(c); });

		if (!vehicleManager.pay(data, licensePlate, dateTime))
		{
			response.status = 400;
			jsonResponse = R"({"success": false})";
			response.set_content(jsonResponse, "application/json");
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
			jsonResponse = R"({"success": false})";
			response.set_content(jsonResponse, "application/json");
			return;
		}
	}

	if (!licensePlate.empty() && !dateTime.empty())
	{
		jsonResponse = "{ \"success\": true, \"licensePlate\": \"" + licensePlate + "\", \"dateTime\": \"" + dateTime + "\" }";
		response.set_content(jsonResponse, "application/json");
	}
	else
	{
		response.status = 400;
		jsonResponse = R"({"success": false})";
		response.set_content(jsonResponse, "application/json");
	}
}
