#include "websocketserver.h"

#include <openssl/aes.h>
#include <openssl/rand.h>
#include <bitset>


std::string encrypt(const std::string& text)
{
	Logger logger;

	std::string password(std::getenv("POSTGRES_PASSWORD"));
	if (password.empty() || password.size() != 32)
	{
		LOG_MESSAGE(LogLevel::CRITICAL, LogOutput::CONSOLE) << "Invalid or missing password for encryption." << std::endl;
		return text;
	}

	const unsigned char* key = reinterpret_cast<const unsigned char*>(password.c_str());

	AES_KEY enc_key;
	if (AES_set_encrypt_key(key, 256, &enc_key) < 0)
	{
		LOG_MESSAGE(LogLevel::CRITICAL, LogOutput::CONSOLE) << "Failed to set encryption key." << std::endl;
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
			AES_encrypt(reinterpret_cast<const unsigned char*>(&paddedText[i]), block, &enc_key);

			for (int j = 0; j < 16; j++)
			{
				std::bitset<8> bits(block[j]);
				cipherText += bits.to_string();
			}
		}
	}
	catch (const std::exception& e)
	{
		LOG_MESSAGE(LogLevel::CRITICAL, LogOutput::CONSOLE) << "Error during encryption process: " << e.what() << std::endl;
		return "";
	}

	return cipherText;
}

nlohmann::json decrypt(const std::string& text)
{
	Logger logger;

	std::string password(std::getenv("POSTGRES_PASSWORD"));
	if (password.empty() || password.size() != 32)
	{
		LOG_MESSAGE(LogLevel::CRITICAL, LogOutput::CONSOLE) << "Invalid or missing password for decryption." << std::endl;
		return nlohmann::json();
	}

	const unsigned char* key = reinterpret_cast<const unsigned char*>(password.c_str());

	AES_KEY dec_key;
	if (AES_set_decrypt_key(key, 256, &dec_key) < 0)
	{
		LOG_MESSAGE(LogLevel::CRITICAL, LogOutput::CONSOLE) << "Failed to set decryption key." << std::endl;
		return nlohmann::json();
	}

	std::vector<unsigned char> cipherText;
	try
	{
		for (int i = 0; i < text.size(); i += 8)
		{
			std::bitset<8> bits(text.substr(i, 8));
			cipherText.push_back(static_cast<unsigned char>(bits.to_ulong()));
		}
	}
	catch (const std::exception& e)
	{
		LOG_MESSAGE(LogLevel::CRITICAL, LogOutput::CONSOLE) << "Error during text conversion to cipher text: " << e.what() << std::endl;
		return nlohmann::json();
	}

	std::string decryptedText;
	decryptedText.resize(cipherText.size());

	try
	{
		for (size_t i = 0; i < cipherText.size(); i += 16)
			AES_decrypt(&cipherText[i], reinterpret_cast<unsigned char*>(&decryptedText[i]), &dec_key);
	}
	catch (const std::exception& e)
	{
		LOG_MESSAGE(LogLevel::CRITICAL, LogOutput::CONSOLE) << "Error during AES decryption: " << e.what() << std::endl;
		return nlohmann::json();
	}

	size_t padding = decryptedText[decryptedText.size() - 1];
	decryptedText.resize(decryptedText.size() - padding);

	nlohmann::json jsonText;
	try
	{
		jsonText = nlohmann::json::parse(decryptedText);
	}
	catch (const std::exception& e)
	{
		LOG_MESSAGE(LogLevel::CRITICAL, LogOutput::CONSOLE) << "Failed to parse decrypted text into JSON. Error: " << e.what() << std::endl;
		return nlohmann::json();
	}

	return jsonText;
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
	{
#ifdef _DEBUG
		LOG_MESSAGE(LogLevel::DEBUG, LogOutput::TEXT_FILE) << "Error reading message from WebSocket: " << errorCode.message() << std::endl;
#endif
		return;
	}

	std::string message = boost::beast::buffers_to_string(buffer.data());
	nlohmann::json decryptMessage = decrypt(message);
	buffer.consume(buffer.size());

#ifdef _DEBUG
	LOG_MESSAGE(LogLevel::DEBUG, LogOutput::TEXT_FILE) << "Received message: " << decryptMessage.dump() << std::endl;
#endif

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
		LOG_MESSAGE(LogLevel::CRITICAL, LogOutput::CONSOLE) << "WebSocket is not open. Cannot send message." << std::endl;
		return;
	}

	if (message.empty())
	{
		LOG_MESSAGE(LogLevel::WARNING, LogOutput::CONSOLE) << "Attempted to send an empty message." << std::endl;
		return;
	}

#ifdef _DEBUG
	LOG_MESSAGE(LogLevel::DEBUG, LogOutput::TEXT_FILE) << "Sending message: " << message << std::endl;
#endif

	std::string encryptMessage = encrypt(message);
	boost::beast::error_code errorCode;
	webSocket.write(boost::asio::buffer(encryptMessage.data(), encryptMessage.size()), errorCode);

	if (errorCode)
	{
		LOG_MESSAGE(LogLevel::CRITICAL, LogOutput::CONSOLE) << "Failed to send message: " << errorCode.message() << std::endl;
		return;
	}
}

void WebSocketServer::start()
{
	doAccept();
}

bool isValidToken(const std::string& token)
{
	Logger logger;
#ifdef _DEBUG
	LOG_MESSAGE(LogLevel::DEBUG, LogOutput::TEXT_FILE) << "Validating token: " << token << std::endl;
#endif

	std::string expectedToken;
#ifdef _DEBUG
	expectedToken = std::getenv("POSTGRES_PASSWORD_DEBUG");
#else
	expectedToken = std::getenv("POSTGRES_PASSWORD");
#endif

	if (token != expectedToken)
	{
		LOG_MESSAGE(LogLevel::CRITICAL, LogOutput::CONSOLE) << "Invalid token: " << token << std::endl;
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
		LOG_MESSAGE(LogLevel::CRITICAL, LogOutput::CONSOLE) << "Failed to accept connection: " << errorCode.message() << std::endl;
		return;
	}

	boost::beast::flat_buffer buffer;
	boost::beast::http::request<boost::beast::http::string_body> req;
	boost::beast::http::read(socket, buffer, req, errorCode);

	if (errorCode)
	{
		LOG_MESSAGE(LogLevel::CRITICAL, LogOutput::CONSOLE) << "Failed to read HTTP request: " << errorCode.message() << std::endl;
		return;
	}

	boost::beast::websocket::stream<boost::asio::ip::tcp::socket> wsStream(std::move(socket));

	wsStream.accept(req, errorCode);

	if (errorCode)
	{
		LOG_MESSAGE(LogLevel::CRITICAL, LogOutput::CONSOLE) << "Failed to accept WebSocket connection: " << errorCode.message() << std::endl;
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
		wsStream.close(boost::beast::websocket::close_code::policy_error, errorCode);

		LOG_MESSAGE(LogLevel::CRITICAL, LogOutput::CONSOLE) << "Invalid token. Closing connection." << std::endl;
		return;
	}
	else
		LOG_MESSAGE(LogLevel::INFO, LogOutput::CONSOLE) << "Accepted a new WebSocket connection." << std::endl;

	auto session = std::make_shared<WebSocketSession>(std::move(wsStream));
	session->start();

	doAccept();
}
