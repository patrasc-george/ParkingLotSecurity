#include "websocketserver.h"

#include <openssl/aes.h>
#include <openssl/buffer.h>
#include <openssl/rand.h>
#include <boost/asio/post.hpp>
#include <boost/beast/core/buffers_to_string.hpp>
#include <nlohmann/json.hpp>
#include <atomic>
#include <cstdlib>
#include <string>
#include <vector>

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

	return nlohmann::json::parse(decryptedText);
}

WebSocketSession::WebSocketSession(boost::beast::websocket::stream<boost::asio::ip::tcp::socket>&& webSocket, const std::function<void()>& onClose)
	: webSocketStream(std::move(webSocket)),
	onClose(std::move(onClose)),
	closing(false),
	dataBaseManager(DatabaseManager::getInstance()),
	logger(Logger::getInstance())
{
}

void WebSocketSession::onRead(const boost::beast::error_code& errorCode, const std::size_t&)
{
	if (errorCode)
	{
		closeSession();
		return;
	}

	std::string message = boost::beast::buffers_to_string(buffer.data());
	buffer.consume(buffer.size());

	nlohmann::json decrypted = decrypt(message);
	std::string command = decrypted.value("command", "");
	std::uint64_t requestId = decrypted.value("requestId", 0ULL);

	nlohmann::json response;
	response["type"] = "response";
	response["command"] = command;
	response["requestId"] = requestId;

	if (command == "getVehicles")
	{
		response["vehicles"] = dataBaseManager.getVehicles();
		response["status"] = "success";
	}
	else if (command == "addVehicle")
	{
		std::string licensePlate = decrypted.value("licensePlate", "");
		std::string dateTime = decrypted.value("dateTime", "");
		std::string ticket = decrypted.value("ticket", "");
		float totalAmount = decrypted.value("totalAmount", 0);

		dataBaseManager.addVehicle(licensePlate, dateTime, ticket, totalAmount);

		response["status"] = "success";
		response["message"] = "Vehicle added " + licensePlate;
	}
	else if (command == "getIsPaid")
	{
		std::string licensePlate = decrypted.value("licensePlate", "");

		response["isPaid"] = dataBaseManager.getIsPaid(licensePlate);
		response["status"] = "success";
	}
	else if (command == "getTickets")
	{
		response["tickets"] = dataBaseManager.getTickets();
		response["status"] = "success";
	}
	else
	{
		response["status"] = "error";
		response["message"] = "Unknown command";
	}

	enqueueWrite(encrypt(response.dump()));
	doRead();
}


void WebSocketSession::doRead()
{
	auto self = shared_from_this();

	webSocketStream.async_read(
		buffer,
		boost::asio::bind_executor(
			webSocketStream.get_executor(),
			[self](const boost::beast::error_code& errorCode, const std::size_t& bytesTransferred)
			{
				self->onRead(errorCode, bytesTransferred);
			}));
}

void WebSocketSession::start()
{
	webSocketStream.text(true);
	doRead();
}

void WebSocketSession::onWrite(const boost::beast::error_code& errorCode, const std::size_t&)
{
	if (errorCode)
	{
		closeSession();
		return;
	}

	writeQueue.pop_front();
	if (!writeQueue.empty())
		doWrite();
}

void WebSocketSession::doWrite()
{
	auto self = shared_from_this();

	webSocketStream.async_write(
		boost::asio::buffer(writeQueue.front()),
		boost::asio::bind_executor(
			webSocketStream.get_executor(),
			[self](const boost::beast::error_code& errorCode, const std::size_t& bytesTransferred)
			{
				self->onWrite(errorCode, bytesTransferred);
			}));
}

void WebSocketSession::enqueueWrite(const std::string& message)
{
	auto self = shared_from_this();

	boost::asio::post(
		webSocketStream.get_executor(),
		[self, message = std::move(message)]() mutable
		{
			bool writeInProgress = !self->writeQueue.empty();
			self->writeQueue.push_back(std::move(message));

			if (!writeInProgress)
				self->doWrite();
		});
}

void WebSocketSession::sendTicket(const std::vector<unsigned char>& image, const std::string& id, const std::string& licensePlate, const std::string& dateTime)
{
	dataBaseManager.addTicket(id, licensePlate, dateTime);

	static std::atomic<uint64_t> ticketSequence{ 0 };
	uint64_t ticketId = ticketSequence++;
	std::string base64Image = base64Encode(std::string(image.begin(), image.end()));

	nlohmann::json payload = {
		{"type", "ticket"},
		{"ticketId", ticketId},
		{"image", base64Image},
		{"id", id},
		{"licensePlate", licensePlate},
		{"dateTime", dateTime}
	};
	enqueueWrite(encrypt(payload.dump()));
}

void WebSocketSession::closeSession()
{
	if (closing)
		return;

	closing = true;
	writeQueue.clear();
	boost::beast::error_code ignored;
	webSocketStream.next_layer().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored);
	webSocketStream.next_layer().close(ignored);

	if (onClose)
		onClose();
}

WebSocketServer::WebSocketServer()
	: acceptor(*static_cast<boost::asio::io_context*>(nullptr)),
	logger(Logger::getInstance())
{
}

WebSocketServer::WebSocketServer(boost::asio::io_context& ioContext, const unsigned short& port)
	: acceptor(ioContext, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
	logger(Logger::getInstance())
{
}

std::string WebSocketServer::extractTokenFromTarget(const boost::beast::string_view& target)
{
	std::string targetSring(target.begin(), target.end());

	auto position = targetSring.find("token=");
	if (position == std::string::npos)
		return {};

	auto endPosition = targetSring.find('&', position);
	if (endPosition == std::string::npos)
		return targetSring.substr(position + 6);

	return targetSring.substr(position + 6, endPosition - (position + 6));
}

bool WebSocketServer::isValidToken(const std::string& token)
{
#ifdef _DEBUG
	const char* env = std::getenv("POSTGRES_PASSWORD_DEBUG");
#else
	const char* env = std::getenv("POSTGRES_PASSWORD");
#endif

	std::string expectedToken = env ? std::string(env) : std::string();
	return token == expectedToken;
}

void WebSocketServer::doAccept()
{
	acceptor.async_accept(
		[this](const boost::beast::error_code& errorCode, boost::asio::ip::tcp::socket socket) mutable
		{
			if (errorCode)
			{
				doAccept();
				return;
			}

			if (session)
			{
				boost::beast::error_code ignored;
				socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored);
				socket.close(ignored);

				doAccept();
				return;
			}

			auto sharedSocket = std::make_shared<boost::asio::ip::tcp::socket>(std::move(socket));
			auto sharedBuffer = std::make_shared<boost::beast::flat_buffer>();
			auto sharedRequest = std::make_shared<boost::beast::http::request<boost::beast::http::string_body>>();

			boost::beast::http::async_read(
				*sharedSocket,
				*sharedBuffer,
				*sharedRequest,
				[this, sharedSocket, sharedBuffer, sharedRequest](const boost::beast::error_code& errorCode, const std::size_t&) mutable
				{
					if (errorCode)
					{
						doAccept();
						return;
					}

					std::string token = extractTokenFromTarget(sharedRequest->target());
					boost::beast::websocket::stream<boost::asio::ip::tcp::socket> webSocket(std::move(*sharedSocket));
					if (!isValidToken(token))
					{
						boost::beast::error_code ignored;
						webSocket.next_layer().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored);
						webSocket.next_layer().close(ignored);

						doAccept();
						return;
					}

					auto sharedWebSocket = std::make_shared<boost::beast::websocket::stream<boost::asio::ip::tcp::socket>>(std::move(webSocket));
					sharedWebSocket->async_accept(
						*sharedRequest,
						[this, sharedWebSocket](const boost::beast::error_code& errorCode) mutable
						{
							if (errorCode)
							{
								doAccept();
								return;
							}

							session = std::make_shared<WebSocketSession>(
								std::move(*sharedWebSocket),
								[this]()
								{
									session.reset();
								});
							session->start();
							doAccept();
						});
				});
		});
}

void WebSocketServer::start()
{
	doAccept();
}

void WebSocketServer::sendTicket(const std::vector<unsigned char>& image, const std::string& id, const std::string& licensePlate, const std::string& dateTime)
{
	if (session)
		session->sendTicket(image, id, licensePlate, dateTime);
}