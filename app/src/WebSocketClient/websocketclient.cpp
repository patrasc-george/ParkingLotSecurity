#include "websocketclient.h"

#include <openssl/aes.h>
#include <openssl/buffer.h>
#include <openssl/rand.h>
#include <boost/asio/post.hpp>
#include <boost/beast/core/buffers_to_string.hpp>
#include <cstdlib>
#include <string>
#include <vector>
#include <fstream>

std::string base64Encode(const std::string& text)
{
	BIO* base64 = BIO_new(BIO_f_base64());
	BIO* mem = BIO_new(BIO_s_mem());

	BIO_set_flags(base64, BIO_FLAGS_BASE64_NO_NL);
	BIO_push(base64, mem);

	BIO_write(base64, text.data(), text.size());
	BIO_flush(base64);

	BUF_MEM* buffer = nullptr;
	BIO_get_mem_ptr(base64, &buffer);

	std::string encoded(buffer->data, buffer->length);
	BIO_free_all(base64);

	return encoded;
}

std::string base64Decode(const std::string& text)
{
	BIO* base64 = BIO_new(BIO_f_base64());
	BIO* mem = BIO_new_mem_buf(text.data(), text.size());

	BIO_set_flags(base64, BIO_FLAGS_BASE64_NO_NL);
	BIO_push(base64, mem);

	std::string decoded(text.size(), '\0');
	int size = BIO_read(base64, decoded.data(), decoded.size());
	decoded.resize(size > 0 ? size : 0);

	BIO_free_all(base64);
	return decoded;
}

std::string encrypt(const std::string& text)
{
	const char* env = std::getenv("POSTGRES_PASSWORD");
	std::string password = env ? std::string(env) : std::string();

	if (password.size() != 32)
		return text;

	const unsigned char* key = reinterpret_cast<const unsigned char*>(password.data());

	AES_KEY encryptKey;
	AES_set_encrypt_key(key, 256, &encryptKey);

	int padding = 16 - (text.size() % 16);
	std::string paddedText = text + std::string(padding, static_cast<char>(padding));

	std::string cipherText;
	cipherText.reserve(paddedText.size());
	for (size_t i = 0; i < paddedText.size(); i += 16)
	{
		unsigned char block[16];
		AES_encrypt(reinterpret_cast<const unsigned char*>(paddedText.data() + i), block, &encryptKey);
		cipherText.append(reinterpret_cast<const char*>(block), 16);
	}

	return base64Encode(cipherText);
}

nlohmann::json decrypt(const std::string& text)
{
	const char* env = std::getenv("POSTGRES_PASSWORD");
	std::string password = env ? std::string(env) : std::string();

	if (password.size() != 32)
		return nlohmann::json();

	const unsigned char* key = reinterpret_cast<const unsigned char*>(password.data());

	AES_KEY decryptKey;
	AES_set_decrypt_key(key, 256, &decryptKey);

	std::string decodedCipherText = base64Decode(text);
	if (decodedCipherText.empty() || decodedCipherText.size() % 16 != 0)
		return nlohmann::json();

	std::string decryptedText(decodedCipherText.size(), '\0');
	for (size_t i = 0; i < decodedCipherText.size(); i += 16)
	{
		AES_decrypt(
			reinterpret_cast<const unsigned char*>(decodedCipherText.data() + i),
			reinterpret_cast<unsigned char*>(decryptedText.data() + i),
			&decryptKey);
	}

	int padding = static_cast<unsigned char>(decryptedText.back());
	if (padding <= 0 || padding > 16 || static_cast<size_t>(padding) > decryptedText.size())
		return nlohmann::json();

	decryptedText.resize(decryptedText.size() - padding);

	try
	{
		return nlohmann::json::parse(decryptedText);
	}
	catch (...)
	{
		return nlohmann::json();
	}
}

WebSocketClient::WebSocketClient()
	: ioContext(nullptr),
	workGuard(nullptr),
	webSocket(nullptr),
	resolver(nullptr),
	uri(""),
	host(""),
	port(""),
	logger(Logger::getInstance()),
	nextRequestId(1),
	listeningStarted(false)
{
}

WebSocketClient::WebSocketClient(boost::asio::io_context& ioContext_, const std::string& uri)
	: ioContext(&ioContext_),
	workGuard(std::make_unique<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>>(boost::asio::make_work_guard(ioContext_))),
	webSocket(std::make_unique<boost::beast::websocket::stream<boost::asio::ip::tcp::socket>>(boost::asio::make_strand(ioContext_))),
	resolver(std::make_unique<boost::asio::ip::tcp::resolver>(ioContext_)),
	uri(uri),
	logger(Logger::getInstance()),
	nextRequestId(1),
	listeningStarted(false)
{
#ifdef _DEBUG
	dataBasePath = "../../../database/";
#else
	dataBasePath = "database/";
#endif

	auto position = uri.find("://");
	auto address = uri.substr(position + 3);
	auto colonPosition = address.find(':');

	host = address.substr(0, colonPosition);
	port = address.substr(colonPosition + 1);

	ioThread = std::thread([this]()
		{
			ioContext->run();
		});
}

void WebSocketClient::connect()
{
#ifdef _DEBUG
	std::string fullUri = uri + "?token=" + std::getenv("POSTGRES_PASSWORD_DEBUG");
#else
	std::string fullUri = uri_ + "?token=" + std::getenv("POSTGRES_PASSWORD");
#endif

	try
	{
		auto results = resolver->resolve(host, port);
		boost::asio::connect(webSocket->next_layer(), results);
		webSocket->handshake(host, fullUri);
		webSocket->text(true);
	}
	catch (...)
	{
		return;
	}

	startListening();
}

void WebSocketClient::setTicketCallback(const std::function<void(const std::string&, const std::string&, const std::string&, const std::string&)>& callback)
{
	ticketCallback = std::move(callback);
}

void WebSocketClient::handleIncomingMessage(const std::string& message)
{
	nlohmann::json decrypted = decrypt(message);
	std::string type = decrypted.value("type", "");

	if (type == "ticket")
	{
		std::string id = decrypted.value("id", "");
		std::string licensePlate = decrypted.value("licensePlate", "");
		std::string dateTime = decrypted.value("dateTime", "");
		std::string binaryString = decrypted.value("image", "");
		std::string decoded = base64Decode(binaryString);
		std::vector<unsigned char> imageData(decoded.begin(), decoded.end());

		if (ticketCallback)
		{
			std::string savePath = dataBasePath + "websiteTickets/" + id + ".jpg";
			std::ofstream file(savePath, std::ios::binary);
			if (file.is_open())
			{
				file.write(reinterpret_cast<const char*>(imageData.data()), imageData.size());
				file.close();
			}

			ticketCallback(id, savePath, licensePlate, dateTime);
		}

		return;
	}

	if (type == "response")
	{
		uint64_t requestId = decrypted.value("requestId", 0ULL);
		std::shared_ptr<PendingRequest> pending;

		{
			std::lock_guard<std::mutex> lock(pendingMutex);
			auto iterator = pendingRequests.find(requestId);
			if (iterator == pendingRequests.end())
				return;

			pending = iterator->second;
		}

		{
			std::lock_guard<std::mutex> lock(pending->mutex);
			pending->response = decrypted;
			pending->ready = true;
		}

		pending->conditionVariable.notify_all();
	}
}

void WebSocketClient::onRead(const boost::beast::error_code& errorCode, const std::size_t&)
{
	if (errorCode)
	{
		std::lock_guard<std::mutex> lock(pendingMutex);
		for (auto& [id, pending] : pendingRequests)
		{
			std::lock_guard<std::mutex> pendingLock(pending->mutex);
			pending->ready = true;
			pending->response = nlohmann::json();
			pending->conditionVariable.notify_all();
		}
		pendingRequests.clear();

		return;
	}

	std::string response = boost::beast::buffers_to_string(buffer.data());
	buffer.consume(buffer.size());

	handleIncomingMessage(response);
	doRead();
}

void WebSocketClient::doRead()
{
	webSocket->async_read(
		buffer,
		boost::asio::bind_executor(
			webSocket->get_executor(),
			[this](const boost::beast::error_code& errorCode, const std::size_t& bytesTransferred)
			{
				onRead(errorCode, bytesTransferred);
			}));
}

void WebSocketClient::startListening()
{
	bool expected = false;
	if (listeningStarted.compare_exchange_strong(expected, true))
		doRead();
}

void WebSocketClient::onWrite(const boost::beast::error_code& errorCode, const std::size_t&)
{
	if (errorCode)
		return;

	writeQueue.pop_front();
	if (!writeQueue.empty())
		doWrite();
}

void WebSocketClient::doWrite()
{
	webSocket->async_write(
		boost::asio::buffer(writeQueue.front()),
		boost::asio::bind_executor(
			webSocket->get_executor(),
			[this](const boost::beast::error_code& errorCode, const std::size_t& bytesTransferred)
			{
				onWrite(errorCode, bytesTransferred);
			}));
}

void WebSocketClient::enqueueWrite(const std::string& message)
{
	boost::asio::post(
		webSocket->get_executor(),
		[this, message = std::move(message)]() mutable
		{
			bool writeInProgress = !writeQueue.empty();
			writeQueue.push_back(std::move(message));

			if (!writeInProgress)
				doWrite();
		});
}

nlohmann::json WebSocketClient::sendRequestAndWait(nlohmann::json& request)
{
	uint64_t requestId = nextRequestId.fetch_add(1);

	request["type"] = "request";
	request["requestId"] = requestId;

	auto pending = std::make_shared<PendingRequest>();
	{
		std::lock_guard<std::mutex> lock(pendingMutex);
		pendingRequests[requestId] = pending;
	}

	enqueueWrite(encrypt(request.dump()));

	std::unique_lock<std::mutex> lock(pending->mutex);
	bool received = pending->conditionVariable.wait_for(
		lock,
		std::chrono::seconds(1),
		[pending]()
		{
			return pending->ready;
		});

	{
		std::lock_guard<std::mutex> pendingLock(pendingMutex);
		pendingRequests.erase(requestId);
	}

	if (!received)
		return nlohmann::json();

	return pending->response;
}

std::vector<std::string> WebSocketClient::getVehicles()
{
	nlohmann::json request = {
		{"command", "getVehicles"}
	};
	nlohmann::json response = sendRequestAndWait(request);

	std::vector<std::string> vehicles;
	if (response.contains("vehicles"))
		for (const auto& vehicle : response["vehicles"])
			vehicles.push_back(vehicle.get<std::string>());

	return vehicles;
}

void WebSocketClient::addVehicle(const std::string& licensePlate, const std::string& dateTime, const std::string& ticket, const float& totalAmount)
{
	nlohmann::json request = {
		{"command", "addVehicle"},
		{"licensePlate", licensePlate},
		{"dateTime", dateTime},
		{"ticket", ticket},
		{"totalAmount", totalAmount}
	};

	sendRequestAndWait(request);
}

bool WebSocketClient::getIsPaid(const std::string& licensePlate)
{
	nlohmann::json request = {
		{"command", "getIsPaid"},
		{"licensePlate", licensePlate}
	};

	nlohmann::json response = sendRequestAndWait(request);
	if (response.contains("isPaid"))
		return response["isPaid"].get<bool>();

	return false;
}

std::vector<std::string> WebSocketClient::getTickets()
{
	nlohmann::json request = {
		{"command", "getTickets"}
	};
	nlohmann::json response = sendRequestAndWait(request);

	std::vector<std::string> tickets;
	if (response.contains("tickets"))
		for (const auto& vehicle : response["tickets"])
			tickets.push_back(vehicle.get<std::string>());

	return tickets;
}

WebSocketClient::~WebSocketClient()
{
	if (webSocket && webSocket->is_open())
	{
		boost::asio::post(
			webSocket->get_executor(),
			[this]()
			{
				boost::beast::error_code ignored;
				webSocket->close(boost::beast::websocket::close_code::normal, ignored);
			});
	}

	if (workGuard)
		workGuard->reset();

	if (ioContext)
		ioContext->stop();

	{
		std::lock_guard<std::mutex> lock(pendingMutex);
		for (auto& [id, pending] : pendingRequests)
		{
			std::lock_guard<std::mutex> pendingLock(pending->mutex);
			pending->ready = true;
			pending->response = nlohmann::json();
			pending->conditionVariable.notify_all();
		}
		pendingRequests.clear();
	}

	if (ioThread.joinable())
		ioThread.join();
}
