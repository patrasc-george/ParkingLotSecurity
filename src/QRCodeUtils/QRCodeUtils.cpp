#include "QRCodeUtils.h"

#include <qrencode.h>
#include <opencv2/opencv.hpp>

void QRCode::generateQR(const std::string& id, const std::string& name, const std::string& licensePlate, const std::string& dataBasePath, const std::string& dateTime, const std::string& timeParked, const int& totalAmount)
{
	std::string text = id;
	QRcode* qr = QRcode_encodeString(text.c_str(), 0, QR_ECLEVEL_L, QR_MODE_8, 1);

	int size = qr->width;
	int scale = 30;
	int qrSize = qr->width * scale;
	int width = qrSize * 2;
	int height = qrSize * 3;

	cv::Mat ticket(height, width, CV_8UC1, cv::Scalar(255));

	int qrX = (width - qrSize) / 2;
	int qrY = qrSize / 2;

	for (int y = 0; y < size; y++)
	{
		for (int x = 0; x < size; x++)
		{
			unsigned char bit = qr->data[y * size + x] & 1;
			cv::Rect roi(qrX + x * scale, qrY + y * scale, scale, scale);

			if (bit)
				ticket(roi).setTo(0);
			else
				ticket(roi).setTo(255);
		}
	}

	int textY = 2 * qrSize;
	int lineSpacing = scale * 4;
	double fontScale = scale / 10.0;
	int thickness = scale / 3;

	cv::Size nameSize = cv::getTextSize(name, cv::FONT_HERSHEY_SIMPLEX, fontScale * 1.5, thickness * 1.5, nullptr);
	int nameX = (width - nameSize.width) / 2;
	cv::putText(ticket, name, cv::Point(nameX, qrY / 2), cv::FONT_HERSHEY_SIMPLEX, fontScale * 1.5, cv::Scalar(0), thickness * 1.5);

	cv::Size licensePlateSize = cv::getTextSize(licensePlate, cv::FONT_HERSHEY_SIMPLEX, fontScale, thickness, nullptr);
	int licensePlateX = (width - licensePlateSize.width) / 2;
	cv::putText(ticket, licensePlate, cv::Point(licensePlateX, textY), cv::FONT_HERSHEY_SIMPLEX, fontScale, cv::Scalar(0), thickness);

	if (dateTime.empty() && timeParked.empty() && totalAmount == 0)
	{
		cv::Size idSize = cv::getTextSize(id, cv::FONT_HERSHEY_SIMPLEX, fontScale, thickness, nullptr);
		int idX = (width - idSize.width) / 2;
		cv::putText(ticket, id, cv::Point(idX, textY + lineSpacing), cv::FONT_HERSHEY_SIMPLEX, fontScale, cv::Scalar(0), thickness);

		text += " entered.jpg";
	}
	else
	{
		cv::Size dateTimeSize = cv::getTextSize(dateTime, cv::FONT_HERSHEY_SIMPLEX, fontScale, thickness, nullptr);
		int dateTimeX = (width - dateTimeSize.width) / 2;
		cv::putText(ticket, dateTime, cv::Point(dateTimeX, textY + lineSpacing), cv::FONT_HERSHEY_SIMPLEX, fontScale, cv::Scalar(0), thickness);

		cv::Size timeParkedSize = cv::getTextSize(timeParked, cv::FONT_HERSHEY_SIMPLEX, fontScale, thickness, nullptr);
		int timeParkedX = (width - timeParkedSize.width) / 2;
		cv::putText(ticket, timeParked, cv::Point(timeParkedX, textY + lineSpacing * 2), cv::FONT_HERSHEY_SIMPLEX, fontScale, cv::Scalar(0), thickness);

		std::string totalAmountString = std::to_string(totalAmount) + " RON";
		cv::Size totalAmountSize = cv::getTextSize(totalAmountString, cv::FONT_HERSHEY_SIMPLEX, fontScale, thickness, nullptr);
		int totalAmountX = (width - totalAmountSize.width) / 2;
		cv::putText(ticket, totalAmountString, cv::Point(totalAmountX, textY + lineSpacing * 3), cv::FONT_HERSHEY_SIMPLEX, fontScale, cv::Scalar(0), thickness);

		text += " exit.jpg";
	}

	std::replace(text.begin(), text.end(), ':', '-');
	std::replace(text.begin(), text.end(), ' ', '_');
	std::string savePath = dataBasePath + "qr/" + text;
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
