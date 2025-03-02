#include "websocketclient.h"

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

WebSocketClient::WebSocketClient(boost::asio::io_context& ioContext, const std::string& uri)
	: ioContext(&ioContext),
	webSocket(std::make_unique<boost::beast::websocket::stream<boost::asio::ip::tcp::socket>>(boost::asio::make_strand(ioContext))),
	resolver(std::make_unique<boost::asio::ip::tcp::resolver>(ioContext)),
	uri(uri),
	logger(Logger::getInstance())

{
	logger.setLogOutput(TEXT_FILE);

	LOG_MESSAGE(INFO) << "The program has been launched." << std::endl;

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
		LOG_MESSAGE(CRITICAL) << "Client not properly initialized." << std::endl;
		return false;
	}

	std::string fullUri;
#ifdef _DEBUG
	fullUri = uri + "?token=" + std::getenv("POSTGRES_PASSWORD_DEBUG");
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
				LOG_MESSAGE(CRITICAL) << "WebSocket handshake failed: " << errorCode.message() << std::endl;
				return false;
			}

			webSocket->text(true);

			LOG_MESSAGE(INFO) << "Successfully connected to WebSocket server on port: " << port << std::endl;
		}
		catch (const boost::system::system_error& e)
		{
			LOG_MESSAGE(CRITICAL) << "Handshake failed: " << e.what() << " [code: " << std::to_string(e.code().value()) << "]" << std::endl;
			return false;
		}

		return true;
	}
	catch (const std::exception& e)
	{
		LOG_MESSAGE(CRITICAL) << "Connection failed: " << e.what() << std::endl;
		return false;
	}
}

void WebSocketClient::send(const std::string& message)
{
	boost::beast::error_code errorCode;
	webSocket->write(boost::asio::buffer(message), errorCode);

	if (errorCode)
		LOG_MESSAGE(CRITICAL) << "Error sending message: " << errorCode.message() << std::endl;
}

std::string WebSocketClient::receiveSync()
{
	boost::beast::flat_buffer buffer;
	boost::beast::error_code errorCode;

	webSocket->read(buffer, errorCode);

	if (errorCode)
	{
		LOG_MESSAGE(CRITICAL) << "Error receiving message: " << errorCode.message() << std::endl;
		return "";
	}

	std::string response = boost::beast::buffers_to_string(buffer.data());

	return response;
}

std::vector<std::string> WebSocketClient::getVehicles()
{
	nlohmann::json request = { {"command", "getVehicles"} };
	std::string encryptRequest = encrypt(request.dump());
	nlohmann::json dec = decrypt(encryptRequest);
	std::string debug = dec.dump();
	std::vector<std::string> vehicles;

	if (!webSocket->is_open())
	{
		LOG_MESSAGE(CRITICAL) << "WebSocket connection is closed." << std::endl;
		return vehicles;
	}

	LOG_MESSAGE(DEBUG) << "Sending request to get vehicles." << std::endl;
	send(encryptRequest);

	std::string response = receiveSync();
	if (response.empty())
	{
		LOG_MESSAGE(CRITICAL) << "Received empty response!" << std::endl;
		return vehicles;
	}

	nlohmann::json decryptResponse = decrypt(response);
	LOG_MESSAGE(DEBUG) << "Received response." << std::endl;

	if (decryptResponse.contains("vehicles"))
		for (const auto& vehicle : decryptResponse["vehicles"])
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
	LOG_MESSAGE(DEBUG) << "Sending request to add vehicle." << std::endl;

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
	LOG_MESSAGE(DEBUG) << "Sending request to check if vehicle is paid." << std::endl;

	std::string encryptRequest = encrypt(request.dump());
	send(encryptRequest);

	std::string response = receiveSync();
	if (response.empty())
	{
		LOG_MESSAGE(CRITICAL) << "Received empty response!" << std::endl;
		send(encryptRequest);
		return false;
	}

	bool isPaid = false;
	nlohmann::json decryptResponse = decrypt(response);

	LOG_MESSAGE(DEBUG) << "Received response." << std::endl;

	if (decryptResponse.contains("isPaid"))
		isPaid = decryptResponse["isPaid"];
	else
	{
		LOG_MESSAGE(CRITICAL) << "'isPaid' not found in the response!" << std::endl;
		send(encryptRequest);
	}

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

			if (errorCode)
				LOG_MESSAGE(CRITICAL) << "Error closing WebSocket: " << errorCode.message() << std::endl;
			else
				LOG_MESSAGE(INFO) << "WebSocket connection closed gracefully." << std::endl;
		}

		if (ioThread.joinable())
		{
			ioThread.join();
			LOG_MESSAGE(INFO) << "I/O thread joined." << std::endl;
		}
	}
	catch (const std::exception& e)
	{
		LOG_MESSAGE(CRITICAL) << "Error during WebSocketClient cleanup: " << e.what() << std::endl;
	}

	LOG_MESSAGE(INFO) << "The program has been closed." << std::endl << std::endl << std::endl << std::endl;
}
