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
	void generateQR(const int& id, const std::string& dataBasePath);

	int decodeQR(const std::string& path);
};