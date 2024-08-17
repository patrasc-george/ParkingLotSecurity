#pragma once

#ifdef QRCODEUTILS_EXPORTS
#define QRCODEUTILS_API __declspec(dllexport)
#else
#define QRCODEUTILS_API __declspec(dllimport)
#endif

#include <string>

class QRCODEUTILS_API QRCode
{
public:
	void generateQR(const std::string& dateTime, const std::string& licensePlate, const std::string& dataBasePath);

	std::string decodeQR(const std::string& path);
};