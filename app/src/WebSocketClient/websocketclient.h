#pragma once

#ifdef WEBSOCKETCLIENT_EXPORTS
#define WEBSOCKETCLIENT_API __declspec(dllexport)
#else
#define WEBSOCKETCLIENT_API __declspec(dllimport)
#endif

#include "logger.h"

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <iostream>
#include <functional>

class WEBSOCKETCLIENT_API WebSocketClient : public std::enable_shared_from_this<WebSocketClient>
{
public:
	WebSocketClient() :
		ioContext(nullptr),
		webSocket(nullptr),
		resolver(nullptr),
		uri(""),
		host(""),
		port("")
	{};

	WebSocketClient(boost::asio::io_context& ioContext, const std::string& uri);

	~WebSocketClient();

public:
	bool connect();

	std::vector<std::string> getVehicles();

	void addVehicle(const int& id, const std::string& imagePath, const std::string& licensePlate, const std::string& dateTime, const std::string& ticket, const std::string& timeParked, const std::string& totalAmount, const std::string& isPaid);

	bool getIsPaid(const std::string& licensePlate);

private:
	void send(const std::string& message);

	std::string receiveSync();

private:
	boost::asio::io_context* ioContext;
	std::unique_ptr<boost::beast::websocket::stream<boost::asio::ip::tcp::socket>> webSocket;
	std::unique_ptr<boost::asio::ip::tcp::resolver> resolver;
	boost::beast::flat_buffer buffer;
	std::thread ioThread;
	std::string uri;
	std::string host;
	std::string port;
	Logger logger;
};
