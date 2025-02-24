#include "websocketserver.h"

#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/buffer.h>
#include <bitset>

std::string base64Encode(const std::string& text)
{
	Logger& logger = Logger::getInstance();

	BIO* bio = BIO_new(BIO_f_base64());
	if (!bio)
	{
		LOG_MESSAGE(CRITICAL) << "Failed to create BIO for base64 encoding." << std::endl;
		return "";
	}

	BIO* mem = BIO_new(BIO_s_mem());
	if (!mem)
	{
		LOG_MESSAGE(CRITICAL) << "Failed to create memory BIO." << std::endl;
		BIO_free_all(bio);
		return "";
	}

	bio = BIO_push(bio, mem);
	BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);

	BIO_write(bio, text.data(), text.size());
	BIO_flush(bio);

	BUF_MEM* buffer;
	BIO_get_mem_ptr(bio, &buffer);
	if (!buffer)
	{
		LOG_MESSAGE(CRITICAL) << "Failed to get memory pointer from BIO." << std::endl;
		BIO_free_all(bio);
		return "";
	}

	std::string encoded(buffer->data, buffer->length);
	BIO_free_all(bio);
	return encoded;
}

std::string encrypt(const std::string& text)
{
	Logger& logger = Logger::getInstance();

	std::string password(std::getenv("POSTGRES_PASSWORD"));
	if (password.empty() || password.size() != 32)
	{
		LOG_MESSAGE(CRITICAL) << "Invalid or missing password for encryption." << std::endl;
		return text;
	}

	const unsigned char* key = reinterpret_cast<const unsigned char*>(password.c_str());

	AES_KEY encKey;
	if (AES_set_encrypt_key(key, 256, &encKey) < 0)
	{
		LOG_MESSAGE(CRITICAL) << "Failed to set encryption key." << std::endl;
		return "";
	}

	int padding = 16 - (text.size() % 16);
	std::string paddedText = text + std::string(padding, '\0');

	std::string cipherText;
	try
	{
		for (int i = 0; i < paddedText.size(); i += 16)
		{
			unsigned char block[16];
			AES_encrypt(reinterpret_cast<const unsigned char*>(&paddedText[i]), block, &encKey);
			cipherText.append(reinterpret_cast<const char*>(block), 16);
		}
	}
	catch (const std::exception& e)
	{
		LOG_MESSAGE(CRITICAL) << "Error during encryption process: " << e.what() << std::endl;
		return "";
	}

	return base64Encode(cipherText);
}

std::string base64Decode(const std::string& text)
{
	Logger& logger = Logger::getInstance();

	BIO* bio = BIO_new_mem_buf(text.data(), text.size());
	if (!bio)
	{
		LOG_MESSAGE(CRITICAL) << "Failed to create memory BIO for base64 decoding." << std::endl;
		return "";
	}

	BIO* b64 = BIO_new(BIO_f_base64());
	if (!b64)
	{
		LOG_MESSAGE(CRITICAL) << "Failed to create BIO filter for base64 decoding." << std::endl;
		BIO_free_all(bio);
		return "";
	}

	bio = BIO_push(b64, bio);
	BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);

	std::string decoded(text.size(), '\0');
	int size = BIO_read(bio, &decoded[0], text.size());

	if (size <= 0)
	{
		LOG_MESSAGE(CRITICAL) << "Failed to read decoded data from BIO." << std::endl;
		BIO_free_all(bio);
		return "";
	}

	decoded.resize(size);

	BIO_free_all(bio);
	return decoded;
}

nlohmann::json decrypt(const std::string& text)
{
	Logger& logger = Logger::getInstance();

	std::string password(std::getenv("POSTGRES_PASSWORD"));
	if (password.empty() || password.size() != 32)
	{
		LOG_MESSAGE(CRITICAL) << "Invalid or missing password for decryption." << std::endl;
		return nlohmann::json();
	}

	const unsigned char* key = reinterpret_cast<const unsigned char*>(password.c_str());

	AES_KEY dec_key;
	if (AES_set_decrypt_key(key, 256, &dec_key) < 0)
	{
		LOG_MESSAGE(CRITICAL) << "Failed to set decryption key." << std::endl;
		return nlohmann::json();
	}

	std::string decodedCipherText = base64Decode(text);
	std::vector<unsigned char> cipherText(decodedCipherText.begin(), decodedCipherText.end());

	std::string decryptedText;
	decryptedText.resize(cipherText.size());

	for (int i = 0; i < cipherText.size(); i += 16)
		AES_decrypt(&cipherText[i], reinterpret_cast<unsigned char*>(&decryptedText[i]), &dec_key);

	int padding = decryptedText[decryptedText.size() - 1];
	decryptedText.resize(decryptedText.size() - padding);

	try
	{
		return nlohmann::json::parse(decryptedText);
	}
	catch (const nlohmann::json::parse_error& e)
	{
		LOG_MESSAGE(CRITICAL) << "Failed to parse decrypted text into JSON: " << e.what() << std::endl;
		return nlohmann::json();
	}
}

void WebSocketSession::start()
{
	doRead();
}

void WebSocketSession::doRead()
{
	boost::beast::error_code errorCode;
	webSocket.binary(true);
	webSocket.read(buffer, errorCode);

	if (errorCode)
		return;

	std::string message = boost::beast::buffers_to_string(buffer.data());
	nlohmann::json decryptMessage = decrypt(message);
	buffer.consume(buffer.size());

	std::string command = decryptMessage["command"];
	nlohmann::json response;
	if (command == "getVehicles")
	{
		std::vector<std::string> vehicles = this->dataBaseManager.getVehicles();
		response["vehicles"] = vehicles;
	}
	else if (command == "addVehicle")
	{
		int id = decryptMessage["id"];
		std::string imagePath = decryptMessage["imagePath"];
		std::string licensePlate = decryptMessage["licensePlate"];
		std::string dateTime = decryptMessage["dateTime"];
		std::string ticket = decryptMessage["ticket"];
		std::string timeParked = decryptMessage["timeParked"];
		std::string totalAmount = decryptMessage["totalAmount"];
		std::string isPaid = decryptMessage["isPaid"];

		this->dataBaseManager.addVehicle(id, imagePath, licensePlate, dateTime, ticket, timeParked, totalAmount, isPaid);

		response["status"] = "success";
		response["message"] = "Vehicle added " + licensePlate;
	}
	else if (command == "getIsPaid")
	{
		std::string licensePlate = decryptMessage["licensePlate"];
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
void WebSocketSession::doWrite(const std::string& message)
{
	if (!webSocket.is_open())
	{
		LOG_MESSAGE(CRITICAL) << "WebSocket is not open. Cannot send message." << std::endl;
		return;
	}

	if (message.empty())
	{
		LOG_MESSAGE(WARNING) << "Attempted to send an empty message." << std::endl;
		return;
	}

	std::string encryptMessage = encrypt(message);
	boost::beast::error_code errorCode;
	webSocket.write(boost::asio::buffer(encryptMessage.data(), encryptMessage.size()), errorCode);

	if (errorCode)
	{
		LOG_MESSAGE(CRITICAL) << "Failed to send message: " << errorCode.message() << std::endl;
		return;
	}
}

void WebSocketServer::start()
{
	doAccept();
}

bool isValidToken(const std::string& token)
{
	Logger& logger = Logger::getInstance();

	std::string expectedToken;
#ifdef _DEBUG
	expectedToken = std::getenv("POSTGRES_PASSWORD_DEBUG");
#else
	expectedToken = std::getenv("POSTGRES_PASSWORD");
#endif

	if (token != expectedToken)
	{
		LOG_MESSAGE(CRITICAL) << "Invalid token: " << token << std::endl;
		return false;
	}

	return true;
}

void WebSocketServer::doAccept()
{
	boost::beast::error_code errorCode;
	boost::asio::ip::tcp::socket socket(ioContext);

	acceptor.accept(socket, errorCode);

	if (errorCode)
	{
		LOG_MESSAGE(CRITICAL) << "Failed to accept connection: " << errorCode.message() << std::endl;
		return;
	}

	boost::beast::flat_buffer buffer;
	boost::beast::http::request<boost::beast::http::string_body> req;
	boost::beast::http::read(socket, buffer, req, errorCode);

	if (errorCode)
	{
		LOG_MESSAGE(CRITICAL) << "Failed to read HTTP request: " << errorCode.message() << std::endl;
		return;
	}

	boost::beast::websocket::stream<boost::asio::ip::tcp::socket> stream(std::move(socket));

	stream.accept(req, errorCode);

	if (errorCode)
	{
		LOG_MESSAGE(CRITICAL) << "Failed to accept WebSocket connection: " << errorCode.message() << std::endl;
		return;
	}

	std::string target(req.target().begin(), req.target().end());

	std::string token;
	std::size_t pos = target.find("token=");
	if (pos != std::string::npos)
	{
		std::size_t endPos = target.find("&", pos);
		token = target.substr(pos + 6, endPos - pos - 6);
	}

	if (!isValidToken(token))
	{
		stream.close(boost::beast::websocket::close_code::policy_error, errorCode);

		LOG_MESSAGE(CRITICAL) << "Invalid token. Closing connection." << std::endl;
		return;
	}
	else
		LOG_MESSAGE(INFO) << "Accepted a new WebSocket connection." << std::endl;

	auto session = std::make_shared<WebSocketSession>(std::move(stream));
	session->start();

	doAccept();
}
