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

/**
 * @class WebSocketClient
 * @brief Manages WebSocket connections and communication with a server.
 *
 * The WebSocketClient class is responsible for establishing a WebSocket connection, sending and receiving messages,
 * and managing interactions with a WebSocket server. It supports asynchronous operations, allowing the client to
 * communicate with the server without blocking the main thread. The class can retrieve vehicle data from the server,
 * add new vehicles to the system, and check the payment status of vehicles. Additionally, it handles encryption and
 * decryption of messages for secure communication and ensures proper cleanup of resources during connection shutdown.
 */
class WEBSOCKETCLIENT_API WebSocketClient : public std::enable_shared_from_this<WebSocketClient>
{
public:
	/**
	 * @brief Default constructor for WebSocketClient.
	 * @details Initializes the WebSocket client with null pointers for I/O context, WebSocket, and resolver,
	 *          and empty strings for URI, host, and port. This constructor is typically used when the client
	 *          is not immediately connected and will be configured later. The logger instance is initialized
	 *          using the singleton pattern.
	 */
	WebSocketClient() :
		ioContext(nullptr),
		webSocket(nullptr),
		resolver(nullptr),
		uri(""),
		host(""),
		port(""),
		logger(Logger::getInstance())
	{};

	/**
	 * @brief Constructor for WebSocketClient.
	 * @details Initializes the WebSocket client by resolving the URI, setting up a resolver, and creating a WebSocket object.
	 *          Starts the I/O context in a separate thread for asynchronous operations.
	 * @param[in] ioContext The I/O context that manages asynchronous operations.
	 * @param[in] uri The URI to connect to the WebSocket server.
	 */
	WebSocketClient(boost::asio::io_context& ioContext, const std::string& uri);

	/**
	 * @brief Destructor for WebSocketClient.
	 * @details Cleans up the WebSocket connection and ensures proper shutdown of the I/O thread.
	 *          It gracefully closes the WebSocket connection and joins the I/O thread.
	 */
	~WebSocketClient();

public:
	/**
	 * @brief Establishes a WebSocket connection to the server.
	 * @details Resolves the server address, performs a handshake, and connects the WebSocket client to the server.
	 *          It also handles potential errors and returns `false` if the connection fails.
	 * @return `true` if the connection is successful, otherwise `false`.
	 */
	bool connect();

	/**
	 * @brief Retrieves the list of vehicles from the WebSocket server.
	 * @details This function sends a request to the server to get a list of vehicles and decrypts the response.
	 *          The list of vehicle data is returned as a vector of strings.
	 * @return A vector of strings representing the vehicles.
	 */
	std::vector<std::string> getVehicles();
	/**
	 * @brief Sends a request to add a new vehicle to the system.
	 * @details This function sends a request to the server with the vehicle's details, including the ID, image path,
	 *          license plate, datetime, ticket, time parked, total amount, and payment status.
	 * @param[in] id The ID of the vehicle.
	 * @param[in] licensePlate The license plate of the vehicle.
	 * @param[in] dateTime The date and time when the vehicle was detected.
	 * @param[in] ticket The ticket associated with the vehicle.
	 * @param[in] totalAmount The total amount for parking.
	 * @return void
	 */
	void addVehicle(const std::string& licensePlate, const std::string& dateTime, const std::string& ticket, const float& totalAmount);

	/**
	 * @brief Checks whether the specified vehicle has been paid for.
	 * @details This function sends a request to the server to check the payment status of a vehicle identified by its license plate.
	 * @param[in] licensePlate The license plate of the vehicle.
	 * @return `true` if the vehicle has been paid for, otherwise `false`.
	 */
	bool getIsPaid(const std::string& licensePlate);

private:
	/**
	 * @brief Sends a message through the WebSocket connection.
	 * @details This function encrypts the message and sends it through the WebSocket.
	 * @param[in] message The message to be sent to the server.
	 * @return void
	 */
	void send(const std::string& message);

	/**
	 * @brief Receives a message synchronously from the WebSocket server.
	 * @details This function reads data from the WebSocket, handling potential errors in the process.
	 * @return The received message as a string.
	 */
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
	Logger& logger;
};
