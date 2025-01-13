#pragma once

#include "databasemanager.h"

#include <boost/asio.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/core.hpp>
#include <nlohmann/json.hpp>
#include <iostream>
#include <memory>

class WebSocketSession : public std::enable_shared_from_this<WebSocketSession>
{
public:
	explicit WebSocketSession::WebSocketSession(boost::asio::ip::tcp::socket webSocket)
		: webSocket(std::move(webSocket)),
		dataBaseManager(DatabaseManager::getInstance())
	{}

	void start();

private:
	void doRead();

	void doWrite(const std::string& message);

private:
	boost::beast::flat_buffer buffer;
	boost::beast::websocket::stream<boost::asio::ip::tcp::socket> webSocket;
	DatabaseManager& dataBaseManager;
};

class WebSocketServer
{
public:
	WebSocketServer();

	WebSocketServer(boost::asio::io_context& ioContext, unsigned short port)
		: ioContext(ioContext),
		acceptor(ioContext, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
	{}

	void start();

private:
	void doAccept();

private:
	boost::asio::io_context& ioContext;
	boost::asio::ip::tcp::acceptor acceptor;
};
