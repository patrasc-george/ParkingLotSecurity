#include "ImageProcessingUtils.h"

void Algorithm::BGR2HSV(const cv::Mat& src, cv::Mat& dst)
{
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

void Algorithm::HSV2Binary(const cv::Mat& src, cv::Mat& dst, const uchar& threshold = 125)
{
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

void Algorithm::getConnectedComponents(const cv::Mat& src, cv::Mat& stats, std::vector<std::pair<int, int>>& areas, const int& newSize = 0)
{
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
	roi.x = stats.ptr<int>(label, cv::CC_STAT_LEFT)[0];
	roi.y = stats.ptr<int>(label, cv::CC_STAT_TOP)[0];
	roi.width = stats.ptr<int>(label, cv::CC_STAT_WIDTH)[0];
	roi.height = stats.ptr<int>(label, cv::CC_STAT_HEIGHT)[0];
}

bool Algorithm::sizeBBox(const cv::Mat& src, const cv::Rect& roi, const float& min = 0, const float& max = 1)
{
	int areaConnectedComponent = roi.height * roi.width;
	int areaSrc = src.rows * src.cols;

	return areaConnectedComponent > areaSrc * min &&
		areaConnectedComponent < areaSrc * max;
}

bool Algorithm::heightBBox(const cv::Rect& roi, const float& min = 0, const float& max = 1)
{
	return roi.height > roi.width * min &&
		roi.height < roi.width * max;
}

void Algorithm::paddingRect(const cv::Rect& src, cv::Rect& dst, const float& percent = 0, const bool& square = 0, const cv::Size& size = cv::Size())
{
	int paddingX = src.width * percent;
	if (paddingX < 3)
		paddingX = 3;

	int paddingY = src.height * percent;
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
	double maxVal;
	cv::minMaxLoc(src, NULL, &maxVal);
	int histSize = static_cast<int>(maxVal) + 1;

	std::vector<float> range = { 0, static_cast<float>(histSize) };
	const float* ranges[] = { range.data() };

	cv::calcHist(&src, 1, 0, cv::Mat(), hist, 1, &histSize, ranges);
}

void Algorithm::cumulativeHistogram(const cv::Mat& hist, cv::Mat& cumulvativeHist)
{
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
	float a = line[1] - line[3];
	float b = line[2] - line[0];
	float c = (line[0] * line[3]) - (line[2] * line[1]);

	return abs((a * x + b * y + c) / sqrt(a * a + b * b));
}

void Algorithm::triangleThresholding(const cv::Mat& src, cv::Mat& dst)
{
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
	cv::Mat sobel, x, y, magnitude;

	cv::Sobel(src, x, CV_32F, 1, 0);
	cv::Sobel(src, y, CV_32F, 0, 1);

	cv::cartToPolar(x, y, magnitude, direction, true);

	triangleThresholding(magnitude, dst);
}

void Algorithm::nonMaximumSuppression(const cv::Mat& src, cv::Mat& dst, const cv::Mat& directions)
{
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
	cv::Mat sobel, direction;
	binarySobel(src, sobel, direction);

	cv::Mat morphologicalGradient;
	cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
	cv::morphologyEx(src, morphologicalGradient, cv::MORPH_GRADIENT, kernel);

	cv::Mat binary;
	triangleThresholding(morphologicalGradient, binary);

	cv::bitwise_or(sobel, binary, binary);

	cv::bitwise_and(sobel, morphologicalGradient, morphologicalGradient);

	nonMaximumSuppression(morphologicalGradient, dst, direction);
}

void Algorithm::bitwiseNand(cv::Mat& src, const cv::Mat& edges)
{
	for (int y = 0; y < src.rows; y++)
		for (int x = 0; x < src.cols; x++)
			if (src.ptr<uchar>(y, x)[0] && edges.ptr<uchar>(y, x)[0])
				src.ptr<uchar>(y, x)[0] = 0;
}

void Algorithm::getLargestContour(const std::vector<std::vector<cv::Point>>& contours, std::vector<cv::Point>& largestContour)
{
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

bool Algorithm::roiContour(const cv::Mat& src, cv::Mat& dst, std::vector<cv::Point>& largestContour, const cv::Mat& edges = cv::Mat(), const float& percent = 0)
{
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

	dst = cv::Mat::zeros(otsu.size(), otsu.type());
	cv::drawContours(dst, std::vector<std::vector<cv::Point>>{largestContour}, 0, cv::Scalar(255), cv::FILLED);

	cv::Rect bbox = cv::boundingRect(largestContour);

	return bbox.width > src.cols * percent;
}

void Algorithm::lineThroughPoint(cv::Vec4f& line, const double& slope, const cv::Point& point, const bool& direction)
{
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
	cv::Vec4i segment = lines[0];
	for (auto& line : lines)
	{
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

bool Algorithm::lineSorting(std::vector<cv::Vec4i>& sortedLines, const std::vector<cv::Vec4i>& lines, const cv::Size& size)
{
	float horizontalSlope = 0;

	std::vector<cv::Vec4i> verticalLines;
	std::vector<cv::Vec4i> horizontalLines;

	for (const auto& line : lines)
	{
		if (line[2] == line[0])
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

	return true;
}

cv::Point2f Algorithm::intersection(const cv::Vec4i& line1, const cv::Vec4i& line2)
{
	int A1 = line1[3] - line1[1];
	int B1 = line1[0] - line1[2];
	int C1 = A1 * line1[0] + B1 * line1[1];

	int A2 = line2[3] - line2[1];
	int B2 = line2[0] - line2[2];
	int C2 = A2 * line2[0] + B2 * line2[1];

	int det = A1 * B2 - A2 * B1;

	float x = (B2 * C1 - B1 * C2) / det;
	float y = (A1 * C2 - A2 * C1) / det;

	return cv::Point2f(x, y);
}

bool Algorithm::cornersCoordinates(const cv::Mat& src, std::vector<cv::Point2f>& quadrilateralCoordinates, const std::vector<cv::Point>& largestContour)
{
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
	} while (!lineSorting(sortedLines, lines, src.size()) && minLineLenght > 0);

	if (minLineLenght <= 0)
		return false;

	for (int i = 0; i < sortedLines.size(); i++)
		quadrilateralCoordinates.push_back(intersection(sortedLines[i], sortedLines[(i + 1) % 4]));

	return true;
}

bool Algorithm::resizeToPoints(const cv::Mat& src, cv::Mat& dst, std::vector<cv::Point2f>& points, const float& percent = 0)
{
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

	if (dst.rows > src.rows + (src.rows * percent))
		return false;

	if (dst.cols > src.cols + (src.cols * percent))
		return false;

	return true;
}

bool Algorithm::geometricalTransformation(const cv::Mat& src, cv::Mat& dst, const std::vector<cv::Point2f>& quadrilateralCoordinates, const float& percent = 0)
{
	int height = quadrilateralCoordinates[3].y - quadrilateralCoordinates[0].y;
	int width = 4.3 * height;

	if (height < 0 || width < 0)
		return false;

	if (height < src.rows * percent)
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

bool Algorithm::insideContour(const cv::Mat& src, cv::Mat& dst, const cv::Mat& regionContour)
{
	cv::Mat otsu;
	cv::threshold(src, otsu, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

	otsu = 255 - otsu;

	dst = cv::Mat::zeros(otsu.size(), otsu.type());
	otsu.copyTo(dst, regionContour);

	return cv::countNonZero(dst);
}

bool compareAreas(const std::vector<cv::Point>& a, const std::vector<cv::Point>& b)
{
	return cv::contourArea(a) > cv::contourArea(b);
}

int Algorithm::getContourHeight(const std::vector<cv::Point>& contour)
{
	int minY = contour[0].y;
	int maxY = contour[0].y;

	for (const cv::Point& point : contour)
	{
		minY = std::min(minY, point.y);
		maxY = std::max(maxY, point.y);
	}

	return maxY - minY;
}

int Algorithm::medianHeight(const std::vector<std::vector<cv::Point>>& contours)
{
	std::vector<int> heights;
	for (const auto& contour : contours)
		heights.push_back(getContourHeight(contour));

	std::sort(heights.begin(), heights.end());
	return heights[heights.size() / 2];
}

bool Algorithm::denoise(const cv::Mat& src, cv::Mat& dst, const float& percent = 1)
{
	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(src, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
	std::sort(contours.begin(), contours.end(), compareAreas);

	if (contours.size() > 8)
		contours.resize(8);

	int median = medianHeight(contours);

	for (int i = 0; i < contours.size(); i++)
	{
		int height = getContourHeight(contours[i]);
		if (abs(median - height) > median * percent)
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

	return indexes[1] < indexes[2];
}

void Algorithm::paddingChars(const std::vector<cv::Rect>& src, std::vector<cv::Rect>& dst, const float& percent = 0)
{
	dst.resize(src.size());

	for (int i = 0; i < src.size(); i++)
		paddingRect(src[i], dst[i], percent, true);

	int paddingX = 0, paddingY = 0;

	for (const auto& rect : dst)
	{
		paddingX = std::min(paddingX, rect.x);
		paddingY = std::min(paddingY, rect.y);
	}

	paddingX = -paddingX;
	paddingY = -paddingY;

	for (auto& rect : dst)
	{
		rect.x += paddingX;
		rect.y += paddingY;
	}
}

void Algorithm::charsSpacing(const cv::Mat& src, cv::Mat& dst, std::vector<cv::Rect>& chars, std::vector<cv::Rect>& paddedChars)
{
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

	dst = cv::Mat::zeros(cv::Size(width, height), src.type());

	for (int i = 0; i < paddedChars.size(); i++)
	{
		cv::Rect rect(paddedChars[i].x, paddedChars[i].y, ROIs[i].cols, ROIs[i].rows);
		ROIs[i].copyTo(dst(rect));
	}
}

void Algorithm::wordsSeparation(const std::vector<cv::Rect>& chars, std::array<std::vector<cv::Rect>, 3>& words, const std::array<int, 3>& indexes)
{
	words[0].insert(words[0].end(), chars.begin(), chars.begin() + indexes[1]);
	words[1].insert(words[1].end(), chars.begin() + indexes[1], chars.begin() + indexes[2]);
	words[2].insert(words[2].end(), chars.begin() + indexes[2], chars.end());
}

bool Algorithm::verifyOutputText(tesseract::TessBaseAPI& tess, float& confidence)
{
	std::string text = tess.GetUTF8Text();

	if (text.size() != 2)
		return false;

	tess.Recognize(0);
	tesseract::ResultIterator* iterator = tess.GetIterator();
	confidence = confidence + iterator->Confidence(tesseract::RIL_SYMBOL);

	return true;
}

void Algorithm::resizeCharTemplate(const cv::Mat& src, cv::Mat& dst, const cv::Size& size)
{
	double aspectRadion = static_cast<float>(src.cols) / src.rows;
	int width = size.height * aspectRadion;

	cv::Mat resizedSrc;
	cv::resize(src, resizedSrc, cv::Size(width, size.height));
	cv::threshold(resizedSrc, resizedSrc, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

	std::vector<std::vector<cv::Point>> contour;
	cv::findContours(resizedSrc, contour, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	cv::Rect bbox(cv::boundingRect(contour[0]));
	bbox.x--;
	bbox.y--;
	bbox.width += 2;
	bbox.height += 2;

	dst = cv::Mat::zeros(cv::Size(bbox.width + 1, bbox.height + 1), resizedSrc.type());
	resizedSrc(bbox).copyTo(dst);
}

void Algorithm::padding(const int& firstSize, const int& secondSize, int& firstPadding, int& secondPadding)
{
	firstPadding = (firstSize - secondSize) / 2;
	secondPadding = firstPadding;
	if ((firstSize - secondSize) % 2)
		secondPadding++;
}

bool Algorithm::matching(const cv::Mat& src, float& dice, const float& percent = 0)
{
	cv::Mat charTemplate = cv::imread("../../../database/i.jpg", cv::IMREAD_GRAYSCALE);

	cv::Mat resizedCharTemplate;
	resizeCharTemplate(charTemplate, resizedCharTemplate, src.size());

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

	return dice > percent;
}

bool Algorithm::applyTesseract(const cv::Mat& src, std::string& text, const std::vector<cv::Rect>& chars, std::vector<cv::Rect>& paddedChars, const bool& charType, float& confidence)
{
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

	for (int i = 0; i < paddedChars.size(); i++)
	{
		do
		{
			tess.SetRectangle(paddedChars[i].x, paddedChars[i].y, paddedChars[i].width, paddedChars[i].height);
			paddedChars[i].x++;
			paddedChars[i].y++;
			paddedChars[i].width -= 2;
			paddedChars[i].height -= 2;
		} while (!verifyOutputText(tess, confidence) && (paddedChars[i].width >= chars[i].width + 3 && paddedChars[i].height >= chars[i].height + 3));

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

void Algorithm::drawBBoxes(cv::Mat& dst, cv::Rect& roi, std::string& time, const std::string& text, const float& confidence)
{
	auto now = std::chrono::system_clock::now();
	auto toTimeT = std::chrono::system_clock::to_time_t(now);

	std::stringstream ss;
	ss << std::put_time(std::localtime(&toTimeT), "%d-%m-%Y %X");

	time = ss.str();

	std::ostringstream stream;
	stream << std::fixed << std::setprecision(2) << confidence;
	std::string displayText = time + " / " + text + " / " + "Score: " + stream.str();

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

std::string textFromImage(const cv::Mat& src, cv::Mat& dst)
{
	cv::Mat bgrSrc;
	if (src.type() == CV_8UC4)
		cv::cvtColor(src, bgrSrc, cv::COLOR_BGRA2BGR);
	else
		bgrSrc = src.clone();

	cv::Mat bgrDst = bgrSrc.clone();

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
	std::string time;
	std::string plate;
	float confidence = 0;
	for (int i = 0; i < areas.size(); i++)
	{
		cv::Rect roi;
		int label = areas[i].first;
		Algorithm::getRoi(stats, roi, label);

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

		cv::Rect crop(1, 1, transformedConnectedComponent.cols - 2, transformedConnectedComponent.rows - 2);
		transformedConnectedComponent = transformedConnectedComponent(crop);
		cv::Mat borderedConnectedComponent = transformedConnectedComponent.clone();
		cv::copyMakeBorder(borderedConnectedComponent, borderedConnectedComponent, 1, 1, 1, 1, cv::BORDER_CONSTANT, cv::Scalar(255));

		Algorithm::roiContour(borderedConnectedComponent, regionContour, largestContour);

		cv::Mat textConnectedComponent;
		if (!Algorithm::insideContour(borderedConnectedComponent, textConnectedComponent, regionContour))
			continue;

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
		Algorithm::wordsSeparation(chars, words, indexes);

		std::array<std::vector<cv::Rect>, 3> paddedWords;
		Algorithm::wordsSeparation(paddedChars, paddedWords, indexes);

		if (!Algorithm::readText(spacedConnectedComponent, plate, confidence, words, paddedWords))
			continue;

		roiConnectedComponent = roi;
	}

	if (plate.empty())
		plate = "necunoscut";

	Algorithm::drawBBoxes(bgrDst, roiConnectedComponent, time, plate, confidence);

	cv::cvtColor(bgrDst, dst, cv::COLOR_BGR2RGB);

	return plate + "\n" + time;
}
