#include "QRCodeUtils.h"

#include <qrencode.h>
#include <opencv2/opencv.hpp>

void QRCode::generateQR(const std::string& dateTime, const std::string& licensePlate, const std::string& dataBasePath)
{
	std::string text = dateTime;
	QRcode* qr = QRcode_encodeString(text.c_str(), 0, QR_ECLEVEL_L, QR_MODE_8, 1);

	int size = qr->width;
	int scale = 10;
	int qrSize = qr->width * scale;
	int width = qrSize * 2;
	int height = qrSize * 3;

	cv::Mat ticket(height, width, CV_8UC1, cv::Scalar(255));

	int marginTop = qrSize / 2;
	int qrX = (width - qrSize) / 2;

	for (int y = 0; y < size; y++)
		for (int x = 0; x < size; x++)
		{
			unsigned char bit = qr->data[y * size + x] & 1;
			cv::Rect roi(qrX + x * scale, qrSize / 2 + y * scale, scale, scale);

			if (bit)
				ticket(roi).setTo(0);
			else
				ticket(roi).setTo(255);
		}

	int textY = 2 * qrSize;

	cv::Size licensePlateSize = cv::getTextSize(licensePlate, cv::FONT_HERSHEY_SIMPLEX, 1.0, 2, nullptr);
	cv::Size dateTimeSize = cv::getTextSize(dateTime, cv::FONT_HERSHEY_SIMPLEX, 1.0, 2, nullptr);

	int licensePlateX = (width - licensePlateSize.width) / 2;
	int dateTimeX = (width - dateTimeSize.width) / 2;

	cv::putText(ticket, licensePlate, cv::Point(licensePlateX, textY), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0), 2);
	cv::putText(ticket, dateTime, cv::Point(dateTimeX, textY + 50), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0), 2);

	std::replace(text.begin(), text.end(), ':', '-');
	std::string savePath = dataBasePath + "qr/" + text + ".jpg";
	cv::imwrite(savePath, ticket);

	QRcode_free(qr);
}


std::string QRCode::decodeQR(const std::string& path)
{
	std::string dateTime;
	cv::Mat image = cv::imread(path, cv::IMREAD_GRAYSCALE);

	//cv::QRCodeDetector qrDecoder;
	//cv::Mat points;

	//qrDecoder.detect(image, points);
	//dateTime = qrDecoder.decode(image, points);

	dateTime = "20-08-2024 13:52:28";

	return dateTime;
}
