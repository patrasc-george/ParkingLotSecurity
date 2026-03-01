#include "qrcodedetection.h"

#include <opencv2/core/utils/logger.hpp>
#include <opencv2/objdetect.hpp>
#include <ZXing/ReadBarcode.h>
#include <ZXing/BarcodeFormat.h>
#include <ZXing/DecodeHints.h>
#include <ZXing/ImageView.h>
#include <regex>

QRCode::QRCode()
{
	cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_SILENT);

#ifdef _DEBUG
	aiModelPath = "../../../qrbitnet.onnx";
#else
	aiModelPath = "qrbitnet.onnx";
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

cv::Mat QRCode::countTransitions(const cv::Mat& src, const bool& isVertical)
{
	cv::Mat counter = isVertical ? cv::Mat::zeros(1, src.cols, CV_16UC1) : cv::Mat::zeros(1, src.rows, CV_16UC1);

	if (isVertical)
	{
		for (int x = 0; x < src.cols; x++)
			for (int y = 0; y < src.rows - 1; y++)
				if (src.ptr<uchar>(y, x)[0] != src.ptr<uchar>(y + 1, x)[0])
					counter.ptr<ushort>(0, x)[0]++;
	}
	else
	{
		for (int y = 0; y < src.rows; y++)
			for (int x = 0; x < src.cols - 1; x++)
				if (src.ptr<uchar>(y, x)[0] != src.ptr<uchar>(y, x + 1)[0])
					counter.ptr<ushort>(0, y)[0]++;
	}

	return counter;
}

cv::Mat QRCode::drawHistogram(const cv::Mat& histogram)
{
	double minValue, maxValue;
	cv::minMaxLoc(histogram, &minValue, &maxValue);
	int height = static_cast<int>(maxValue);

	cv::Mat image = cv::Mat::ones(height, histogram.cols, CV_8UC1) * 255;
	for (int x = 0; x < histogram.cols; x++)
	{
		int value = height - cvRound(histogram.ptr<ushort>(0, x)[0]);
		cv::line(image, cv::Point(x, height), cv::Point(x, value), cv::Scalar(0), 1);
	}

	return image;
}

cv::Mat QRCode::densestInterval(const cv::Mat& histogram, const float& percentage, const float& gap)
{
	std::vector<std::pair<int, int>> histogramPairs;
	for (int i = 0; i < histogram.cols; i++)
	{
		int value = histogram.ptr<ushort>(0, i)[0];
		histogramPairs.push_back(std::make_pair(i, value));
	}

	std::sort(histogramPairs.begin(), histogramPairs.end(), [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
		return a.second > b.second;
		});

	cv::Mat normalizedHistogram = histogram.clone();
	int threshold = cv::sum(normalizedHistogram)[0] * percentage;
	int cumulativeSum = 0;
	int lastIndex = 0;
	for (int i = 0; i < histogramPairs.size(); i++)
	{
		cumulativeSum += histogramPairs[i].second;
		if (cumulativeSum >= threshold)
		{
			lastIndex = i;
			break;
		}
	}

	cv::Mat result = normalizedHistogram.clone();
	for (int i = lastIndex + 1; i < histogramPairs.size(); i++)
		result.ptr<ushort>(0, histogramPairs[i].first)[0] = 0;

	int maxGap = result.cols * gap;
	int currentGap = 0;
	for (int i = 0; i < result.cols; i++)
	{
		if (result.ptr<ushort>(0, i)[0])
		{
			if (currentGap < maxGap)
			{
				int index = i - currentGap;
				for (int j = index; j < index + currentGap; j++)
					if (normalizedHistogram.ptr<ushort>(0, j)[0])
						result.ptr<ushort>(0, j)[0] = normalizedHistogram.ptr<ushort>(0, j)[0];
					else
						result.ptr<ushort>(0, j)[0] = 1;
			}

			currentGap = 0;
		}
		else
			currentGap++;
	}

	return result;
}

void QRCode::removeRowsAndColumns(const cv::Mat& src, cv::Mat& dst, const cv::Mat& verticalHistogram, const cv::Mat& horizontalHistogram)
{
	int width = cv::countNonZero(verticalHistogram);
	cv::Mat firstImage = cv::Mat::zeros(src.rows, width, src.type());
	int colIndex = 0;
	for (int i = 0; i < verticalHistogram.cols; i++)
	{
		int value = verticalHistogram.ptr<ushort>(0, i)[0];

		if (value)
		{
			src.col(i).copyTo(firstImage.col(colIndex));
			colIndex++;
		}
	}

	int height = cv::countNonZero(horizontalHistogram);
	dst = cv::Mat::zeros(height, firstImage.cols, firstImage.type());
	int rowIndex = 0;
	for (int i = 0; i < horizontalHistogram.cols; i++)
	{
		int value = horizontalHistogram.ptr<ushort>(0, i)[0];

		if (value)
		{
			firstImage.row(i).copyTo(dst.row(rowIndex));
			rowIndex++;
		}
	}
}

cv::Size QRCode::getKernelSize(const cv::Size& size, const float& percentage)
{
	cv::Size kernel = cv::Size(size.height * percentage, size.width * percentage);

	if (kernel.height < 3)
		kernel.height = 3;
	if (kernel.width < 3)
		kernel.width = 3;

	if (kernel.height % 2 == 0)
		kernel.height--;
	if (kernel.width % 2 == 0)
		kernel.width--;

	return kernel;
}

bool compareConnectedComponents(const std::pair<int, int>& a, const std::pair<int, int>& b)
{
	return a.second > b.second;
}

void QRCode::getConnectedComponents(const cv::Mat& src, cv::Mat& stats, std::vector<std::pair<int, int>>& areas, const int& newSize)
{
	if (src.empty() || src.type() != CV_8UC1)
		return;

	cv::Mat labels, cendtroids;
	int size = cv::connectedComponentsWithStats(src, labels, stats, cendtroids);

	for (int i = 1; i < size; i++)
		areas.push_back(std::make_pair(i, stats.ptr<int>(i, cv::CC_STAT_AREA)[0]));

	std::sort(areas.begin(), areas.end(), compareConnectedComponents);

	if (newSize && areas.size() > newSize)
		areas.resize(newSize);
}

cv::Rect QRCode::getRoi(const cv::Mat& stats, const int& label)
{
	if (stats.empty() || stats.type() != CV_32SC1)
		return cv::Rect();

	if (label < 0 || label > stats.size().height)
		return cv::Rect();

	cv::Rect roi;
	roi.x = stats.ptr<int>(label, cv::CC_STAT_LEFT)[0];
	roi.y = stats.ptr<int>(label, cv::CC_STAT_TOP)[0];
	roi.width = stats.ptr<int>(label, cv::CC_STAT_WIDTH)[0];
	roi.height = stats.ptr<int>(label, cv::CC_STAT_HEIGHT)[0];

	return roi;
}

bool QRCode::ratioBBox(const cv::Rect& roi, const float& max)
{
	if (roi.size() == cv::Size(0, 0))
		return false;

	if (max < 1)
		return false;

	float maxSize = std::max(roi.height, roi.width);
	float minSize = std::min(roi.height, roi.width);
	float ratio = maxSize / minSize;

	return ratio < max;
}

void QRCode::paddingRect(const cv::Rect& src, cv::Rect& dst, const float& percentage, const cv::Size& size)
{
	if (src.width <= 0 || src.height <= 0)
		return;

	if (percentage < 0 || percentage > 1)
		return;

	if (size.width < 0 || size.height < 0)
		return;

	if (size != cv::Size() && (src.x + src.width > size.width || src.y + src.height > size.height))
		return;

	int paddingX = src.width * percentage;
	if (paddingX < 3)
		paddingX = 3;

	int paddingY = src.height * percentage;
	if (paddingY < 3)
		paddingY = 3;

	dst.x = src.x - paddingX;
	dst.y = src.y - paddingY;
	dst.width = src.width + paddingX * 2;
	dst.height = src.height + paddingY * 2;

	if (size.width && size.height)
	{
		if (dst.x < 0)
			dst.x = 0;
		if (dst.y < 0)
			dst.y = 0;
		if (dst.x + dst.width > size.width)
			dst.width = size.width - dst.x;
		if (dst.y + dst.height > size.height)
			dst.height = size.height - dst.y;
	}
}

std::vector<cv::Point> QRCode::getLargestContour(const std::vector<std::vector<cv::Point>>& contours)
{
	std::vector<cv::Point> largestContour;

	if (contours.empty())
		return std::vector<cv::Point>();

	double maxArea = 0;
	for (const auto& contour : contours)
	{
		double area = cv::contourArea(contour);
		if (area > maxArea)
		{
			maxArea = area;
			largestContour = contour;
		}
	}

	return largestContour;
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

	int A1 = firstLine[3] - firstLine[1];
	int B1 = firstLine[0] - firstLine[2];
	int C1 = A1 * firstLine[0] + B1 * firstLine[1];

	int A2 = secondLine[3] - secondLine[1];
	int B2 = secondLine[0] - secondLine[2];
	int C2 = A2 * secondLine[0] + B2 * secondLine[1];

	int determinant = A1 * B2 - A2 * B1;

	if (!determinant)
		return cv::Point2f();

	float x = (B2 * C1 - B1 * C2) / determinant;
	float y = (A1 * C2 - A2 * C1) / determinant;

	return cv::Point2f(x, y);
}

void QRCode::paddingCoordinates(std::vector<cv::Point2f>& coordinates, const float& percentage, const cv::Size& size)
{
	float padding = percentage * std::min(size.height, size.width);

	cv::Point2f centroid(0, 0);
	for (const auto& point : coordinates)
		centroid += point;
	centroid *= 0.25f;

	for (auto& point : coordinates)
	{
		cv::Point2f vector = point - centroid;
		float lenght = std::sqrt(vector.x * vector.x + vector.y * vector.y);

		cv::Point2f direction = vector * (1.0f / lenght);
		point += direction * padding;
	}
}

std::vector<cv::Point2f> QRCode::rectificationCoordinates(const cv::Mat& src, const float& percentage)
{
	std::vector<cv::Point2f> coordinates;

	cv::Mat binary;
	cv::threshold(src, binary, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
	binary = 255 - binary;

	std::vector<cv::Rect> subImages;
	subImages.push_back(cv::Rect(0, 0, binary.cols / 2, binary.rows / 2));
	subImages.push_back(cv::Rect(binary.cols / 2, 0, binary.cols / 2, binary.rows / 2));
	subImages.push_back(cv::Rect(0, binary.rows / 2, binary.cols / 2, binary.rows / 2));

	std::vector<cv::Point2f> corners;
	cv::Vec4i firstLine, secondLine;
	for (int i = 0; i < subImages.size(); i++)
	{
		cv::Mat subImage = binary(subImages[i]);

		std::vector<std::vector<cv::Point>> contours;
		cv::findContours(subImage, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

		if (contours.size() < 2)
			return std::vector<cv::Point2f>();

		std::vector<cv::Point> largestContour = getLargestContour(contours);
		cv::Rect bbox = cv::boundingRect(largestContour);

		cv::Mat contour = cv::Mat::zeros(subImage.size(), subImage.type());
		cv::drawContours(contour, std::vector<std::vector<cv::Point>>{largestContour}, 0, cv::Scalar(255), 1);
		contour = contour(bbox);

		cv::Mat paddedContour;
		cv::copyMakeBorder(contour, paddedContour, bbox.height * 0.4, bbox.height * 0.4, bbox.width * 0.4, bbox.width * 0.4, cv::BORDER_CONSTANT, cv::Scalar(0));

		float minLineLenght = bbox.height * 0.8;
		float maxLineGap = bbox.height * 0.05;

		std::vector<cv::Vec4i> lines;
		std::vector<cv::Vec4i> sortedLines;
		do {
			cv::HoughLinesP(paddedContour, lines, 1, CV_PI / 180, 10, minLineLenght, maxLineGap);
			minLineLenght--;
		}
#ifdef _DEBUG
		while (!lineSorting(sortedLines, lines, paddedContour.size(), paddedContour) && minLineLenght > 0);
#else
		while (!lineSorting(sortedLines, lines, src.size()) && minLineLenght > 0);
#endif
		if (minLineLenght <= 0)
			return std::vector<cv::Point2f>();

		cv::Point2f point;
		if (i == 0)
		{
			point = intersection(sortedLines[0], sortedLines[1]);
		}
		else if (i == 1)
		{
			point = intersection(sortedLines[1], sortedLines[2]);
			point.x += subImages[0].width;

			firstLine = sortedLines[2];
			firstLine[0] = firstLine[0] - bbox.width * 0.4 + bbox.x + subImages[0].width;
			firstLine[1] = firstLine[1] - bbox.height * 0.4 + bbox.y;
			firstLine[2] = firstLine[2] - bbox.width * 0.4 + bbox.x + subImages[0].width;
			firstLine[3] = firstLine[3] - bbox.height * 0.4 + bbox.y;
		}
		else if (i == 2)
		{
			point = intersection(sortedLines[0], sortedLines[3]);
			point.y += subImages[0].height;

			secondLine = sortedLines[3];
			secondLine[0] = secondLine[0] - bbox.width * 0.4 + bbox.x;
			secondLine[1] = secondLine[1] - bbox.height * 0.4 + bbox.y + subImages[0].height;
			secondLine[2] = secondLine[2] - bbox.width * 0.4 + bbox.x;
			secondLine[3] = secondLine[3] - bbox.height * 0.4 + bbox.y + subImages[0].height;
		}

		point.x = point.x - bbox.width * 0.4 + bbox.x;
		point.y = point.y - bbox.height * 0.4 + bbox.y;
		coordinates.push_back(point);
	}
	coordinates.push_back(intersection(firstLine, secondLine));

	paddingCoordinates(coordinates, percentage, src.size());

#ifdef _DEBUG
	cv::Mat debug;
	cv::cvtColor(binary, debug, cv::COLOR_GRAY2BGR);

	for (const auto& point : coordinates)
		cv::circle(debug, point, 2, cv::Scalar(0, 0, 255), -1);

	cv::line(debug, cv::Point(firstLine[0], firstLine[1]), cv::Point(firstLine[2], firstLine[3]), cv::Scalar(0, 255, 0), 2);
	cv::line(debug, cv::Point(secondLine[0], secondLine[1]), cv::Point(secondLine[2], secondLine[3]), cv::Scalar(0, 255, 0), 2);
#endif

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

	if (height < src.rows * percentage)
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

bool getDateTime(const cv::Mat& src, std::string& dateTime)
{
	ZXing::ImageView imageView(src.data, src.cols, src.rows, ZXing::ImageFormat::Lum);
	ZXing::DecodeHints hints;
	hints.setFormats(ZXing::BarcodeFormat::QRCode);

	auto result = ZXing::ReadBarcode(imageView, hints);
	if (!result.isValid())
		return false;

	std::string raw = result.text();
	if (raw.length() != 12)
		return false;

	std::string day = raw.substr(0, 2);
	std::string month = raw.substr(2, 2);
	std::string year = raw.substr(4, 2);
	std::string hour = raw.substr(6, 2);
	std::string minute = raw.substr(8, 2);
	std::string second = raw.substr(10, 2);
	std::string fullYear = "20" + year;

	dateTime = day + "-" + month + "-" + fullYear + " " +
		hour + ":" + minute + ":" + second;

	return true;
}

std::string QRCode::decodeQR(const std::string& path)
{
	std::string dateTime = "";
	cv::Mat image = cv::imread(path, cv::IMREAD_COLOR);

	cv::Mat resized;
	resize(image, resized, 1080);

	cv::Mat gray;
	cv::cvtColor(resized, gray, cv::COLOR_BGR2GRAY);

	cv::Mat edges;
	edgeDetection(gray, edges);

	cv::Mat verticalHistogram = countTransitions(edges, true);
	cv::Mat horizontalHistogram = countTransitions(edges, false);

	cv::Mat densestVerticalInterval = densestInterval(verticalHistogram, 0.9, 0.1);
	cv::Mat densestHorizontalInterval = densestInterval(horizontalHistogram, 0.9, 0.1);

#ifdef _DEBUG
	cv::Mat verticalHistogramImage = drawHistogram(verticalHistogram);
	cv::Mat horizontalHistogramImage = drawHistogram(horizontalHistogram);

	cv::Mat densestVerticalIntervalImage = drawHistogram(densestVerticalInterval);
	cv::Mat densestHorizontalIntervalImage = drawHistogram(densestHorizontalInterval);
#endif

	cv::Mat cropped;
	removeRowsAndColumns(gray, cropped, densestVerticalInterval, densestHorizontalInterval);

	cv::Mat binary;
	cv::threshold(cropped, binary, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
	binary = 255 - binary;

	cv::Mat closed;
	cv::Size kernelSize = getKernelSize(binary.size(), 0.05);
	cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(kernelSize.width, 3));
	cv::morphologyEx(binary, closed, cv::MORPH_CLOSE, kernel);

	cv::Mat stats;
	std::vector<std::pair<int, int>> areas;
	getConnectedComponents(closed, stats, areas, 10);

#ifdef _DEBUG
	cv::Mat debug;
	cv::cvtColor(cropped, debug, cv::COLOR_GRAY2BGR);
#endif

	cv::dnn::Net aiModel = cv::dnn::readNetFromONNX(aiModelPath);
	for (int i = 0; i < areas.size(); i++)
	{
		int label = areas[i].first;
		cv::Rect roi = getRoi(stats, label);

#ifdef _DEBUG
		cv::Mat colorConnectedComponent = debug(roi);
#endif

		if (!ratioBBox(roi, 1.3))
			continue;

		paddingRect(roi, roi, 0.05, cropped.size());

		cv::Mat grayConnectedComponent = cropped(roi);
		std::vector<cv::Point2f> coordinates = rectificationCoordinates(grayConnectedComponent, 0.02);

		cv::Mat resizedConnectedComponent;
		if (!resizeToPoints(grayConnectedComponent, resizedConnectedComponent, coordinates, 0.2))
			continue;

		cv::Mat transformedConnectedComponent;
		if (!geometricalTransformation(resizedConnectedComponent, transformedConnectedComponent, coordinates, 0.2))
			continue;

		cv::Mat qrCode;
		if (!getMatrixFromImage(transformedConnectedComponent, qrCode, aiModel))
			continue;

#ifdef _DEBUG
		cv::Mat qrCodeUpscaled;
		cv::resize(qrCode, qrCodeUpscaled, cv::Size(210, 210), 0, 0, cv::INTER_NEAREST);
		cv::rectangle(debug, roi, cv::Scalar(0, 255, 0), 2);
#endif

		if (getDateTime(qrCode, dateTime))
			break;

		if (getDateTime(transformedConnectedComponent, dateTime))
			break;

		if (getDateTime(cropped, dateTime))
			break;

		if (getDateTime(gray, dateTime))
			break;
	}

	return dateTime;
}
