#include "websocketserver.h"

#include <openssl/aes.h>
#include <openssl/rand.h>
#include <bitset>

std::string encrypt(const std::string& text)
{
	std::string password(std::getenv("POSTGRES_PASSWORD"));
	if (password.empty() || password.size() != 32)
		return text;

	const unsigned char* key = reinterpret_cast<const unsigned char*>(password.c_str());

	AES_KEY enc_key;
	if (AES_set_encrypt_key(key, 256, &enc_key) < 0)
		return "";

	int padding = 16 - (text.size() % 16);
	std::string paddedText = text + std::string(padding, '\0');

	std::string cipherText;
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

	return cipherText;
}

nlohmann::json decrypt(const std::string& text)
{
	std::string password(std::getenv("POSTGRES_PASSWORD"));
	if (password.empty() || password.size() != 32)
		return nlohmann::json();

	const unsigned char* key = reinterpret_cast<const unsigned char*>(password.c_str());

	AES_KEY dec_key;
	if (AES_set_decrypt_key(key, 256, &dec_key) < 0)
		return nlohmann::json();

	std::vector<unsigned char> cipherText;
	for (int i = 0; i < text.size(); i += 8)
	{
		std::bitset<8> bits(text.substr(i, 8));
		cipherText.push_back(static_cast<unsigned char>(bits.to_ulong()));
	}

	std::string decryptedText;
	decryptedText.resize(cipherText.size());

	for (size_t i = 0; i < cipherText.size(); i += 16)
		AES_decrypt(&cipherText[i], reinterpret_cast<unsigned char*>(&decryptedText[i]), &dec_key);

	size_t padding = decryptedText[decryptedText.size() - 1];
	decryptedText.resize(decryptedText.size() - padding);

	nlohmann::json jsonText;
	try {
		jsonText = nlohmann::json::parse(decryptedText);
	}
	catch (const std::exception& error) {
		std::cerr << "Error parsing JSON: " << error.what() << std::endl;
	}

	return jsonText;
}

void WebSocketSession::start()
{
	//std::cout << "[DEBUG] Starting WebSocket session." << std::endl;
	//boost::beast::error_code errorCode;

	//webSocket.accept(errorCode);

	//if (errorCode)
	//{
	//	std::cerr << "[ERROR] Accept failed: " << errorCode.message() << std::endl;
	//	return;
	//}

	//std::cout << "[DEBUG] WebSocket connection established." << std::endl;
	doRead();
}

void WebSocketSession::doRead()
{
	boost::beast::error_code errorCode;
	webSocket.binary(true);
	webSocket.read(buffer, errorCode);

	if (errorCode)
	{
		std::cerr << "[ERROR] Read failed: " << errorCode.message() << std::endl;
		return;
	}

	std::string message = boost::beast::buffers_to_string(buffer.data());
	nlohmann::json decryptMessage = decrypt(message);

	std::cout << "[DEBUG] Received message: " << decryptMessage << std::endl;

	buffer.consume(buffer.size());

	std::string command = decryptMessage["command"];
	std::cout << "[DEBUG] Command received: " << command << std::endl;

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
		std::cerr << "[ERROR] WebSocket is not open!" << std::endl;
		return;
	}

	if (message.empty())
	{
		std::cerr << "[ERROR] Message is empty, nothing to send!" << std::endl;
		return;
	}

	std::string encryptMessage = encrypt(message);
	boost::beast::error_code errorCode;
	webSocket.write(boost::asio::buffer(encryptMessage.data(), encryptMessage.size()), errorCode);

	if (errorCode)
		std::cerr << "[ERROR] Write failed: " << errorCode.message() << " [Code: " << errorCode.value() << "]" << std::endl;
	else
		std::cout << "[DEBUG] Sent: " << message << std::endl << std::endl;
}

void WebSocketServer::start()
{
	std::cout << "[DEBUG] WebSocket server starting on port " << acceptor.local_endpoint().port() << std::endl;
	doAccept();
}

bool isValidToken(const std::string& token)
{
	std::string expectedToken;
#ifdef _DEBUG
	expectedToken = std::getenv("POSTGRES_PASSWORD_DEBUG");
#else
	expectedToken = std::getenv("POSTGRES_PASSWORD");
#endif

	return token == expectedToken;
}

void WebSocketServer::doAccept()
{
	std::cout << "[DEBUG] Waiting for new connections..." << std::endl;
	boost::beast::error_code errorCode;
	boost::asio::ip::tcp::socket socket(ioContext);

	acceptor.accept(socket, errorCode);

	if (errorCode)
	{
		std::cerr << "[ERROR] Accept failed: " << errorCode.message() << std::endl;
		return;
	}

	std::cout << "[DEBUG] New connection accepted." << std::endl;

	boost::beast::flat_buffer buffer;
	boost::beast::http::request<boost::beast::http::string_body> req;
	boost::beast::http::read(socket, buffer, req, errorCode);

	if (errorCode)
	{
		std::cerr << "[ERROR] Failed to read HTTP request: " << errorCode.message() << std::endl;
		return;
	}

	boost::beast::websocket::stream<boost::asio::ip::tcp::socket> wsStream(std::move(socket));

	// Here we directly call `accept` with the request (req) object
	wsStream.accept(req, errorCode);

	if (errorCode)
	{
		std::cerr << "[ERROR] WebSocket handshake failed: " << errorCode.message() << std::endl;
		return;
	}

	std::string target(req.target().begin(), req.target().end());
	std::cout << "[DEBUG] Client connected with URI: " << target << std::endl;

	std::string token;
	std::size_t pos = target.find("token=");
	if (pos != std::string::npos)
	{
		std::size_t endPos = target.find("&", pos);
		token = target.substr(pos + 6, endPos - pos - 6);
	}

	std::string password;
#ifdef _DEBUG
	password = std::getenv("POSTGRES_PASSWORD_DEBUG");
#else
	password = std::getenv("POSTGRES_PASSWORD");
#endif

	if (token != password)
	{
		std::cerr << "[ERROR] Unauthorized WebSocket connection! Invalid token: " << token << std::endl;
		wsStream.close(boost::beast::websocket::close_code::policy_error, errorCode);
		return;
	}

	std::cout << "[DEBUG] WebSocket connection authorized." << std::endl;

	auto session = std::make_shared<WebSocketSession>(std::move(wsStream));
	session->start();

	doAccept();
}
