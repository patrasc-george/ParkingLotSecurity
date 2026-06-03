#include "qrcodedetection.h"

#include <opencv2/core/utils/logger.hpp>
#include <ZXing/ReadBarcode.h>
#include <ZXing/BarcodeFormat.h>
#include <ZXing/DecodeHints.h>
#include <ZXing/ImageView.h>
#include <regex>

QRCode::QRCode()
{
	cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_SILENT);

#ifdef _DEBUG
	aiModel = cv::dnn::readNetFromONNX("../../../qrbitnet.onnx");
#else
	aiModel = cv::dnn::readNetFromONNX("qrbitnet.onnx");
#endif
}

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

void QRCode::generateQR(const std::string& id, const std::string& name, const std::string& licensePlate, const std::string& dataBasePath, const std::string& assetsPath, std::string& savePath, const std::string& dateTime, const std::string& timeParked, const int& totalAmount)
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

	int margin = 10;

	int qrScale = 40;
	int qrSize = qr.cols * qrScale;

	int width = qrSize + margin * 2;
	int height = qrSize * 4;

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

	x = margin;
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

	savePath = dataBasePath + "tickets/" + text;
	cv::imwrite(savePath, ticket);
}

void QRCode::resize(const cv::Mat& src, cv::Mat& dst, const int& max)
{
	if (src.rows > src.cols && src.rows > max)
	{
		int width = ((float)max / src.rows) * src.cols;
		cv::resize(src, dst, cv::Size(width, max));
	}
	else if (src.cols > src.rows && src.cols > max)
	{
		int height = ((float)max / src.cols) * src.rows;
		cv::resize(src, dst, cv::Size(max, height));
	}
	else if (src.cols == src.rows && src.cols > max)
	{
		cv::resize(src, dst, cv::Size(max, max));
	}
	else
		dst = src.clone();
}

void QRCode::binarySobel(const cv::Mat& src, cv::Mat& dst, cv::Mat& direction)
{
	if (src.empty() || src.type() != CV_8UC1)
		return;

	cv::Mat x, y, magnitude;

	cv::Sobel(src, x, CV_32F, 1, 0);
	cv::Sobel(src, y, CV_32F, 0, 1);

	cv::cartToPolar(x, y, magnitude, direction, true);
	cv::normalize(magnitude, magnitude, 0, 255, cv::NORM_MINMAX);

	cv::Mat normalizedMagnitude, binaryMagnitude;
	magnitude.convertTo(normalizedMagnitude, CV_8UC1);
	magnitude.convertTo(binaryMagnitude, CV_8UC1);

	cv::threshold(binaryMagnitude, binaryMagnitude, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
	cv::bitwise_and(normalizedMagnitude, binaryMagnitude, dst);
}

void QRCode::nonMaximumSuppression(const cv::Mat& src, cv::Mat& dst, const cv::Mat& directions)
{
	if (src.empty() || src.type() != CV_8UC1)
		return;

	if (directions.empty() || directions.type() != CV_32FC1)
		return;

	if (src.size() != directions.size())
		return;

	for (int y = 2; y < src.rows - 2; y++)
		for (int x = 2; x < src.cols - 2; x++)
		{
			float direction = fmod(directions.ptr<float>(y, x)[0], 180);

			uchar pixel = src.ptr<uchar>(y, x)[0];
			uchar pixel1, pixel2, pixel3, pixel4;

			if ((direction >= 0 && direction < 22.5) || (direction >= 157.5 && direction <= 180))
			{
				pixel1 = src.ptr<uchar>(y, x - 1)[0];
				pixel2 = src.ptr<uchar>(y, x - 2)[0];
				pixel3 = src.ptr<uchar>(y, x + 1)[0];
				pixel4 = src.ptr<uchar>(y, x + 2)[0];
			}
			else if (direction >= 22.5 && direction < 67.5)
			{
				pixel1 = src.ptr<uchar>(y - 1, x - 1)[0];
				pixel2 = src.ptr<uchar>(y - 2, x - 2)[0];
				pixel3 = src.ptr<uchar>(y + 1, x + 1)[0];
				pixel4 = src.ptr<uchar>(y + 2, x + 2)[0];
			}
			else if (direction >= 67.5 && direction < 112.5)
			{
				pixel1 = src.ptr<uchar>(y - 1, x)[0];
				pixel2 = src.ptr<uchar>(y - 2, x)[0];
				pixel3 = src.ptr<uchar>(y + 1, x)[0];
				pixel4 = src.ptr<uchar>(y + 2, x)[0];
			}
			else if (direction >= 112.5 && direction <= 157.5)
			{
				pixel1 = src.ptr<uchar>(y - 1, x + 1)[0];
				pixel2 = src.ptr<uchar>(y - 2, x + 2)[0];
				pixel3 = src.ptr<uchar>(y + 1, x - 1)[0];
				pixel4 = src.ptr<uchar>(y + 2, x - 2)[0];
			}

			if (!(pixel < pixel1 || pixel <= pixel2 || pixel <= pixel3 || pixel <= pixel4))
				dst.ptr<uchar>(y, x)[0] = 255;
		}
}

void QRCode::edgeDetection(const cv::Mat& src, cv::Mat& dst)
{
	if (src.empty() || src.type() != CV_8UC1)
		return;

	dst = cv::Mat::zeros(src.size(), src.type());

	cv::Mat sobel, direction;
	binarySobel(src, sobel, direction);

	nonMaximumSuppression(sobel, dst, direction);
}

bool QRCode::isQuadrilateral(const std::vector<cv::Point>& contour)
{
	if (cv::contourArea(contour) < 10.0)
		return false;

	std::vector<cv::Point> approx;
	double perimeter = cv::arcLength(contour, true);
	cv::approxPolyDP(contour, approx, 0.02 * perimeter, true);

	return approx.size() == 4;
}

bool QRCode::detectQRAnchors(const cv::Mat& binary, std::vector<std::vector<cv::Point>>& anchors)
{
	cv::Mat dilated;
	cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
	cv::morphologyEx(binary, dilated, cv::MORPH_CLOSE, kernel);

	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(dilated, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

	std::vector<std::pair<float, int>> scores;
	for (int i = 0; i < contours.size(); i++)
	{
		if (hierarchy[i][3] != -1)
			continue;

		if (!isQuadrilateral(contours[i]))
			continue;

		float sum = 0;
		int comparisons = 0;
		int current = i;
		int depth = 0;
		while (current != -1)
		{
			int child = hierarchy[current][2];
			int bestChild = -1;
			float bestArea = -1;
			while (child != -1)
			{
				float areaChild = cv::contourArea(contours[child]);
				if (areaChild > bestArea)
				{
					bestArea = areaChild;
					bestChild = child;
				}
				child = hierarchy[child][0];
			}

			if (bestChild != -1)
			{
				float similarity = cv::matchShapes(contours[current], contours[bestChild], cv::CONTOURS_MATCH_I2, 0);
				sum += similarity;
				comparisons++;
			}

			current = bestChild;
			depth++;
		}

		if (depth < 3)
			continue;

		float mean = comparisons > 0 ? sum / comparisons : DBL_MAX;
		scores.push_back({ mean, i });
	}

	if (scores.size() < 3)
		return false;

	std::sort(scores.begin(), scores.end());

	for (int i = 0; i < 3; i++)
	{
		int index = scores[i].second;
		anchors.push_back(contours[index]);
	}

	return true;
}

cv::Point2f QRCode::getContourCentroid(const std::vector<cv::Point>& contour)
{
	cv::Moments moments = cv::moments(contour);

	if (std::abs(moments.m00) < std::numeric_limits<double>::epsilon())
		return cv::Point2f();

	return cv::Point2f(static_cast<float>(moments.m10 / moments.m00), static_cast<float>(moments.m01 / moments.m00));
}

void QRCode::sortAnchors(std::vector<std::vector<cv::Point>>& anchors)
{
	std::vector<cv::Point2f> centroids;

	for (const auto& anchor : anchors)
		centroids.push_back(getContourCentroid(anchor));

	int topLeft = 0;
	int topRight = 0;
	int bottomLeft = 0;

	float minSum = FLT_MAX;
	float maxDifference = -FLT_MAX;
	float minDifference = FLT_MAX;

	for (int i = 0; i < 3; i++)
	{
		cv::Point2f centroid = centroids[i];
		float sum = centroid.x + centroid.y;
		float difference = centroid.x - centroid.y;

		if (sum < minSum)
		{
			minSum = sum;
			topLeft = i;
		}

		if (difference > maxDifference)
		{
			maxDifference = difference;
			topRight = i;
		}

		if (difference < minDifference)
		{
			minDifference = difference;
			bottomLeft = i;
		}
	}

	anchors =
	{
		anchors[topLeft],
		anchors[topRight],
		anchors[bottomLeft]
	};
}

void QRCode::drawAnchor(const cv::Mat& src, cv::Mat& dst, const std::vector<std::vector<cv::Point>>& contours)
{
	cv::cvtColor(src, dst, cv::COLOR_GRAY2BGR);

	std::vector<cv::Scalar> colors = {
		cv::Scalar(0, 255, 0),
		cv::Scalar(255, 0, 0),
		cv::Scalar(0, 0, 255)
	};

	for (int i = 0; i < contours.size(); i++)
	{
		cv::Scalar color = colors[i % colors.size()];
		cv::drawContours(dst, contours, i, color, 2);
	}
}

void QRCode::lineThroughPoint(cv::Vec4f& line, const double& slope, const cv::Point& point, const bool& direction)
{
	if (point.x < 0 || point.y < 0)
		return;

	cv::Size size(point.x * 2, point.y * 2);
	double b = point.y - slope * point.x;

	cv::Point startPoint, endPoint;
	if (direction)
	{
		startPoint.x = 0;
		endPoint.x = size.width;
		startPoint.y = slope * startPoint.x + b;
		endPoint.y = slope * endPoint.x + b;
	}
	else
	{
		if (std::abs(slope) > std::numeric_limits<double>::epsilon())
		{
			startPoint.x = (0 - b) / slope;
			endPoint.x = (size.height - b) / slope;
		}
		else
		{
			startPoint.x = point.x;
			endPoint.x = point.x;
		}

		startPoint.y = 0;
		endPoint.y = size.height;
	}

	line = cv::Vec4f(startPoint.x, startPoint.y, endPoint.x, endPoint.y);
}

void QRCode::compareWithLine(const std::vector<cv::Vec4i>& lines, std::vector<cv::Vec4i>& firstLines, std::vector<cv::Vec4i>& secondLines, const cv::Vec4f& referenceLine)
{
	if (lines.empty())
		return;

	if (referenceLine[0] < 0 || referenceLine[1] < 0 || referenceLine[2] < 0 || referenceLine[3] < 0)
		return;

	if ((referenceLine[0] == referenceLine[2]) && (referenceLine[1] == referenceLine[3]))
		return;

	for (const auto& line : lines)
	{
		int x = (line[0] + line[2]) / 2;
		int y = (line[1] + line[3]) / 2;

		float distance = (x - referenceLine[0]) * (referenceLine[3] - referenceLine[1]) - (y - referenceLine[1]) * (referenceLine[2] - referenceLine[0]);

		if (distance > 0)
			firstLines.push_back(line);
		else
			secondLines.push_back(line);
	}
}

cv::Vec4i QRCode::initialTerminalPoints(std::vector<cv::Vec4i>& lines, const bool& direction)
{
	if (lines.empty())
		return cv::Vec4i();

	cv::Vec4i segment = lines[0];
	for (auto& line : lines)
	{
		if (line[0] < 0 || line[1] < 0 || line[2] < 0 || line[3] < 0)
			continue;

		if (!(line[2] - line[1]) && !(line[3] - line[1]))
			continue;

		if (line[direction] > line[direction + 2])
		{
			std::swap(line[0], line[2]);
			std::swap(line[1], line[3]);
		}

		if (line[direction] < segment[direction])
		{
			segment[0] = line[0];
			segment[1] = line[1];
		}
		if (line[direction + 2] > segment[direction + 2])
		{
			segment[2] = line[2];
			segment[3] = line[3];
		}
	}

	return segment;
}

#ifdef _DEBUG
bool QRCode::lineSorting(std::vector<cv::Vec4i>& sortedLines, const std::vector<cv::Vec4i>& lines, const cv::Size& size, const cv::Mat& src)
#else
bool QRCode::lineSorting(std::vector<cv::Vec4i>& sortedLines, const std::vector<cv::Vec4i>& lines, const cv::Size& size)
#endif
{
	if (lines.size() < 4)
		return false;

	if (size.width < 0 || size.height < 0)
		return false;

	float horizontalSlope = 0;

	std::vector<cv::Vec4i> verticalLines;
	std::vector<cv::Vec4i> horizontalLines;

	for (const auto& line : lines)
	{
		if (line[0] == line[2])
		{
			verticalLines.push_back(line);
			continue;
		}

		float slope = static_cast<float>(line[3] - line[1]) / static_cast<float>(line[2] - line[0]);
		float angle = std::abs(atan(slope) * 180 / CV_PI);
		float distanceOy = std::abs(90 - angle);
		float distanceOx = std::abs(0 - angle);

		if (distanceOy < distanceOx)
			verticalLines.push_back(line);
		else
		{
			horizontalLines.push_back(line);
			horizontalSlope += slope;
		}
	}
	horizontalSlope /= horizontalLines.size();

	cv::Vec2i centroid = cv::Vec2i(size.width / 2, size.height / 2);

	cv::Vec4f horizontalLine;
	lineThroughPoint(horizontalLine, horizontalSlope, centroid, 1);

	std::vector<cv::Vec4i> topLines, bottomLines;
	compareWithLine(horizontalLines, topLines, bottomLines, horizontalLine);

	if (topLines.empty() || bottomLines.empty())
		return false;

	cv::Vec4f verticalLine;
	lineThroughPoint(verticalLine, 0, centroid, 0);

	std::vector<cv::Vec4i> leftLines, rightLines;
	compareWithLine(verticalLines, rightLines, leftLines, verticalLine);

	if (leftLines.empty() || rightLines.empty())
		return false;

	sortedLines.push_back(initialTerminalPoints(leftLines, 1));
	sortedLines.push_back(initialTerminalPoints(topLines, 0));
	sortedLines.push_back(initialTerminalPoints(rightLines, 1));
	sortedLines.push_back(initialTerminalPoints(bottomLines, 0));

#ifdef _DEBUG
	if (src.empty() || src.type() != CV_8UC1)
		return true;

	cv::Mat drawnReferenceLines;
	cv::cvtColor(src, drawnReferenceLines, cv::COLOR_GRAY2BGR);
	cv::line(drawnReferenceLines, cv::Point(horizontalLine[0], horizontalLine[1]), cv::Point(horizontalLine[2], horizontalLine[3]), cv::Scalar(0, 255, 0), 1);
	cv::line(drawnReferenceLines, cv::Point(verticalLine[0], verticalLine[1]), cv::Point(verticalLine[2], verticalLine[3]), cv::Scalar(0, 255, 0), 1);

	cv::Mat drawnHoughLines;
	cv::cvtColor(src, drawnHoughLines, cv::COLOR_GRAY2BGR);
	for (const cv::Vec4i& line : leftLines)
	{
		cv::Point start(line[0], line[1]);
		cv::Point end(line[2], line[3]);
		cv::line(drawnHoughLines, start, end, cv::Scalar(255, 0, 0), 1);
	}
	for (const cv::Vec4i& line : topLines)
	{
		cv::Point start(line[0], line[1]);
		cv::Point end(line[2], line[3]);
		cv::line(drawnHoughLines, start, end, cv::Scalar(0, 255, 0), 1);
	}
	for (const cv::Vec4i& line : rightLines)
	{
		cv::Point start(line[0], line[1]);
		cv::Point end(line[2], line[3]);
		cv::line(drawnHoughLines, start, end, cv::Scalar(0, 0, 255), 1);
	}
	for (const cv::Vec4i& line : bottomLines)
	{
		cv::Point start(line[0], line[1]);
		cv::Point end(line[2], line[3]);
		cv::line(drawnHoughLines, start, end, cv::Scalar(0, 255, 255), 1);
	}

	cv::Mat drawnSortedLines;
	cv::cvtColor(src, drawnSortedLines, cv::COLOR_GRAY2BGR);
	cv::line(drawnSortedLines, cv::Point(sortedLines[0][0], sortedLines[0][1]), cv::Point(sortedLines[0][2], sortedLines[0][3]), cv::Scalar(255, 0, 0), 1);
	cv::line(drawnSortedLines, cv::Point(sortedLines[1][0], sortedLines[1][1]), cv::Point(sortedLines[1][2], sortedLines[1][3]), cv::Scalar(0, 255, 0), 1);
	cv::line(drawnSortedLines, cv::Point(sortedLines[2][0], sortedLines[2][1]), cv::Point(sortedLines[2][2], sortedLines[2][3]), cv::Scalar(0, 0, 255), 1);
	cv::line(drawnSortedLines, cv::Point(sortedLines[3][0], sortedLines[3][1]), cv::Point(sortedLines[3][2], sortedLines[3][3]), cv::Scalar(0, 255, 255), 1);
#endif

	return true;
}

cv::Point2f QRCode::intersection(const cv::Vec4i& firstLine, const cv::Vec4i& secondLine)
{
	if ((firstLine[0] == firstLine[2]) && (firstLine[1] == firstLine[3]))
		return cv::Point2f();

	if ((secondLine[0] == secondLine[2]) && (secondLine[1] == secondLine[3]))
		return cv::Point2f();

	int a1 = firstLine[3] - firstLine[1];
	int b1 = firstLine[0] - firstLine[2];
	int c1 = a1 * firstLine[0] + b1 * firstLine[1];

	int a2 = secondLine[3] - secondLine[1];
	int b2 = secondLine[0] - secondLine[2];
	int c2 = a2 * secondLine[0] + b2 * secondLine[1];

	int determinant = a1 * b2 - a2 * b1;

	if (!determinant)
		return cv::Point2f();

	float x = (b2 * c1 - b1 * c2) / determinant;
	float y = (a1 * c2 - a2 * c1) / determinant;

	return cv::Point2f(x, y);
}

void QRCode::paddingCoordinates(std::vector<cv::Point2f>& coordinates, const float& percentage)
{
	cv::Rect2f bbox = cv::boundingRect(coordinates);
	float padding = percentage * std::min(bbox.height, bbox.width);

	cv::Point2f centroid(0, 0);
	for (const auto& point : coordinates)
		centroid += point;
	centroid *= 0.25f;

	for (auto& point : coordinates)
	{
		cv::Point2f vector = point - centroid;
		float length = std::sqrt(vector.x * vector.x + vector.y * vector.y);

		cv::Point2f direction = vector * (1.0f / length);
		point += direction * padding;
	}
}

std::vector<cv::Point2f> QRCode::rectificationCoordinates(const std::vector<std::vector<cv::Point>>& anchors, const float& percentage)
{
	std::vector<cv::Point2f> coordinates;
	cv::Vec4i firstLine, secondLine;
	for (int i = 0; i < 3; i++)
	{
		std::vector<cv::Point> anchor = anchors[i];
		cv::Rect bbox = cv::boundingRect(anchor);

		std::vector<cv::Point> localContour;
		localContour.reserve(anchor.size());

		for (const auto& p : anchor)
			localContour.emplace_back(p.x - bbox.x, p.y - bbox.y);

		cv::Mat contour = cv::Mat::zeros(bbox.size(), CV_8UC1);
		cv::drawContours(contour, std::vector<std::vector<cv::Point>>{ localContour }, 0, cv::Scalar(255), 1);

		int paddingY = static_cast<int>(bbox.height * 0.4);
		int paddingX = static_cast<int>(bbox.width * 0.4);
		cv::Mat paddedContour;
		cv::copyMakeBorder(contour, paddedContour, paddingY, paddingY, paddingX, paddingX, cv::BORDER_CONSTANT, cv::Scalar(0));

		float minLineLength = bbox.height * 0.8;
		float maxLineGap = bbox.height * 0.05;

		std::vector<cv::Vec4i> lines;
		std::vector<cv::Vec4i> sortedLines;
		do {
			cv::HoughLinesP(paddedContour, lines, 1, CV_PI / 180, 10, minLineLength, maxLineGap);
			minLineLength--;
		}
#ifdef _DEBUG
		while (!lineSorting(sortedLines, lines, paddedContour.size(), paddedContour) && minLineLength > 0);
#else
		while (!lineSorting(sortedLines, lines, paddedContour.size()) && minLineLength > 0);
#endif
		if (minLineLength <= 0)
			return std::vector<cv::Point2f>();

		cv::Point2f point;
		if (i == 0)
		{
			point = intersection(sortedLines[0], sortedLines[1]);
		}
		else if (i == 1)
		{
			point = intersection(sortedLines[1], sortedLines[2]);

			firstLine = sortedLines[2];
			firstLine[0] = firstLine[0] - paddingX + bbox.x;
			firstLine[1] = firstLine[1] - paddingY + bbox.y;
			firstLine[2] = firstLine[2] - paddingX + bbox.x;
			firstLine[3] = firstLine[3] - paddingY + bbox.y;
		}
		else if (i == 2)
		{
			point = intersection(sortedLines[0], sortedLines[3]);

			secondLine = sortedLines[3];
			secondLine[0] = secondLine[0] - paddingX + bbox.x;
			secondLine[1] = secondLine[1] - paddingY + bbox.y;
			secondLine[2] = secondLine[2] - paddingX + bbox.x;
			secondLine[3] = secondLine[3] - paddingY + bbox.y;
		}

		point.x = point.x - paddingX + bbox.x;
		point.y = point.y - paddingY + bbox.y;
		coordinates.push_back(point);
	}
	coordinates.push_back(intersection(firstLine, secondLine));

	paddingCoordinates(coordinates, percentage);
	return coordinates;
}

bool QRCode::resizeToPoints(const cv::Mat& src, cv::Mat& dst, std::vector<cv::Point2f>& coordinates, const float& percentage)
{
	if (src.empty() || src.type() != CV_8UC1)
		return false;

	if (coordinates.size() != 4)
		return false;

	if (percentage < 0 || percentage > 1)
		return false;

	int minX = src.cols, minY = src.rows, maxX = 0, maxY = 0;

	for (const auto& point : coordinates)
	{
		minX = std::min(minX, static_cast<int>(point.x));
		minY = std::min(minY, static_cast<int>(point.y));
		maxX = std::max(maxX, static_cast<int>(point.x));
		maxY = std::max(maxY, static_cast<int>(point.y));
	}

	int paddingTop = 0, paddingBottom = 0, paddingRight = 0, paddingLeft = 0;

	if (maxX > src.cols - 1)
		paddingRight = maxX - (src.cols - 1);
	if (maxY > src.rows - 1)
		paddingBottom = maxY - (src.rows - 1);
	if (minX < 0)
		paddingLeft = -minX;
	if (minY < 0)
		paddingTop = -minY;

	for (auto& point : coordinates)
	{
		point.x = paddingLeft + point.x;
		point.y = paddingTop + point.y;
	}

	cv::copyMakeBorder(src, dst, paddingTop, paddingBottom, paddingLeft, paddingRight, cv::BORDER_CONSTANT);

	if (dst.rows > src.rows + (src.rows * percentage))
		return false;

	if (dst.cols > src.cols + (src.cols * percentage))
		return false;

	return true;
}

bool QRCode::geometricalTransformation(const cv::Mat& src, cv::Mat& dst, const std::vector<cv::Point2f>& coordinates, const float& percentage)
{
	if (src.empty() || src.type() != CV_8UC1)
		return false;

	if (coordinates.size() != 4 || percentage < 0)
		return false;

	int height = coordinates[3].y - coordinates[0].y;
	int width = height;

	if (height < 0 || width < 0)
		return false;

	std::vector<cv::Point2f> finalCoordinates;
	finalCoordinates.push_back(cv::Point2f(0, 0));
	finalCoordinates.push_back(cv::Point2f(width - 1, 0));
	finalCoordinates.push_back(cv::Point2f(0, height - 1));
	finalCoordinates.push_back(cv::Point2f(width - 1, height - 1));

	cv::Mat perspectiveTransform = cv::getPerspectiveTransform(coordinates, finalCoordinates);
	cv::warpPerspective(src, dst, perspectiveTransform, cv::Size(width, height));

	return cv::countNonZero(dst);
}

bool QRCode::getMatrixFromImage(const cv::Mat& src, cv::Mat& dst, cv::dnn::Net aiModel)
{
	cv::Mat image;
	cv::resize(src, image, cv::Size(210, 210), 0, 0, cv::INTER_NEAREST);

	cv::Mat blob = cv::dnn::blobFromImage(image, 1.0 / 255.0);
	aiModel.setInput(blob);
	cv::Mat output = aiModel.forward();

	int total = output.total();
	int n = std::sqrt(total);
	if (std::pow(n, 2) != total)
		return false;

	cv::Mat reshaped = output.reshape(1, n);
	cv::threshold(reshaped, dst, 0.5, 255, cv::THRESH_BINARY);
	dst.convertTo(dst, CV_8UC1);

	return true;
}

bool QRCode::getID(const cv::Mat& src, std::string& id, ZXing::Position* position)
{
	ZXing::ImageView imageView(src.data, src.cols, src.rows, ZXing::ImageFormat::Lum);
	ZXing::DecodeHints hints;
	hints.setFormats(ZXing::BarcodeFormat::QRCode);

	auto result = ZXing::ReadBarcode(imageView, hints);
	if (!result.isValid())
		return false;

	if (position)
	{
		*position = result.position();
		if (position->empty())
			return false;
	}

	id = result.text();

	return true;
}

std::vector<cv::Point2f> QRCode::cvtPositionToCoordinates(const ZXing::Position& position)
{
	std::vector<cv::Point2f> coordinates;

	for (const auto& coordinate : position)
		coordinates.emplace_back(static_cast<float>(coordinate.x), static_cast<float>(coordinate.y));

	return coordinates;
}

std::string QRCode::decodeQR(const std::string& path)
{
	std::string id;
	std::vector<cv::Point2f> coordinates;
	ZXing::Position position;
	cv::Mat image = cv::imread(path, cv::IMREAD_COLOR);

	cv::Mat resized;
	resize(image, resized, 1080);

	cv::Mat gray;
	cv::cvtColor(resized, gray, cv::COLOR_BGR2GRAY);

	cv::Mat edges;
	edgeDetection(gray, edges);

	std::vector<std::vector<cv::Point>> anchors;
	if (!detectQRAnchors(edges, anchors))
	{
		if (getID(gray, id, &position))
			coordinates = cvtPositionToCoordinates(position);

		return id;
	}

	sortAnchors(anchors);
	coordinates = rectificationCoordinates(anchors, 0.07);

	cv::Mat resizedConnectedComponent;
	if (!resizeToPoints(gray, resizedConnectedComponent, coordinates, 0.2))
	{
		if (getID(gray, id, &position))
			coordinates = cvtPositionToCoordinates(position);

		return id;
	}

	cv::Mat transformedConnectedComponent;
	if (!geometricalTransformation(resizedConnectedComponent, transformedConnectedComponent, coordinates, 0.2))
	{
		if (getID(gray, id, &position))
			coordinates = cvtPositionToCoordinates(position);

		return id;
	}

	cv::Mat qrCode;
	if (!getMatrixFromImage(transformedConnectedComponent, qrCode, aiModel))
	{
		if (!getID(transformedConnectedComponent, id))
		{
			if (getID(gray, id, &position))
				coordinates = cvtPositionToCoordinates(position);
		}

		return id;
	}

	if (!getID(qrCode, id))
		if (!getID(transformedConnectedComponent, id))
			getID(gray, id);

	return id;
}
