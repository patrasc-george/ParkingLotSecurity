#pragma once

#ifdef QRCODEDETECTION_EXPORTS
#define QRCODEDETECTION_API __declspec(dllexport)
#else
#define QRCODEDETECTION_API __declspec(dllimport)
#endif

#include <string>

class QRCODEDETECTION_API QRCode
{
public:
	/**
	 * @brief Generates a QR code containing vehicle information and saves it as an image.
	 * @details This function generates a QR code based on the provided information such as
	 *          vehicle ID, name, license plate, date/time, and parking duration. The QR code is
	 *          rendered onto an image, and additional information (such as the vehicle's name,
	 *          license plate, date/time, and total amount) is added as text. The final image is
	 *          saved in the specified database path with a formatted filename.
	 * @param[in] id The vehicle's unique identifier.
	 * @param[in] name The vehicle owner's name.
	 * @param[in] licensePlate The vehicle's license plate number.
	 * @param[in] dataBasePath The base directory path where the QR code image will be saved.
	 * @param[in] dateTime The date and time the vehicle entered the parking lot.
	 * @param[in] timeParked The total time the vehicle was parked.
	 * @param[in] totalAmount The total amount for parking, in RON.
	 * @return void
	 */
	void generateQR(const std::string& id, const std::string& name, const std::string& licensePlate, const std::string& dataBasePath, const std::string& dateTime = "", const std::string& timeParked = "", const int& totalAmount = 0);

	std::string decodeQR(const std::string& path);
};