#include "qrcodedetection.h"

#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>
#include <regex>

std::string formatString(const std::string& str)
{
	std::string result;
	bool firstNum = true, firstLetter = true;

	for (int i = 0; i < str.size(); i++)
	{
		if (isdigit(str[i]) && firstNum)
		{
			result += ' ';
			firstNum = false;
		}
		if (isalpha(str[i]) && !firstNum && firstLetter)
		{
			result += ' ';
			firstLetter = false;
		}
		result += str[i];
	}

	return result;
}

void write(cv::Mat& image, int x, int& y, const int& margin, const std::string& leftText, const std::string& rightText, const float& fontScale, const int& thickness)
{
	cv::Size textSize;

	textSize = cv::getTextSize(leftText, cv::FONT_HERSHEY_SIMPLEX, fontScale, thickness, nullptr);
	y = y + textSize.height;
	cv::putText(image, leftText, cv::Point(margin, y), cv::FONT_HERSHEY_SIMPLEX, fontScale, cv::Scalar(0), thickness);

	textSize = cv::getTextSize(rightText, cv::FONT_HERSHEY_SIMPLEX, fontScale, thickness, nullptr);
	x = x - textSize.width;
	cv::putText(image, rightText, cv::Point(x, y), cv::FONT_HERSHEY_SIMPLEX, fontScale, cv::Scalar(0), thickness);
}

void QRCode::generateQR(const std::string& id, const std::string& name, const std::string& licensePlate, const std::string& dataBasePath, const std::string& assetsPath, const std::string& dateTime, const std::string& timeParked, const int& totalAmount)
{
	std::regex pattern(R"((\d{2})-(\d{2})-(\d{4}) (\d{2}):(\d{2}):(\d{2}))");
	std::smatch match;

	std::string text;
	if (std::regex_match(id, match, pattern))
		text = match[1].str() + match[2].str() + match[3].str().substr(2) + match[4].str() + match[5].str() + match[6].str();

	cv::QRCodeEncoder::Params params;
	params.correction_level = cv::QRCodeEncoder::CorrectionLevel::CORRECT_LEVEL_L;
	params.mode = cv::QRCodeEncoder::EncodeMode::MODE_AUTO;
	cv::Ptr<cv::QRCodeEncoder> encoder = cv::QRCodeEncoder::create(params);
	cv::Mat qr;
	encoder->encode(text, qr);

	int qrScale = 30;
	int qrSize = qr.cols * qrScale;
	int width = qrSize * 2;
	int height = qrSize * 4;
	int margin = width * 0.1;
	int textScale = 20;
	int lineSpacing = textScale * 3;
	float fontScale = textScale / 10.0;
	int thickness = textScale / 3;

	cv::Mat ticket(height, width, CV_8UC1, cv::Scalar(255));

	cv::Mat logo = cv::imread(assetsPath + "park.png", cv::IMREAD_GRAYSCALE);
	int logoWidth = width / 5;
	int logoHeight = ((float)logoWidth / logo.cols) * logo.rows;
	cv::resize(logo, logo, cv::Size(logoWidth, logoHeight));
	logo.copyTo(ticket(cv::Rect(margin, margin, logoWidth, logoHeight)));

	cv::Size textSize = cv::getTextSize(name, cv::FONT_HERSHEY_SIMPLEX, fontScale, thickness, nullptr);
	int x = width - (margin + textSize.width);
	int y = margin + logoHeight / 2 + textSize.height / 2;
	cv::putText(ticket, name, cv::Point(x, y), cv::FONT_HERSHEY_SIMPLEX, fontScale, cv::Scalar(0), thickness);

	y = margin + logoHeight + lineSpacing;
	cv::line(ticket, cv::Point(margin, y), cv::Point(width - margin, y), cv::Scalar(0), thickness);

	textSize = cv::getTextSize("PARKING RECEIPT", cv::FONT_HERSHEY_SIMPLEX, fontScale, thickness, nullptr);
	y = y + lineSpacing + textSize.height;
	x = width / 2 - textSize.width / 2;
	cv::putText(ticket, "PARKING RECEIPT", cv::Point(x, y), cv::FONT_HERSHEY_SIMPLEX, fontScale, cv::Scalar(0), thickness);

	y = y + lineSpacing;
	cv::line(ticket, cv::Point(margin, y), cv::Point(width - margin, y), cv::Scalar(0), thickness);

	x = (width - qrSize) / 2;
	y = y + lineSpacing * 2;

	for (int j = 0; j < qr.rows; j++)
	{
		for (int i = 0; i < qr.cols; i++)
		{
			bool bit = (qr.ptr<uchar>(j, i)[0] == 0);
			cv::Rect roi(x + i * qrScale, y + j * qrScale, qrScale, qrScale);

			if (bit)
				ticket(roi).setTo(0);
			else
				ticket(roi).setTo(255);
		}
	}

	y = y + qrSize + lineSpacing * 2;
	x = width - margin;
	std::string formattedLicensePlate = formatString(licensePlate);
	write(ticket, x, y, margin, "License plate:", formattedLicensePlate, fontScale, thickness);

	y = y + lineSpacing;
	write(ticket, x, y, margin, "Entered:", id, fontScale, thickness);

	if (!dateTime.empty() && !timeParked.empty() && totalAmount != 0)
	{
		y = y + lineSpacing;
		write(ticket, x, y, margin, "Exit:", dateTime, fontScale, thickness);

		y = y + lineSpacing;
		write(ticket, x, y, margin, "Time parked:", timeParked, fontScale, thickness);

		y = y + lineSpacing;
		write(ticket, x, y, margin, "Paid:", std::to_string(totalAmount) + " RON", fontScale, thickness);

		text += " exit.jpg";
	}
	else
	{
		y = y + lineSpacing;
		write(ticket, x, y, margin, "", "", fontScale, thickness);

		y = y + lineSpacing;
		write(ticket, x, y, margin, "", "", fontScale, thickness);

		y = y + lineSpacing;
		write(ticket, x, y, margin, "", "", fontScale, thickness);

		text += " entered.jpg";
	}

	y = y + lineSpacing;
	cv::line(ticket, cv::Point(margin, y), cv::Point(width - margin, y), cv::Scalar(0), thickness);

	textSize = cv::getTextSize("THANK YOU!", cv::FONT_HERSHEY_SIMPLEX, fontScale, thickness, nullptr);
	y = y + lineSpacing + textSize.height;
	x = width / 2 - textSize.width / 2;
	cv::putText(ticket, "THANK YOU!", cv::Point(x, y), cv::FONT_HERSHEY_SIMPLEX, fontScale, cv::Scalar(0), thickness);

	y = y + margin;
	ticket = ticket(cv::Rect(0, 0, ticket.cols, y));

	std::replace(text.begin(), text.end(), ':', '-');
	std::replace(text.begin(), text.end(), ' ', '_');
	std::string savePath = dataBasePath + "tickets/" + text;
	cv::imwrite(savePath, ticket);
}

std::string QRCode::decodeQR(const std::string& path)
{
	std::string dateTime;
	cv::Mat image = cv::imread(path, cv::IMREAD_GRAYSCALE);

	return dateTime;
}
