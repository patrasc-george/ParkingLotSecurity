#pragma once

#include "databasemanager.h"
#include "logger.h"

#include <boost/asio.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/core.hpp>
#include <nlohmann/json.hpp>
#include <iostream>
#include <memory>

/**
 * @class WebSocketSession
 * @brief Handles a single WebSocket session.
 *
 * This class manages a single WebSocket session, handling communication with the client through the WebSocket protocol.
 * It reads incoming messages, processes commands, and sends back appropriate responses. The supported commands are:
 * "getVehicles", "addVehicle", and "getIsPaid".
 *
 * The session is encrypted and uses the DatabaseManager to process commands that interact with the database.
 */
class WebSocketSession : public std::enable_shared_from_this<WebSocketSession>
{
public:
	/**
	 * @brief Constructs a WebSocketSession with a given WebSocket stream.
	 * @details Initializes the session with a WebSocket stream, a reference to the shared DatabaseManager, and the logger instance.
	 * @param[in] webSocket The WebSocket stream used for communication.
	 */
	explicit WebSocketSession(boost::beast::websocket::stream<boost::asio::ip::tcp::socket> webSocket)
		: webSocket(std::move(webSocket)),
		dataBaseManager(DatabaseManager::getInstance()),
		logger(Logger::getInstance())
	{}

	/**
	 * @brief Starts the WebSocket session.
	 * @details Initiates the reading process for the WebSocket session.
	 */
	void start();

private:
	/**
	 * @brief Reads data from the WebSocket connection.
	 * @details Reads an incoming message, decrypts it, processes the command, and sends a corresponding response.
	 *          Supported commands: "getVehicles", "addVehicle", and "getIsPaid". An error response is sent for unknown commands.
	 */
	void doRead();

	/**
	 * @brief Writes a message to the WebSocket connection.
	 * @details Encrypts the message and sends it over the WebSocket. Logs errors if the WebSocket is not open or if the message fails to send.
	 * @param[in] message The message to be sent over the WebSocket.
	 */
	void doWrite(const std::string& message);

private:
	boost::beast::flat_buffer buffer;
	boost::beast::websocket::stream<boost::asio::ip::tcp::socket> webSocket;
	DatabaseManager& dataBaseManager;
	Logger& logger;
};

/**
 * @class WebSocketServer
 * @brief Manages a WebSocket server.
 *
 * This class sets up a WebSocket server to accept incoming WebSocket connections on a specified port.
 * It accepts connections, validates them, and then creates a new `WebSocketSession` for each connection.
 * The server uses the logger for logging server activities.
 */
class WebSocketServer
{
public:
	/**
	 * @brief Default constructor for WebSocketServer.
	 * @details Initializes a WebSocketServer without any parameters. Typically used before setting up the server's context and port.
	 */
	WebSocketServer();

	/**
	 * @brief Constructs a WebSocketServer with an I/O context and port.
	 * @details Sets up the server to accept incoming WebSocket connections on the specified port, using the provided I/O context.
	 * @param[in] ioContext The I/O context used to manage asynchronous I/O operations.
	 * @param[in] port The port on which the server listens for incoming connections.
	 */
	WebSocketServer(boost::asio::io_context& ioContext, unsigned short port)
		: ioContext(ioContext),
		acceptor(ioContext, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
		logger(Logger::getInstance())
	{}

public:
	/**
	 * @brief Starts the WebSocket server.
	 * @details Initiates the process to accept new WebSocket connections.
	 */
	void start();

private:
	/**
	 * @brief Accepts new WebSocket connections.
	 * @details Accepts a TCP socket, upgrades it to a WebSocket connection, validates the connection token, and starts a new session.
	 *          Closes the connection if the token is invalid.
	 */
	void doAccept();

private:
	boost::asio::io_context& ioContext;
	boost::asio::ip::tcp::acceptor acceptor;
	Logger& logger;
};
