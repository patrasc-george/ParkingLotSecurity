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

    int ticketWidth = qrSize * 2;
    int ticketHeight = qrSize * 3;

    cv::Mat ticket(ticketHeight, ticketWidth, CV_8UC1, cv::Scalar(255));

    int marginTop = qrSize / 2;
    int qrXPos = (ticketWidth - qrSize) / 2;

    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            unsigned char bit = qr->data[y * size + x] & 1;
            cv::Rect roi(qrXPos + x * scale, marginTop + y * scale, scale, scale);

            if (bit)
                ticket(roi).setTo(0);
            else
                ticket(roi).setTo(255);
        }
    }

    int textYPosition = marginTop + qrSize + qrSize / 2;
    int fontFace = cv::FONT_HERSHEY_SIMPLEX;
    double fontScale = 1.0;
    int thickness = 2;

    cv::Size licensePlateSize = cv::getTextSize(licensePlate, fontFace, fontScale, thickness, nullptr);
    cv::Size dateTimeSize = cv::getTextSize(dateTime, fontFace, fontScale, thickness, nullptr);

    int licensePlateXPos = (ticketWidth - licensePlateSize.width) / 2;
    int dateTimeXPos = (ticketWidth - dateTimeSize.width) / 2;

    cv::putText(ticket, licensePlate, cv::Point(licensePlateXPos, textYPosition), fontFace, fontScale, cv::Scalar(0), thickness);

    cv::putText(ticket, dateTime, cv::Point(dateTimeXPos, textYPosition + 50), fontFace, fontScale, cv::Scalar(0), thickness);

    std::replace(text.begin(), text.end(), ':', '-');
    std::string savePath = dataBasePath + "qr/" + text + ".jpg";

    cv::imwrite(savePath, ticket);

    QRcode_free(qr);
}


std::string QRCode::decodeQR(const std::string& path)
{
	cv::Mat image = cv::imread(path, cv::IMREAD_GRAYSCALE);

	cv::QRCodeDetector qrDecoder;
	cv::Mat points;

	qrDecoder.detect(image, points);
	//std::string id = qrDecoder.decode(image, points);
	std::string id = "i";

	return id;
}
