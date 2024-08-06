#include "QRCodeUtils.h"

#include <qrencode.h>
#include <opencv2/opencv.hpp>

void QRCode::generateQR(const int& id, const std::string& dataBasePath)
{
	std::string text = std::to_string(id);
	QRcode* qr = QRcode_encodeString(text.c_str(), 0, QR_ECLEVEL_L, QR_MODE_8, 1);

	int size = qr->width;
	int scale = 10;
	int scaledSize = qr->width * 10;

	cv::Mat image(scaledSize, scaledSize, CV_8UC1, cv::Scalar(255));

	for (int y = 0; y < size; y++)
		for (int x = 0; x < size; x++)
		{
			unsigned char bit = qr->data[y * size + x] & 1;
			cv::Rect roi(x * scale, y * scale, scale, scale);

			if (bit)
				image(roi).setTo(0);
			else
				image(roi).setTo(255);
		}

	std::string savePath = dataBasePath + "qr/" + text + ".jpg";
	cv::imwrite(savePath, image);

	QRcode_free(qr);
}

int QRCode::decodeQR(const std::string& path)
{
	cv::Mat image = cv::imread(path, cv::IMREAD_GRAYSCALE);

	cv::QRCodeDetector qrDecoder;
	cv::Mat points;

	qrDecoder.detect(image, points);
	//std::string id = qrDecoder.decode(image, points);
	std::string id = "192";

	return std::stoi(id);
}
