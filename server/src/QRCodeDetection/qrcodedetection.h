#pragma once

#ifdef QRCODEDETECTION_EXPORTS
#define QRCODEDETECTION_API __declspec(dllexport)
#else
#define QRCODEDETECTION_API __declspec(dllimport)
#endif

#include <string>
#include <vector>

class QRCODEDETECTION_API QRCode
{
public:
	void generateQR(const std::string& id, const std::string& name, const std::string& licensePlate, const std::string& dataBasePath, const std::string& dateTime = "", const std::string& timeParked = "", const int& totalAmount = 0);

	std::string decodeQR(const std::vector<unsigned char>& data);
};