#include "websocketclient.h"

WebSocketClient::WebSocketClient(boost::asio::io_context& ioContext, const std::string& uri)
	: ioContext(&ioContext),
	webSocket(std::make_unique<boost::beast::websocket::stream<boost::asio::ip::tcp::socket>>(boost::asio::make_strand(ioContext))),
	resolver(std::make_unique<boost::asio::ip::tcp::resolver>(ioContext)),
	uri(uri)
{
	auto pos = uri.find("://");
	auto address = uri.substr(pos + 3);
	auto colonPosition = address.find(':');

	host = address.substr(0, colonPosition);
	port = address.substr(colonPosition + 1);

	ioThread = std::thread([this, &ioContext]() { ioContext.run(); });
}


bool WebSocketClient::connect()
{
	if (!ioContext || !webSocket || !resolver)
	{
		std::cerr << "Client not properly initialized." << std::endl;
		return false;
	}

	try
	{
		auto results = resolver->resolve(host, port);
		auto endpoint = boost::asio::connect(webSocket->next_layer(), results);

		try
		{
			webSocket->handshake(host, "/");
			webSocket->text(true);
		}
		catch (const boost::system::system_error& e)
		{
			std::cerr << "Handshake failed: " << e.what() << " [code: " << e.code() << "]" << std::endl;
		}
		return true;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Connection failed: " << e.what() << std::endl;
		return false;
	}
}

void WebSocketClient::send(const std::string& message)
{
	boost::beast::error_code errorCode;
	webSocket->write(boost::asio::buffer(message), errorCode);

	if (errorCode)
		std::cerr << "Error sending message: " << errorCode.message() << std::endl;
}

std::string WebSocketClient::receiveSync()
{
	boost::beast::flat_buffer buffer;
	boost::beast::error_code errorCode;

	webSocket->read(buffer, errorCode);

	if (errorCode)
	{
		std::cerr << "Error receiving message: " << errorCode.message() << std::endl;
		return "";
	}

	std::string response = boost::beast::buffers_to_string(buffer.data());
	return response;
}

std::vector<std::string> WebSocketClient::getVehicles()
{
	nlohmann::json request = { {"command", "getVehicles"} };

	std::vector<std::string> vehicles;

	if (!webSocket->is_open())
	{
		std::cerr << "WebSocket connection is closed!" << std::endl;
		return vehicles;
	}

	send(request.dump());

	std::string response = receiveSync();

	if (response.empty())
	{
		std::cerr << "Received empty response!" << std::endl;
		return vehicles;
	}

	try
	{
		auto jsonResponse = nlohmann::json::parse(response);
		if (jsonResponse.contains("vehicles"))
			for (const auto& vehicle : jsonResponse["vehicles"])
				vehicles.push_back(vehicle.get<std::string>());
	}
	catch (const std::exception& error)
	{
		std::cerr << "Error parsing JSON: " << error.what() << std::endl;
	}

	return vehicles;
}

void WebSocketClient::addVehicle(const int& id, const std::string& imagePath, const std::string& licensePlate, const std::string& dateTime, const std::string& ticket, const std::string& timeParked, const std::string& totalAmount, const std::string& isPaid)
{
	nlohmann::json request = {
		{"command", "addVehicle"},
		{"id", id},
		{"imagePath", imagePath},
		{"licensePlate", licensePlate},
		{"dateTime", dateTime},
		{"ticket", ticket},
		{"timeParked", timeParked},
		{"totalAmount", totalAmount},
		{"isPaid", isPaid}
	};

	send(request.dump());

	receiveSync();
}

bool WebSocketClient::getIsPaid(const std::string& licensePlate)
{
	nlohmann::json request = {
		{"command", "getIsPaid"},
		{"licensePlate", licensePlate},
	};

	send(request.dump());
	std::string response = receiveSync();
	if (response.empty())
	{
		std::cerr << "Received empty response!" << std::endl;
		send(request.dump());
		return false;
	}

	bool isPaid = false;
	try
	{
		auto jsonResponse = nlohmann::json::parse(response);
		if (jsonResponse.contains("isPaid"))
		{
			std::cout << "Received response: " << response << std::endl;
			isPaid = jsonResponse["isPaid"];
		}
		else
			send(request.dump());
	}
	catch (const std::exception& error)
	{
		std::cerr << "Error parsing JSON: " << error.what() << std::endl;
		send(request.dump());
	}

	return isPaid;
}

WebSocketClient::~WebSocketClient()
{
	try
	{
		if (webSocket && webSocket->is_open())
		{
			boost::beast::error_code errorCode;
			webSocket->close(boost::beast::websocket::close_code::normal, errorCode);
		}

		if (ioThread.joinable())
			ioThread.join();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error during WebSocketClient cleanup: " << e.what() << std::endl;
	}
}
