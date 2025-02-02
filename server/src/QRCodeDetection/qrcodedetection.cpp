#include "qrcodedetection.h"

#include <qrencode.h>
#include <opencv2/opencv.hpp>

std::string QRCode::decodeQR(const std::vector<unsigned char>& data)
{
	std::string dateTime;
	cv::Mat image = cv::imdecode(data, cv::IMREAD_COLOR);

	return dateTime;
}
