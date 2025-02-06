#include "websocketserver.h"

void WebSocketSession::start()
{
	std::cout << "[DEBUG] Starting WebSocket session." << std::endl;
	boost::beast::error_code errorCode;

	webSocket.accept(errorCode);

	if (errorCode)
	{
		std::cerr << "[ERROR] Accept failed: " << errorCode.message() << std::endl;
		return;
	}

	std::cout << "[DEBUG] WebSocket connection established." << std::endl;
	doRead();
}

void WebSocketSession::doRead()
{
	boost::beast::error_code errorCode;
	webSocket.read(buffer, errorCode);

	if (errorCode)
	{
		std::cerr << "[ERROR] Read failed: " << errorCode.message() << std::endl;
		return;
	}

	std::string message = boost::beast::buffers_to_string(buffer.data());
	std::cout << "[DEBUG] Received message: " << message << std::endl;

	buffer.consume(buffer.size());

	try
	{
		auto jsonMessage = nlohmann::json::parse(message);
		std::string command = jsonMessage["command"];
		std::cout << "[DEBUG] Command received: " << command << std::endl;

		nlohmann::json response;

		if (command == "getVehicles")
		{
			std::vector<std::string> vehicles = this->dataBaseManager.getVehicles();
			response["vehicles"] = vehicles;
		}
		else if (command == "addVehicle")
		{
			int id = jsonMessage["id"];
			std::string imagePath = jsonMessage["imagePath"];
			std::string licensePlate = jsonMessage["licensePlate"];
			std::string dateTime = jsonMessage["dateTime"];
			std::string ticket = jsonMessage["ticket"];
			std::string timeParked = jsonMessage["timeParked"];
			std::string totalAmount = jsonMessage["totalAmount"];
			std::string isPaid = jsonMessage["isPaid"];

			this->dataBaseManager.addVehicle(id, imagePath, licensePlate, dateTime, ticket, timeParked, totalAmount, isPaid);

			response["status"] = "success";
			response["message"] = "Vehicle added " + licensePlate;
		}
		else if (command == "getIsPaid")
		{
			std::string licensePlate = jsonMessage["licensePlate"];
			response["isPaid"] = this->dataBaseManager.getIsPaid(licensePlate);
		}
		else
		{
			response["status"] = "error";
			response["message"] = "Unknown command";
		}

		doWrite(response.dump());

		doRead();
	}
	catch (const nlohmann::json::exception& error)
	{
		std::cerr << "[ERROR] Invalid JSON: " << error.what() << std::endl;
	}
	catch (const std::exception& error)
	{
		std::cerr << "[ERROR] Server error: " << error.what() << std::endl;
	}
}

void WebSocketSession::doWrite(const std::string& message)
{
	if (!webSocket.is_open())
	{
		std::cerr << "[ERROR] WebSocket is not open!" << std::endl;
		return;
	}

	if (message.empty())
	{
		std::cerr << "[ERROR] Message is empty, nothing to send!" << std::endl;
		return;
	}

	boost::beast::error_code errorCode;

	webSocket.write(boost::asio::buffer(message.data(), message.size()), errorCode);

	if (errorCode)
		std::cerr << "[ERROR] Write failed: " << errorCode.message() << " [Code: " << errorCode.value() << "]" << std::endl;
	else
		std::cout << "[DEBUG] Sent: " << message << std::endl << std::endl;
}

void WebSocketServer::start()
{
	std::cout << "[DEBUG] WebSocket server starting on port " << acceptor.local_endpoint().port() << std::endl;
	doAccept();
}

void WebSocketServer::doAccept()
{
	std::cout << "[DEBUG] Waiting for new connections..." << std::endl;
	boost::beast::error_code errorCode;
	boost::asio::ip::tcp::socket socket(ioContext);

	acceptor.accept(socket, errorCode);

	if (errorCode)
	{
		std::cerr << "[ERROR] Accept failed: " << errorCode.message() << std::endl;
		return;
	}

	std::cout << "[DEBUG] New connection accepted." << std::endl;
	auto session = std::make_shared<WebSocketSession>(std::move(socket));
	session->start();

	doAccept();
}
