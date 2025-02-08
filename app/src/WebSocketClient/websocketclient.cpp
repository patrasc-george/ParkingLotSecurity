#include "websocketclient.h"

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

WebSocketClient::WebSocketClient(boost::asio::io_context& ioContext, const std::string& uri)
	: ioContext(&ioContext),
	webSocket(std::make_unique<boost::beast::websocket::stream<boost::asio::ip::tcp::socket>>(boost::asio::make_strand(ioContext))),
	resolver(std::make_unique<boost::asio::ip::tcp::resolver>(ioContext)),
	uri(uri)
{
	auto pos = uri.find("://");
	auto address = uri.substr(pos + 3);
	auto colonPosition = address.find(':');

	host = address.substr(0, colonPosition);
	port = address.substr(colonPosition + 1);

	ioThread = std::thread([this, &ioContext]() { ioContext.run(); });
}

bool WebSocketClient::connect()
{
	if (!ioContext || !webSocket || !resolver)
	{
		std::cerr << "Client not properly initialized." << std::endl;
		return false;
	}

	std::string fullUri;
#ifdef _DEBUG
	fullUri = uri + "?token=" + "";
#else
	fullUri = uri + "?token=" + std::getenv("POSTGRES_PASSWORD");
#endif

	try
	{
		auto results = resolver->resolve(host, port);
		auto endpoint = boost::asio::connect(webSocket->next_layer(), results);

		try
		{
			boost::beast::error_code errorCode;
			webSocket->handshake(host, fullUri, errorCode);

			if (errorCode)
			{
				std::cerr << "[ERROR] WebSocket handshake failed: " << errorCode.message() << std::endl;
				return false;
			}

			webSocket->text(true);
		}
		catch (const boost::system::system_error& e)
		{
			std::cerr << "Handshake failed: " << e.what() << " [code: " << e.code() << "]" << std::endl;
			return false;
		}

		return true;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Connection failed: " << e.what() << std::endl;
		return false;
	}
}

void WebSocketClient::send(const std::string& message)
{
	boost::beast::error_code errorCode;
	webSocket->write(boost::asio::buffer(message), errorCode);

	if (errorCode)
		std::cerr << "Error sending message: " << errorCode.message() << std::endl;
}

std::string WebSocketClient::receiveSync()
{
	boost::beast::flat_buffer buffer;
	boost::beast::error_code errorCode;

	webSocket->read(buffer, errorCode);

	if (errorCode)
	{
		std::cerr << "Error receiving message: " << errorCode.message() << std::endl;
		return "";
	}

	std::string response = boost::beast::buffers_to_string(buffer.data());
	return response;
}

std::vector<std::string> WebSocketClient::getVehicles()
{
	nlohmann::json request = { {"command", "getVehicles"} };
	std::string encryptRequest = encrypt(request.dump());

	std::vector<std::string> vehicles;

	if (!webSocket->is_open())
	{
		std::cerr << "WebSocket connection is closed!" << std::endl;
		return vehicles;
	}

	send(encryptRequest);

	std::string response = receiveSync();
	if (response.empty())
	{
		std::cerr << "Received empty response!" << std::endl;
		return vehicles;
	}

	nlohmann::json decryptResponse = decrypt(response);
	if (decryptResponse.contains("vehicles"))
		for (const auto& vehicle : decryptResponse["vehicles"])
			vehicles.push_back(vehicle.get<std::string>());

	return vehicles;
}

void WebSocketClient::addVehicle(const int& id, const std::string& imagePath, const std::string& licensePlate, const std::string& dateTime, const std::string& ticket, const std::string& timeParked, const std::string& totalAmount, const std::string& isPaid)
{
	nlohmann::json request = {
		{"command", "addVehicle"},
		{"id", id},
		{"imagePath", imagePath},
		{"licensePlate", licensePlate},
		{"dateTime", dateTime},
		{"ticket", ticket},
		{"timeParked", timeParked},
		{"totalAmount", totalAmount},
		{"isPaid", isPaid}
	};
	std::string encryptRequest = encrypt(request.dump());

	send(encryptRequest);

	receiveSync();
}

bool WebSocketClient::getIsPaid(const std::string& licensePlate)
{
	nlohmann::json request = {
		{"command", "getIsPaid"},
		{"licensePlate", licensePlate},
	};
	std::string encryptRequest = encrypt(request.dump());

	send(encryptRequest);
	std::string response = receiveSync();
	if (response.empty())
	{
		std::cerr << "Received empty response!" << std::endl;
		send(encryptRequest);
		return false;
	}

	bool isPaid = false;
	nlohmann::json decryptResponse = decrypt(response);
	if (decryptResponse.contains("isPaid"))
	{
		std::cout << "Received response: " << decryptResponse << std::endl;
		isPaid = decryptResponse["isPaid"];
	}
	else
		send(encryptRequest);

	return isPaid;
}

WebSocketClient::~WebSocketClient()
{
	try
	{
		if (webSocket && webSocket->is_open())
		{
			boost::beast::error_code errorCode;
			webSocket->close(boost::beast::websocket::close_code::normal, errorCode);
		}

		if (ioThread.joinable())
			ioThread.join();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error during WebSocketClient cleanup: " << e.what() << std::endl;
	}
}
