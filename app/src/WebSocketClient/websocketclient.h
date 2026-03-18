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
#include <atomic>
#include <condition_variable>
#include <deque>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

class WEBSOCKETCLIENT_API WebSocketClient : public std::enable_shared_from_this<WebSocketClient>
{
private:
	struct PendingRequest
	{
		std::mutex mutex;
		std::condition_variable conditionVariable;
		bool ready = false;
		nlohmann::json response;
	};

public:
	WebSocketClient();

	WebSocketClient(boost::asio::io_context& ioContext, const std::string& uri);

	~WebSocketClient();

public:
	void setTicketCallback(const std::function<void(const std::string&, const std::string&, const std::string&, const std::string&)>& callback);

public:
	void connect();

	void startListening();

	std::vector<std::string> getVehicles();

	void addVehicle(const std::string& licensePlate, const std::string& dateTime, const std::string& ticket, const float& totalAmount);

	bool getIsPaid(const std::string& licensePlate);

	std::vector<std::string> getTickets();

private:
	void handleIncomingMessage(const std::string& message);

	void onRead(const boost::beast::error_code& errorCode, const std::size_t& bytesTransferred);

	void doRead();

	void onWrite(const boost::beast::error_code& errorCode, const std::size_t& bytesTransferred);

	void doWrite();

	void enqueueWrite(const std::string& message);

	nlohmann::json sendRequestAndWait(nlohmann::json& request);

private:
	boost::asio::io_context* ioContext;
	std::unique_ptr<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>> workGuard;
	std::unique_ptr<boost::beast::websocket::stream<boost::asio::ip::tcp::socket>> webSocket;
	std::unique_ptr<boost::asio::ip::tcp::resolver> resolver;
	boost::beast::flat_buffer buffer;
	std::thread ioThread;
	std::string uri;
	std::string host;
	std::string port;
	Logger& logger;
	std::atomic<uint64_t> nextRequestId;
	std::atomic<bool> listeningStarted;
	std::mutex pendingMutex;
	std::unordered_map<uint64_t, std::shared_ptr<PendingRequest>> pendingRequests;
	std::deque<std::string> writeQueue;
	std::string dataBasePath;
	std::function<void(const std::string&, const std::string&, const std::string&, const std::string&)> ticketCallback;
};