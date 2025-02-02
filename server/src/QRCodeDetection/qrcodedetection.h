#pragma once

#ifdef _WIN32
#ifdef QRCODEDETECTION_EXPORTS
#define QRCODEDETECTION_API __declspec(dllexport)
#else
#define QRCODEDETECTION_API __declspec(dllimport)
#endif
#elif __linux__
#define QRCODEDETECTION_API __attribute__((visibility("default")))
#else
#define QRCODEDETECTION_API
#endif

#include <string>
#include <vector>

class QRCODEDETECTION_API QRCode
{
public:
	std::string decodeQR(const std::vector<unsigned char>& data);
};
