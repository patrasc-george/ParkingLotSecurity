#pragma once

#include "databasemanager.h"
#include "logger.h"

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/websocket.hpp>
#include <deque>
#include <functional>
#include <memory>
#include <string>
#include <vector>

class WebSocketSession : public std::enable_shared_from_this<WebSocketSession>
{
public:
	WebSocketSession(boost::beast::websocket::stream<boost::asio::ip::tcp::socket>&& webSocket, const std::function<void()>& onClose);

public:
	void start();

	void sendTicket(const std::vector<unsigned char>& image, const std::string& id, const std::string& licensePlate, const std::string& dateTime);

private:
	void onRead(const boost::beast::error_code& errorCode, const std::size_t& bytesTransferred);

	void doRead();

	void onWrite(const boost::beast::error_code& errorCode, const std::size_t& bytesTransferred);

	void doWrite();

	void enqueueWrite(const std::string& message);

	void closeSession();

private:
	boost::beast::websocket::stream<boost::asio::ip::tcp::socket> webSocketStream;
	std::function<void()> onClose;
	bool closing;
	boost::beast::flat_buffer buffer;
	std::deque<std::string> writeQueue;
	DatabaseManager& dataBaseManager;
	Logger& logger;
};

class WebSocketServer
{
public:
	WebSocketServer();

	WebSocketServer(boost::asio::io_context& ioContext, const unsigned short& port);

public:
	void start();

	void sendTicket(const std::vector<unsigned char>& image, const std::string& id, const std::string& licensePlate, const std::string& dateTime);

private:
	std::string extractTokenFromTarget(const boost::beast::string_view& target);

	bool isValidToken(const std::string& token);

	void doAccept();

private:
	boost::asio::ip::tcp::acceptor acceptor;
	std::shared_ptr<WebSocketSession> session;
	Logger& logger;
};