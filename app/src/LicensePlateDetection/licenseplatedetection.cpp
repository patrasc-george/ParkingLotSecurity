#include "licenseplatedetection.h"

void Algorithm::BGR2HSV(const cv::Mat& src, cv::Mat& dst)
{
	if (src.empty() || src.type() != CV_8UC3)
		return;

	dst = cv::Mat(src.size(), CV_8UC3);

	for (int y = 0; y < src.rows; y++)
		for (int x = 0; x < src.cols; x++)
		{
			double b = src.ptr<uchar>(y, x)[0] / 255.0;
			double g = src.ptr<uchar>(y, x)[1] / 255.0;
			double r = src.ptr<uchar>(y, x)[2] / 255.0;

			double max = std::max(b, std::max(g, r));
			double min = std::min(b, std::min(g, r));
			double difference = max - min;
			double h, s;

			if (max == min)
				h = 0;
			else if (max == r)
				h = fmod(60 * ((g - b) / difference) + 360, 360) / 2;
			else if (max == g)
				h = fmod(60 * ((b - r) / difference) + 120, 360) / 2;
			else if (max == b)
				h = fmod(60 * ((r - g) / difference) + 240, 360) / 2;

			if (max == 0)
				s = 0;
			else
				s = (difference / max) * 255;

			double v = max * 255;

			dst.ptr<uchar>(y, x)[0] = h;
			dst.ptr<uchar>(y, x)[1] = s;
			dst.ptr<uchar>(y, x)[2] = v;
		}
}

void Algorithm::HSV2Binary(const cv::Mat& src, cv::Mat& dst, const uchar& threshold)
{
	if (src.empty() || src.type() != CV_8UC3)
		return;

	dst = cv::Mat::zeros(src.size(), CV_8UC1);

	for (int y = 0; y < src.rows; y++)
		for (int x = 0; x < src.cols; x++)
			if (src.ptr<uchar>(y, x)[1] < threshold && src.ptr<uchar>(y, x)[2] > threshold)
				dst.ptr<uchar>(y, x)[0] = 255;
}

bool compareConnectedComponents(const std::pair<int, int>& a, const std::pair<int, int>& b)
{
	return a.second > b.second;
}

void Algorithm::getConnectedComponents(const cv::Mat& src, cv::Mat& stats, std::vector<std::pair<int, int>>& areas, const int& newSize)
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

void Algorithm::getRoi(const cv::Mat& stats, cv::Rect& roi, const int& label)
{
	if (stats.empty() || stats.type() != CV_32SC1)
		return;

	if (label < 0 || label > stats.size().height)
		return;

	roi.x = stats.ptr<int>(label, cv::CC_STAT_LEFT)[0];
	roi.y = stats.ptr<int>(label, cv::CC_STAT_TOP)[0];
	roi.width = stats.ptr<int>(label, cv::CC_STAT_WIDTH)[0];
	roi.height = stats.ptr<int>(label, cv::CC_STAT_HEIGHT)[0];
}

bool Algorithm::sizeBBox(const cv::Mat& src, const cv::Rect& roi, const float& min, const float& max)
{
	if (src.size() == cv::Size(0, 0) || roi.size() == cv::Size(0, 0))
		return false;

	if (max < 0 || max > 1 || min < 0 || min > 1 || max <= min)
		return false;

	int areaConnectedComponent = roi.height * roi.width;
	int areaSrc = src.rows * src.cols;

	return areaConnectedComponent > areaSrc * min &&
		areaConnectedComponent < areaSrc * max;
}

bool Algorithm::heightBBox(const cv::Rect& roi, const float& min, const float& max)
{
	if (roi.size() == cv::Size(0, 0))
		return false;

	if (max < 0 || max > 1 || min < 0 || min > 1 || max <= min)
		return false;

	return roi.height > roi.width * min &&
		roi.height < roi.width * max;
}

void Algorithm::paddingRect(const cv::Rect& src, cv::Rect& dst, const float& percentage, const bool& square, const cv::Size& size)
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

	if (square)
	{
		paddingX = std::max(paddingX, paddingY);
		paddingY = paddingX;
	}

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

void Algorithm::blueToBlack(const cv::Mat& src, cv::Mat& dst)
{
	if (src.empty() || src.type() != CV_8UC3)
		return;

	dst = src.clone();

	for (int y = 0; y < src.rows; y++)
		for (int x = 0; x < src.cols; x++)
			if (src.ptr<uchar>(y, x)[0] > 100 && src.ptr<uchar>(y, x)[0] < 130 &&
				src.ptr<uchar>(y, x)[1] > 90 && src.ptr<uchar>(y, x)[1] < 230 &&
				src.ptr<uchar>(y, x)[2] > 110 && src.ptr<uchar>(y, x)[2] < 195)
			{
				dst.ptr<uchar>(y, x)[0] = 0;
				dst.ptr<uchar>(y, x)[1] = 0;
				dst.ptr<uchar>(y, x)[2] = 0;
			}
}

void Algorithm::HSV2BGR(const cv::Mat& src, cv::Mat& dst)
{
	if (src.empty() || src.type() != CV_8UC3)
		return;

	dst = cv::Mat(src.size(), CV_8UC3);

	for (int y = 0; y < src.rows; y++)
		for (int x = 0; x < src.cols; x++)
		{
			double h = src.ptr<uchar>(y, x)[0];
			double s = src.ptr<uchar>(y, x)[1] / 255.0;
			double v = src.ptr<uchar>(y, x)[2] / 255.0;

			double chroma = s * v;
			double scaledHue = h / 30.0;
			double secondaryComponent = chroma * (1 - std::abs(fmod(scaledHue, 2) - 1));
			double valueAdjustment = v - chroma;

			double b, g, r;
			switch (int(scaledHue))
			{
			case 0:
				b = 0;
				g = secondaryComponent;
				r = chroma;
				break;
			case 1:
				b = 0;
				g = chroma;
				r = secondaryComponent;
				break;
			case 2:
				r = 0;
				g = chroma;
				b = secondaryComponent;
				break;
			case 3:
				b = chroma;
				g = secondaryComponent;
				r = 0;
				break;
			case 4:
				b = chroma;
				g = 0;
				r = secondaryComponent;
				break;
			default:
				b = secondaryComponent;
				g = 0;
				r = chroma;
				break;
			}

			dst.ptr<uchar>(y, x)[0] = (b + valueAdjustment) * 255;
			dst.ptr<uchar>(y, x)[1] = (g + valueAdjustment) * 255;
			dst.ptr<uchar>(y, x)[2] = (r + valueAdjustment) * 255;
		}
}

void Algorithm::histogram(const cv::Mat& src, cv::Mat& hist)
{
	if (src.empty() || (src.type() != CV_32FC1 && src.type() != CV_8UC1))
		return;

	double maxVal;
	cv::minMaxLoc(src, NULL, &maxVal);
	int histSize = static_cast<int>(maxVal) + 1;

	std::vector<float> range = { 0, static_cast<float>(histSize) };
	const float* ranges[] = { range.data() };

	cv::calcHist(&src, 1, 0, cv::Mat(), hist, 1, &histSize, ranges);
}

void Algorithm::cumulativeHistogram(const cv::Mat& hist, cv::Mat& cumulvativeHist)
{
	if (hist.empty() || (hist.type() != CV_32FC1 && hist.type() != CV_8UC1))
		return;

	cumulvativeHist = cv::Mat(cv::Size(hist.rows, hist.cols), hist.type());

	float sum = 0;
	for (int i = 0; i < hist.rows; i++)
	{
		sum = sum + hist.ptr<float>(i, 0)[0];
		cumulvativeHist.ptr<float>(0, i)[0] = sum;
	}
}

void Algorithm::histogramLine(const cv::Mat& cumulvativeHist, cv::Vec4f& line)
{
	if (cumulvativeHist.empty() || (cumulvativeHist.type() != CV_32FC1 && cumulvativeHist.type() != CV_8UC1))
		return;

	double maxVal;
	cv::Point max;
	cv::minMaxLoc(cumulvativeHist, NULL, &maxVal, NULL, &max);

	double minVal;
	cv::Point min;
	for (int i = 0; i < cumulvativeHist.cols; i++)
		if (cumulvativeHist.ptr<float>(0, i)[0])
		{
			minVal = cumulvativeHist.ptr<float>(0, i)[0];
			min = cv::Point(i, 0);
			break;
		}

	line = cv::Vec4f(min.x, minVal, max.x, maxVal);
}

float Algorithm::distance(const float& x, const float& y, const cv::Vec4f& line)
{
	if (x < 0 || y < 0)
		return 0;

	if (line[0] < 0 || line[1] < 0 || line[2] < 0 || line[3] < 0)
		return 0;

	if (line[0] == line[2] && line[1] == line[3])
		return 0;

	float a = line[1] - line[3];
	float b = line[2] - line[0];
	float c = (line[0] * line[3]) - (line[2] * line[1]);

	if (!(a * a + b * b))
		return 0;

	return abs((a * x + b * y + c) / sqrt(a * a + b * b));
}

void Algorithm::triangleThresholding(const cv::Mat& src, cv::Mat& dst)
{
	if (src.empty() || (src.type() != CV_32FC1 && src.type() != CV_8UC1))
		return;

	cv::Mat floatSrc;
	src.convertTo(floatSrc, CV_32F);

	cv::Mat hist;
	histogram(floatSrc, hist);

	cv::Mat cumulvativeHist;
	cumulativeHistogram(hist, cumulvativeHist);

	cv::Vec4f line;
	histogramLine(cumulvativeHist, line);

	int threshold = 0;
	float maxDistance = 0;
	for (int i = 0; i < cumulvativeHist.cols; i++)
	{
		int auxDistance = distance(i, cumulvativeHist.ptr<float>(0, i)[0], line);
		if (auxDistance > maxDistance)
		{
			maxDistance = auxDistance;
			threshold = i;
		}
	}

	cv::threshold(floatSrc, dst, threshold, 255, cv::THRESH_BINARY);
	cv::convertScaleAbs(dst, dst);
}

void Algorithm::binarySobel(const cv::Mat& src, cv::Mat& dst, cv::Mat& direction)
{
	if (src.empty() || src.type() != CV_8UC1)
		return;

	cv::Mat x, y, magnitude;

	cv::Sobel(src, x, CV_32F, 1, 0);
	cv::Sobel(src, y, CV_32F, 0, 1);

	cv::cartToPolar(x, y, magnitude, direction, true);

	triangleThresholding(magnitude, dst);
}

void Algorithm::nonMaximumSuppression(const cv::Mat& src, cv::Mat& dst, const cv::Mat& directions)
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

void Algorithm::edgeDetection(const cv::Mat& src, cv::Mat& dst)
{
	if (src.empty() || src.type() != CV_8UC1)
		return;

	cv::Mat sobel, direction;
	binarySobel(src, sobel, direction);

	cv::Mat morphologicalGradient;
	cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
	cv::morphologyEx(src, morphologicalGradient, cv::MORPH_GRADIENT, kernel);

	cv::Mat binary;
	triangleThresholding(morphologicalGradient, binary);

	cv::bitwise_or(sobel, binary, binary);

	cv::bitwise_and(binary, morphologicalGradient, morphologicalGradient);

	nonMaximumSuppression(morphologicalGradient, dst, direction);
}

void Algorithm::bitwiseNand(cv::Mat& src, const cv::Mat& edges)
{
	if (src.empty() || src.type() != CV_8UC1)
		return;

	if (edges.empty() || edges.type() != CV_8UC1)
		return;

	if (src.size() != edges.size())
		return;

	for (int y = 0; y < src.rows; y++)
		for (int x = 0; x < src.cols; x++)
			if (src.ptr<uchar>(y, x)[0] && edges.ptr<uchar>(y, x)[0])
				src.ptr<uchar>(y, x)[0] = 0;
}

void Algorithm::getLargestContour(const std::vector<std::vector<cv::Point>>& contours, std::vector<cv::Point>& largestContour)
{
	if (contours.empty())
		return;

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
}

bool Algorithm::roiContour(const cv::Mat& src, cv::Mat& dst, std::vector<cv::Point>& largestContour, const cv::Mat& edges, const float& percentage)
{
	if (src.empty() || src.type() != CV_8UC1)
		return false;

	if (percentage < 0 || percentage > 1)
		return false;

	cv::Mat otsu;
	cv::threshold(src, otsu, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

	if (!edges.empty())
	{
		cv::Mat dilatedEdges;
		cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(1, 3));
		cv::dilate(edges, dilatedEdges, kernel);
		bitwiseNand(otsu, edges);
	}

	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(otsu, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	getLargestContour(contours, largestContour);

	if (!largestContour.size())
		return false;

	dst = cv::Mat::zeros(otsu.size(), otsu.type());
	cv::drawContours(dst, std::vector<std::vector<cv::Point>>{largestContour}, 0, cv::Scalar(255), cv::FILLED);

	cv::Rect bbox = cv::boundingRect(largestContour);

	return bbox.width > src.cols * percentage;
}

void Algorithm::lineThroughPoint(cv::Vec4f& line, const double& slope, const cv::Point& point, const bool& direction)
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

void Algorithm::compareWithLine(const std::vector<cv::Vec4i>& lines, std::vector<cv::Vec4i>& firstLines, std::vector<cv::Vec4i>& secondLines, const cv::Vec4f& referenceLine)
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

cv::Vec4i Algorithm::initialTerminalPoints(std::vector<cv::Vec4i>& lines, const bool& direction)
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
bool Algorithm::lineSorting(std::vector<cv::Vec4i>& sortedLines, const std::vector<cv::Vec4i>& lines, const cv::Size& size, const cv::Mat& src)
#else
bool Algorithm::lineSorting(std::vector<cv::Vec4i>& sortedLines, const std::vector<cv::Vec4i>& lines, const cv::Size& size)
#endif
{
	if (lines.empty())
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
		return true;;

	cv::Mat drawnReferenceLines;
	cv::cvtColor(src, drawnReferenceLines, cv::COLOR_GRAY2BGR);
	cv::line(drawnReferenceLines, cv::Point(horizontalLine[0], horizontalLine[1]), cv::Point(horizontalLine[2], horizontalLine[3]), cv::Scalar(0, 255, 0), 2);
	cv::line(drawnReferenceLines, cv::Point(verticalLine[0], verticalLine[1]), cv::Point(verticalLine[2], verticalLine[3]), cv::Scalar(0, 255, 0), 2);

	cv::Mat drawnHoughLines;
	cv::cvtColor(src, drawnHoughLines, cv::COLOR_GRAY2BGR);
	for (const cv::Vec4i& line : leftLines)
	{
		cv::Point start(line[0], line[1]);
		cv::Point end(line[2], line[3]);
		cv::line(drawnHoughLines, start, end, cv::Scalar(255, 0, 0), 2);
	}
	for (const cv::Vec4i& line : topLines)
	{
		cv::Point start(line[0], line[1]);
		cv::Point end(line[2], line[3]);
		cv::line(drawnHoughLines, start, end, cv::Scalar(0, 255, 0), 2);
	}
	for (const cv::Vec4i& line : rightLines)
	{
		cv::Point start(line[0], line[1]);
		cv::Point end(line[2], line[3]);
		cv::line(drawnHoughLines, start, end, cv::Scalar(0, 0, 255), 2);
	}
	for (const cv::Vec4i& line : bottomLines)
	{
		cv::Point start(line[0], line[1]);
		cv::Point end(line[2], line[3]);
		cv::line(drawnHoughLines, start, end, cv::Scalar(0, 255, 255), 2);
	}

	cv::Mat drawnSortedLines;
	cv::cvtColor(src, drawnSortedLines, cv::COLOR_GRAY2BGR);
	cv::line(drawnSortedLines, cv::Point(sortedLines[0][0], sortedLines[0][1]), cv::Point(sortedLines[0][2], sortedLines[0][3]), cv::Scalar(255, 0, 0), 2);
	cv::line(drawnSortedLines, cv::Point(sortedLines[1][0], sortedLines[1][1]), cv::Point(sortedLines[1][2], sortedLines[1][3]), cv::Scalar(0, 255, 0), 2);
	cv::line(drawnSortedLines, cv::Point(sortedLines[2][0], sortedLines[2][1]), cv::Point(sortedLines[2][2], sortedLines[2][3]), cv::Scalar(0, 0, 255), 2);
	cv::line(drawnSortedLines, cv::Point(sortedLines[3][0], sortedLines[3][1]), cv::Point(sortedLines[3][2], sortedLines[3][3]), cv::Scalar(0, 255, 255), 2);
#endif

	return true;
}

cv::Point2f Algorithm::intersection(const cv::Vec4i& line1, const cv::Vec4i& line2)
{
	if ((line1[0] == line1[2]) && (line1[1] == line1[3]))
		return cv::Point2f();

	if ((line2[0] == line2[2]) && (line2[1] == line2[3]))
		return cv::Point2f();

	int A1 = line1[3] - line1[1];
	int B1 = line1[0] - line1[2];
	int C1 = A1 * line1[0] + B1 * line1[1];

	int A2 = line2[3] - line2[1];
	int B2 = line2[0] - line2[2];
	int C2 = A2 * line2[0] + B2 * line2[1];

	int determinant = A1 * B2 - A2 * B1;

	if (!determinant)
		return cv::Point2f();

	float x = (B2 * C1 - B1 * C2) / determinant;
	float y = (A1 * C2 - A2 * C1) / determinant;

	return cv::Point2f(x, y);
}

bool Algorithm::cornersCoordinates(const cv::Mat& src, std::vector<cv::Point2f>& quadrilateralCoordinates, const std::vector<cv::Point>& largestContour)
{
	if (src.empty() || src.type() != CV_8UC1)
		return false;

	if (largestContour.empty())
		return false;

	cv::Mat erodedRegionContour;
	cv::erode(src, erodedRegionContour, cv::Mat());
	cv::Mat edges = src - erodedRegionContour;

	cv::RotatedRect rotatedBBox = cv::minAreaRect(largestContour);
	float minLineLenght = rotatedBBox.size.height * 0.25;
	float maxLineGap = rotatedBBox.size.height * 0.1;

	std::vector<cv::Vec4i> lines;
	std::vector<cv::Vec4i> sortedLines;
	do {
		cv::HoughLinesP(edges, lines, 1, CV_PI / 180, 10, minLineLenght, maxLineGap);
		minLineLenght--;
	}
#ifdef _DEBUG
	while (!lineSorting(sortedLines, lines, src.size(), src) && minLineLenght > 0);
#else
	while (!lineSorting(sortedLines, lines, src.size()) && minLineLenght > 0);
#endif

	if (minLineLenght <= 0)
		return false;

	for (int i = 0; i < sortedLines.size(); i++)
		quadrilateralCoordinates.push_back(intersection(sortedLines[i], sortedLines[(i + 1) % 4]));

#ifdef _DEBUG
	cv::Mat drawnQuadrilateralCoordinates;
	cv::cvtColor(src, drawnQuadrilateralCoordinates, cv::COLOR_GRAY2BGR);
	for (const auto& point : quadrilateralCoordinates)
		cv::circle(drawnQuadrilateralCoordinates, point, 4, cv::Scalar(0, 0, 255), -1);
#endif

	return true;
}

bool Algorithm::resizeToPoints(const cv::Mat& src, cv::Mat& dst, std::vector<cv::Point2f>& points, const float& percentage)
{
	if (src.empty() || src.type() != CV_8UC1)
		return false;

	if (points.empty())
		return false;

	if (percentage < 0 || percentage > 1)
		return false;

	int minX = src.cols, minY = src.rows, maxX = 0, maxY = 0;

	for (const auto& point : points)
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

	for (auto& point : points)
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

bool Algorithm::geometricalTransformation(const cv::Mat& src, cv::Mat& dst, const std::vector<cv::Point2f>& quadrilateralCoordinates, const float& percentage)
{
	if (src.empty() || src.type() != CV_8UC1)
		return false;

	if (quadrilateralCoordinates.size() != 4 || percentage < 0)
		return false;

	int height = quadrilateralCoordinates[3].y - quadrilateralCoordinates[0].y;
	int width = 4.3 * height;

	if (height < 0 || width < 0)
		return false;

	if (height < src.rows * percentage)
		return false;

	std::vector<cv::Point2f> finalCoordinates;
	finalCoordinates.push_back(cv::Point2f(0, 0));
	finalCoordinates.push_back(cv::Point2f(width - 1, 0));
	finalCoordinates.push_back(cv::Point2f(width - 1, height - 1));
	finalCoordinates.push_back(cv::Point2f(0, height - 1));

	cv::Mat perspectiveTransform = cv::getPerspectiveTransform(quadrilateralCoordinates, finalCoordinates);
	cv::warpPerspective(src, dst, perspectiveTransform, cv::Size(width, height));

	return cv::countNonZero(dst);
}

void Algorithm::insideContour(const cv::Mat& src, cv::Mat& dst)
{
	if (src.empty() || src.type() != CV_8UC1)
		return;

	cv::Rect crop(1, 1, src.cols - 2, src.rows - 2);
	cv::Mat cropped = src(crop);

	cv::Mat bordered = cropped.clone();
	cv::copyMakeBorder(bordered, bordered, 1, 1, 1, 1, cv::BORDER_CONSTANT, cv::Scalar(255));

	cv::threshold(bordered, dst, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
	dst = 255 - dst;
}

int Algorithm::getContourHeight(const std::vector<cv::Point>& contour)
{
	if (contour.size() < 2)
		return 0;

	int minY = contour[0].y;
	int maxY = contour[0].y;

	for (const cv::Point& point : contour)
	{
		minY = std::min(minY, point.y);
		maxY = std::max(maxY, point.y);
	}

	return maxY - minY;
}

bool Algorithm::compareAreas(const std::vector<cv::Point>& a, const std::vector<cv::Point>& b)
{
	return getContourHeight(a) > getContourHeight(b);
}

int Algorithm::medianHeight(const std::vector<std::vector<cv::Point>>& contours)
{
	if (contours.empty())
		return 0;

	std::vector<int> heights;
	for (const auto& contour : contours)
		heights.push_back(getContourHeight(contour));

	std::sort(heights.begin(), heights.end());
	return heights[heights.size() / 2];
}

bool Algorithm::denoise(const cv::Mat& src, cv::Mat& dst, const float& percentage)
{
	if (src.empty() || src.type() != CV_8UC1)
		return false;

	if (percentage < 0 || percentage > 1)
		return false;

	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(src, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
	std::sort(contours.begin(), contours.end(), compareAreas);

	if (contours.size() > 8)
		contours.resize(8);

	int median = medianHeight(contours);

	for (int i = 0; i < contours.size(); i++)
	{
		int height = getContourHeight(contours[i]);
		if (abs(median - height) > median * percentage)
		{
			contours.erase(contours.begin() + i);
			i--;
		}
	}

	if (contours.size() < 6)
		return false;

	cv::Mat contoursRegion = cv::Mat::zeros(src.size(), src.type());
	for (const auto& contour : contours)
		cv::drawContours(contoursRegion, std::vector<std::vector<cv::Point>>{contour}, 0, cv::Scalar(255), cv::FILLED);

	src.copyTo(dst, contoursRegion);

	std::vector<std::vector<cv::Point>> deletedContours;
	cv::findContours(dst, deletedContours, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE);

	if (deletedContours.size() == contours.size())
		return cv::countNonZero(dst);

	for (int i = 0; i < deletedContours.size(); i++)
		for (int j = 0; j < contours.size(); j++)
			if (deletedContours[i] == contours[j])
			{
				deletedContours.erase(deletedContours.begin() + i);
				i--;
				break;
			}

	for (const auto& contour : deletedContours)
	{
		bool isWhite = dst.ptr<uchar>(contour[0].y, contour[0].x)[0];

		cv::drawContours(dst, std::vector<std::vector<cv::Point>>{contour}, 0, cv::Scalar(0), cv::FILLED);

		if (isWhite)
			cv::drawContours(dst, std::vector<std::vector<cv::Point>>{contour}, 0, cv::Scalar(255));
	}

	return cv::countNonZero(dst);
}

bool compareX(const cv::Rect& a, const cv::Rect& b)
{
	return a.x < b.x;
}

void Algorithm::charsBBoxes(const cv::Mat& src, std::vector<cv::Rect>& chars)
{
	if (src.empty() || src.type() != CV_8UC1)
		return;

	cv::Mat stats;
	std::vector<std::pair<int, int>> areas;
	Algorithm::getConnectedComponents(src, stats, areas);

	for (const auto& area : areas)
	{
		cv::Rect roi;
		int label = area.first;

		Algorithm::getRoi(stats, roi, label);
		chars.push_back(roi);
	}

	std::sort(chars.begin(), chars.end(), compareX);
}

bool Algorithm::firstIndexes(const std::vector<cv::Rect>& chars, std::array<int, 3>& indexes)
{
	if (chars.size() < 6)
		return false;

	indexes[0] = 0;

	int maxDistance = 0;
	for (int i = 0; i < chars.size() - 1; i++)
	{
		int distance = chars[i + 1].x - chars[i].x;
		if (distance > maxDistance)
		{
			maxDistance = distance;
			indexes[1] = i + 1;
		}
	}

	if (indexes[1] < 1 || indexes[1] > 2)
		return false;

	indexes[2] = chars.size() - 3;

	if (indexes[2] - indexes[1] < 2 || indexes[2] - indexes[1] > 3)
		return false;

	return true;
}

void Algorithm::paddingChars(const std::vector<cv::Rect>& src, std::vector<cv::Rect>& dst, const float& percentage)
{
	if (src.empty())
		return;

	if (percentage < 0 || percentage > 1)
		return;

	dst.resize(src.size());

	for (int i = 0; i < src.size(); i++)
		paddingRect(src[i], dst[i], percentage, true);
}

void Algorithm::charsSpacing(const cv::Mat& src, cv::Mat& dst, const std::vector<cv::Rect>& chars, std::vector<cv::Rect>& paddedChars)
{
	if (src.empty() || src.type() != CV_8UC1)
		return;

	if (chars.empty() || paddedChars.empty() || chars.size() != paddedChars.size())
		return;

	std::vector<cv::Mat> ROIs;
	for (int i = 0; i < chars.size(); i++)
	{
		cv::Mat roi = src(chars[i]).clone();

		int padding = (paddedChars[i].width - chars[i].width) / 2;
		int extraPadding = (paddedChars[i].width - chars[i].width) % 2;

		cv::Mat paddedRoi;
		cv::copyMakeBorder(roi, paddedRoi, padding, padding + extraPadding, padding, padding + extraPadding, cv::BORDER_CONSTANT);

		ROIs.push_back(paddedRoi);
	}

	int height = 0;
	for (const auto& rect : paddedChars)
		height = std::max(height, rect.height);

	int width = 0;
	for (auto& rect : paddedChars)
	{
		rect.y = (height - rect.height) / 2;
		rect.x = width;
		width += rect.width;
	}

	dst = cv::Mat::zeros(cv::Size(width + 1, height + 1), src.type());

	for (int i = 0; i < paddedChars.size(); i++)
	{
		if (!ROIs[i].cols || !ROIs[i].rows)
			continue;

		cv::Rect rect(paddedChars[i].x, paddedChars[i].y, ROIs[i].cols, ROIs[i].rows);
		ROIs[i].copyTo(dst(rect));
	}
}
#ifdef _DEBUG
void Algorithm::wordsSeparation(const std::vector<cv::Rect>& chars, std::array<std::vector<cv::Rect>, 3>& words, const std::array<int, 3>& indexes, const cv::Mat src)
#else
void Algorithm::wordsSeparation(const std::vector<cv::Rect>& chars, std::array<std::vector<cv::Rect>, 3>& words, const std::array<int, 3>& indexes)
#endif
{
	if (chars.empty())
		return;

	if (indexes[0] > indexes[1] || indexes[1] > indexes[2])
		return;

	if (chars.size() < indexes[2])
		return;

	words[0].insert(words[0].end(), chars.begin(), chars.begin() + indexes[1]);
	words[1].insert(words[1].end(), chars.begin() + indexes[1], chars.begin() + indexes[2]);
	words[2].insert(words[2].end(), chars.begin() + indexes[2], chars.end());

#ifdef _DEBUG
	if (src.empty() || src.type() != CV_8UC1)
		return;

	cv::Mat drawnChars;
	cv::cvtColor(src, drawnChars, cv::COLOR_GRAY2BGR);
	for (const auto& character : words[0])
		cv::rectangle(drawnChars, character, cv::Scalar(255, 0, 0), 2);
	for (const auto& character : words[1])
		cv::rectangle(drawnChars, character, cv::Scalar(0, 255, 0), 2);
	for (const auto& character : words[2])
		cv::rectangle(drawnChars, character, cv::Scalar(0, 0, 255), 2);
#endif
}

tesseract::TessBaseAPI& Algorithm::getTessInstance()
{
	static tesseract::TessBaseAPI tess;
	static bool isInitialized = false;

	if (!isInitialized)
	{
		tess.Init(NULL, "DIN1451Mittelschrift", tesseract::OEM_LSTM_ONLY);
		isInitialized = true;
	}

	return tess;
}

bool Algorithm::verifyOutputText(tesseract::TessBaseAPI& tess, float& confidence)
{
	if (!tess.GetUTF8Text())
		return false;

	std::string text = tess.GetUTF8Text();

	if (text.size() != 2)
		return false;

	tess.Recognize(0);
	tesseract::ResultIterator* iterator = tess.GetIterator();
	confidence = confidence + iterator->Confidence(tesseract::RIL_SYMBOL);

	return true;
}

bool Algorithm::resizeCharTemplate(const cv::Mat& src, cv::Mat& dst, const cv::Size& size)
{
	if (src.empty() || src.type() != CV_8UC1)
		return false;

	if (size.width < 3 || size.height < 3)
		return false;

	double aspectRation = static_cast<float>(src.cols) / src.rows;
	int width = round(size.height * aspectRation);

	cv::Mat resizedSrc;
	cv::resize(src, resizedSrc, cv::Size(width, size.height));
	cv::threshold(resizedSrc, resizedSrc, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

	std::vector<std::vector<cv::Point>> contour;
	cv::findContours(resizedSrc, contour, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	if (contour.empty())
		return false;

	cv::Rect bbox(cv::boundingRect(contour[0]));
	bbox.x = std::max(0, bbox.x - 1);
	bbox.y = std::max(0, bbox.y - 1);
	bbox.width = std::min(resizedSrc.cols - bbox.x, bbox.width + 2);
	bbox.height = std::min(resizedSrc.rows - bbox.y, bbox.height + 2);

	dst = cv::Mat::zeros(cv::Size(bbox.width + 1, bbox.height + 1), resizedSrc.type());
	resizedSrc(bbox).copyTo(dst);

	return true;
}

void Algorithm::padding(const int& firstSize, const int& secondSize, int& firstPadding, int& secondPadding)
{
	if (firstSize < 0 && secondSize < 0)
		return;

	if (firstSize < secondSize)
		return;

	firstPadding = (firstSize - secondSize) / 2;
	secondPadding = firstPadding;
	if ((firstSize - secondSize) % 2)
		secondPadding++;
}

bool Algorithm::matching(const cv::Mat& src, float& dice, const float& percentage)
{
	if (src.empty() || src.type() != CV_8UC1)
		return false;

	if (percentage < 0)
		return false;

	cv::Mat charTemplate = cv::imread("../../../assets/i.jpg", cv::IMREAD_GRAYSCALE);

	if (charTemplate.empty())
		charTemplate = cv::imread("assets/i.jpg", cv::IMREAD_GRAYSCALE);

	cv::Mat resizedCharTemplate;
	if (!resizeCharTemplate(charTemplate, resizedCharTemplate, src.size()))
		return false;

	int width = std::max(src.cols, resizedCharTemplate.cols);
	cv::Size size(width, src.rows);

	int paddingTop, paddingBottom;
	padding(size.height, resizedCharTemplate.rows, paddingTop, paddingBottom);

	int paddingLeft, paddingRight;
	padding(size.width, resizedCharTemplate.cols, paddingLeft, paddingRight);

	cv::copyMakeBorder(resizedCharTemplate, resizedCharTemplate, paddingTop, paddingBottom, paddingLeft, paddingRight, cv::BORDER_CONSTANT);

	padding(size.width, src.cols, paddingLeft, paddingRight);

	cv::Mat resizedSrc = src.clone();
	cv::copyMakeBorder(src, resizedSrc, 0, 0, paddingLeft, paddingRight, cv::BORDER_CONSTANT);

	cv::Mat intersection;
	cv::bitwise_and(resizedSrc, resizedCharTemplate, intersection);

	dice = (2.0 * cv::countNonZero(intersection)) / (cv::countNonZero(resizedSrc) + cv::countNonZero(resizedCharTemplate));

	return dice > percentage;
}

bool Algorithm::applyTesseract(const cv::Mat& src, std::string& text, const std::vector<cv::Rect>& chars, std::vector<cv::Rect>& paddedChars, const bool& charType, float& confidence)
{
	if (src.empty() || src.type() != CV_8UC1)
		return false;

	if (!chars.size() || !paddedChars.size())
		return false;

	if (chars.size() != paddedChars.size())
		return false;

	tesseract::TessBaseAPI tess;
	tess.Init(NULL, "DIN1451Mittelschrift", tesseract::OEM_LSTM_ONLY);

	tess.SetVariable("load_bigram_dawg", "false");
	tess.SetVariable("load_freq_dawg", "false");
	tess.SetVariable("load_number_dawg", "false");
	tess.SetVariable("load_punc_dawg", "false");
	tess.SetVariable("load_system_dawg", "false");
	tess.SetVariable("load_unambig_dawg", "false");
	tess.SetVariable("load_word_dawg", "false");

	if (charType)
		tess.SetVariable("tessedit_char_whitelist", "0123456789");
	else
		tess.SetVariable("tessedit_char_whitelist", "ABCDEFGHIJKLMNOPQRSTUVWXYZ");

	tess.SetImage(src.data, src.cols, src.rows, 1, src.step);


#ifdef _DEBUG
	cv::Mat drawnPaddedChars;
	cv::cvtColor(src, drawnPaddedChars, cv::COLOR_GRAY2BGR);
#endif

	for (int i = 0; i < paddedChars.size(); i++)
	{
		if (paddedChars[i].width >= src.cols)
			continue;

		if (paddedChars[i].height >= src.rows)
			continue;

		while (paddedChars[i].width >= chars[i].width + 3 && paddedChars[i].height >= chars[i].height + 3)
		{
			tess.SetRectangle(paddedChars[i].x, paddedChars[i].y, paddedChars[i].width, paddedChars[i].height);
			paddedChars[i].x++;
			paddedChars[i].y++;
			paddedChars[i].width -= 2;
			paddedChars[i].height -= 2;

			if (verifyOutputText(tess, confidence))
				break;
		}

#ifdef _DEBUG
		cv::rectangle(drawnPaddedChars, paddedChars[i], cv::Scalar(0, 255, 0), 2);
#endif

		std::string result = tess.GetUTF8Text();

		if (result.empty())
		{
			cv::Mat roi = src(paddedChars[i]).clone();

			float dice;
			if (matching(roi, dice, 0.8))
			{
				result = "I";
				confidence += dice * 100;
			}
		}

		text = text + result;
	}

	return true;
}

bool Algorithm::readText(const cv::Mat& src, std::string& text, float& confidence, const std::array<std::vector<cv::Rect>, 3>& words, std::array<std::vector<cv::Rect>, 3>& paddedWords)
{
	if (src.empty() || src.type() != CV_8UC1)
		return false;

	for (int i = 0; i < words.size(); i++)
	{
		if (!words[i].size() || !paddedWords[i].size())
			return false;

		if (words[i].size() != paddedWords[i].size())
			return false;
	}

	if (!applyTesseract(src, text, words[0], paddedWords[0], 0, confidence))
		return false;
	if (!applyTesseract(src, text, words[1], paddedWords[1], 1, confidence))
		return false;
	if (!applyTesseract(src, text, words[2], paddedWords[2], 0, confidence))
		return false;

	text.erase(std::remove(text.begin(), text.end(), '\n'), text.end());

	confidence = confidence / text.size();

	return true;
}

void Algorithm::drawBBoxes(cv::Mat& dst, cv::Rect& roi, std::string& dateTime, const std::string& text, const float& confidence)
{
	if (text.empty() || confidence < 0)
		return;

	if (!dst.empty() && dst.type() != CV_8UC3)
		return;

	if (!roi.empty() && (roi.x < 0 && roi.y < 0 && roi.height <= 0 && roi.height <= 0))
		return;

	auto now = std::chrono::system_clock::now();
	auto toTimeT = std::chrono::system_clock::to_time_t(now);

	std::stringstream ss;
	ss << std::put_time(std::localtime(&toTimeT), "%d-%m-%Y %X");

	dateTime = ss.str();

	if (dst.empty())
		return;

	std::ostringstream stream;
	stream << std::fixed << std::setprecision(2) << confidence;
	std::string displayText = dateTime + " / " + text + " / " + "Score: " + stream.str();

	cv::putText(dst, displayText, cv::Point(10, dst.rows - 30), cv::FONT_HERSHEY_SIMPLEX, 4, cv::Scalar(0, 0, 0), 18, cv::LINE_AA);
	cv::putText(dst, displayText, cv::Point(10, dst.rows - 30), cv::FONT_HERSHEY_SIMPLEX, 4, cv::Scalar(255, 255, 255), 9, cv::LINE_AA);

	if (roi.empty())
		return;

	roi.x = roi.x * 2 + dst.cols * 0.1;
	roi.y = roi.y * 2 + dst.rows / 2;
	roi.width = roi.width * 2;
	roi.height = roi.height * 2;

	cv::rectangle(dst, roi, cv::Scalar(0, 255, 0), 5);
}

std::string textFromImage(const std::string& srcPath, const std::string& dstPath)
{
	cv::Mat src, dst;
	std::string dateTime;
	std::string plate;
	float confidence = 0;

	src = cv::imread(srcPath, cv::IMREAD_COLOR);
	if (src.empty() || (src.type() != CV_8UC4 && src.type() != CV_8UC3))
	{
		plate = "N/A";
		Algorithm::drawBBoxes(cv::Mat(), cv::Rect(), dateTime, plate, confidence);
		return plate + "\n" + dateTime;
	}

	cv::Mat bgrSrc;
	if (src.type() == CV_8UC4)
		cv::cvtColor(src, bgrSrc, cv::COLOR_BGRA2BGR);
	else
		bgrSrc = src.clone();

	dst = bgrSrc.clone();

	cv::resize(bgrSrc, bgrSrc, cv::Size(bgrSrc.cols / 2, bgrSrc.rows / 2));

	cv::Rect roi(bgrSrc.cols * 0.1, bgrSrc.rows / 2, bgrSrc.cols - bgrSrc.cols * 0.1, bgrSrc.rows / 2);
	cv::Mat cropped = bgrSrc(roi);

	cv::Mat gauss;
	cv::GaussianBlur(cropped, gauss, cv::Size(3, 3), 0);

	cv::Mat hsv;
	Algorithm::BGR2HSV(gauss, hsv);

	cv::Mat binary;
	Algorithm::HSV2Binary(hsv, binary, 125);

	cv::Mat stats;
	std::vector<std::pair<int, int>> areas;
	Algorithm::getConnectedComponents(binary, stats, areas, 10);

	cv::Rect roiConnectedComponent;
	for (int i = 0; i < areas.size(); i++)
	{
		cv::Rect roi;
		int label = areas[i].first;
		Algorithm::getRoi(stats, roi, label);

#ifdef _DEBUG
		cv::Mat colorConnectedComponent = cropped(roi);
#endif

		if (!Algorithm::sizeBBox(cropped, roi, 0.01, 0.15) || !Algorithm::heightBBox(roi, 0.2, 0.9))
			continue;

		Algorithm::paddingRect(roi, roi, 0.05, false, cropped.size());

		cv::Mat hsvConnectedComponent = hsv(roi);
		Algorithm::blueToBlack(hsvConnectedComponent, hsvConnectedComponent);

		cv::Mat bgrConnectedComponent;
		Algorithm::HSV2BGR(hsvConnectedComponent, bgrConnectedComponent);

		cv::Mat grayConnectedComponent;
		cv::cvtColor(bgrConnectedComponent, grayConnectedComponent, cv::COLOR_BGR2GRAY);

		cv::Mat connectedComponent = cropped(roi);
		cv::cvtColor(connectedComponent, connectedComponent, cv::COLOR_BGR2GRAY);

		cv::Mat edges;
		cv::threshold(grayConnectedComponent, edges, 0, 255, cv::THRESH_BINARY);
		edges = 255 - edges;
		Algorithm::edgeDetection(connectedComponent, edges);

		cv::Mat regionContour;
		std::vector<cv::Point> largestContour;
		if (!Algorithm::roiContour(connectedComponent, regionContour, largestContour, edges, 0.8))
			continue;

		cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
		cv::erode(regionContour, regionContour, kernel);
		cv::dilate(regionContour, regionContour, kernel);

		Algorithm::roiContour(regionContour, regionContour, largestContour);

		std::vector<cv::Point2f> quadrilateralCoordinates;
		if (!Algorithm::cornersCoordinates(regionContour, quadrilateralCoordinates, largestContour))
			continue;

		cv::Mat resizedConnectedComponent;
		if (!Algorithm::resizeToPoints(connectedComponent, resizedConnectedComponent, quadrilateralCoordinates, 0.2))
			continue;

		cv::Mat transformedConnectedComponent;
		if (!Algorithm::geometricalTransformation(resizedConnectedComponent, transformedConnectedComponent, quadrilateralCoordinates, 0.2))
			continue;

		cv::Mat textConnectedComponent;
		Algorithm::insideContour(transformedConnectedComponent, textConnectedComponent);

		cv::Mat denoiseConnectedComponent;
		if (!Algorithm::denoise(textConnectedComponent, denoiseConnectedComponent, 0.15))
			continue;

		cv::dilate(denoiseConnectedComponent, denoiseConnectedComponent, cv::Mat());
		cv::erode(denoiseConnectedComponent, denoiseConnectedComponent, cv::Mat());

		if (!Algorithm::denoise(denoiseConnectedComponent, denoiseConnectedComponent, 0.15))
			continue;

		std::vector<cv::Rect> chars;
		Algorithm::charsBBoxes(denoiseConnectedComponent, chars);

		std::array<int, 3> indexes;
		if (!Algorithm::firstIndexes(chars, indexes))
			continue;

		std::vector<cv::Rect> paddedChars;
		Algorithm::paddingChars(chars, paddedChars, 0.6);

		cv::Mat spacedConnectedComponent;
		Algorithm::charsSpacing(denoiseConnectedComponent, spacedConnectedComponent, chars, paddedChars);

		std::array<std::vector<cv::Rect>, 3> words;
#ifdef _DEBUG
		Algorithm::wordsSeparation(chars, words, indexes, denoiseConnectedComponent);
#else
		Algorithm::wordsSeparation(chars, words, indexes);
#endif

		std::array<std::vector<cv::Rect>, 3> paddedWords;
#ifdef _DEBUG
		Algorithm::wordsSeparation(paddedChars, paddedWords, indexes, spacedConnectedComponent);
#else
		Algorithm::wordsSeparation(paddedChars, paddedWords, indexes);
#endif

		if (!Algorithm::readText(spacedConnectedComponent, plate, confidence, words, paddedWords))
			continue;

		roiConnectedComponent = roi;
		break;
	}

	if (plate.empty())
		plate = "N/A";

	Algorithm::drawBBoxes(dst, roiConnectedComponent, dateTime, plate, confidence);

	if (!dstPath.empty())
		cv::imwrite(dstPath, dst);

	return plate + "\n" + dateTime;
}
