#include "ImageProcessingUtils.h"

void algorithm::BGR2HSV(const cv::Mat& src, cv::Mat& dst)
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

void algorithm::HSV2Binary(const cv::Mat& src, cv::Mat& dst, const uchar& threshold = 125)
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

void algorithm::getConnectedComponents(const cv::Mat& src, cv::Mat& stats, std::vector<std::pair<int, int>>& areas, const int& newSize = 0)
{
	cv::Mat labels, cendtroids;
	int size = cv::connectedComponentsWithStats(src, labels, stats, cendtroids);

	for (int i = 1; i < size; i++)
		areas.push_back(std::make_pair(i, stats.ptr<int>(i, cv::CC_STAT_AREA)[0]));

	std::sort(areas.begin(), areas.end(), compareConnectedComponents);

	if (newSize && areas.size() > newSize)
		areas.resize(newSize);
}

void algorithm::getRoi(const cv::Mat& stats, cv::Rect& roi, const int& label)
{
	roi.x = stats.ptr<int>(label, cv::CC_STAT_LEFT)[0];
	roi.y = stats.ptr<int>(label, cv::CC_STAT_TOP)[0];
	roi.width = stats.ptr<int>(label, cv::CC_STAT_WIDTH)[0];
	roi.height = stats.ptr<int>(label, cv::CC_STAT_HEIGHT)[0];
}

bool algorithm::sizeBBox(const cv::Mat& src, const cv::Rect& roi, const float& min = 0, const float& max = 1)
{
	int areaConnectedComponent = roi.height * roi.width;
	int areaSrc = src.rows * src.cols;

	return areaConnectedComponent > areaSrc * min &&
		areaConnectedComponent < areaSrc * max;
}

bool algorithm::heightBBox(const cv::Rect& roi, const float& min = 0, const float& max = 1)
{
	return roi.height > roi.width * min &&
		roi.height < roi.width * max;
}

void algorithm::paddingRect(const cv::Rect& src, cv::Rect& dst, const cv::Size& size, const float& percent = 0)
{
	int paddingX = src.width * percent;
	if (paddingX < 3)
		paddingX = 3;

	int paddingY = src.height * percent;
	if (paddingY < 3)
		paddingY = 3;

	dst.x = std::max(src.x - paddingX, 0);
	dst.y = std::max(src.y - paddingY, 0);
	dst.width = std::min(src.width + paddingX * 2, size.width - dst.x);
	dst.height = std::min(src.height + paddingY * 2, size.height - dst.y);
}

void algorithm::blueToBlack(const cv::Mat& src, cv::Mat& dst)
{
	dst = src.clone();

	for (int y = 0; y < src.rows; y++)
		for (int x = 0; x < src.cols; x++)
			if (src.ptr<uchar>(y, x)[0] > 100 && src.ptr<uchar>(y, x)[0] < 135 &&
				src.ptr<uchar>(y, x)[1] > 100 &&
				src.ptr<uchar>(y, x)[2] > 25 && src.ptr<uchar>(y, x)[2] < 230)
			{
				dst.ptr<uchar>(y, x)[0] = 0;
				dst.ptr<uchar>(y, x)[1] = 0;
				dst.ptr<uchar>(y, x)[2] = 0;
			}
}

void algorithm::HSV2BGR(const cv::Mat& src, cv::Mat& dst)
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

void algorithm::histogram(const cv::Mat& src, cv::Mat& hist)
{
	double maxVal;
	cv::minMaxLoc(src, NULL, &maxVal);
	int histSize = static_cast<int>(maxVal) + 1;

	std::vector<float> range = { 0, static_cast<float>(histSize) };
	const float* ranges[] = { range.data() };

	cv::calcHist(&src, 1, 0, cv::Mat(), hist, 1, &histSize, ranges);
}

void algorithm::cumulativeHistogram(const cv::Mat& hist, cv::Mat& cumulvativeHist)
{
	cumulvativeHist = cv::Mat(cv::Size(hist.rows, hist.cols), hist.type());

	float sum = 0;
	for (int i = 0; i < hist.rows; i++)
	{
		sum = sum + hist.ptr<float>(i, 0)[0];
		cumulvativeHist.ptr<float>(0, i)[0] = sum;
	}
}

void algorithm::histogramLine(const cv::Mat& cumulvativeHist, cv::Vec4f& line)
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

float algorithm::distance(const float& x, const float& y, const cv::Vec4f& line)
{
	float a = line[1] - line[3];
	float b = line[2] - line[0];
	float c = (line[0] * line[3]) - (line[2] * line[1]);

	return abs((a * x + b * y + c) / sqrt(a * a + b * b));
}

void algorithm::binaryThresholding(const cv::Mat& src, cv::Mat& dst, const int& threshold)
{
	dst = cv::Mat::zeros(src.size(), CV_8UC1);

	for (int y = 0; y < src.rows; y++)
		for (int x = 0; x < src.cols; x++)
			if (src.ptr<float>(y, x)[0] > threshold)
				dst.ptr<uchar>(y, x)[0] = 255;
}

void algorithm::triangleThresholding(const cv::Mat& src, cv::Mat& dst)
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

	binaryThresholding(floatSrc, dst, threshold);
}

void algorithm::binarySobel(const cv::Mat& src, cv::Mat& dst, cv::Mat& direction)
{
	cv::Mat sobel, x, y, magnitude;

	cv::Sobel(src, x, CV_32F, 1, 0);
	cv::Sobel(src, y, CV_32F, 0, 1);

	cv::cartToPolar(x, y, magnitude, direction, true);

	triangleThresholding(magnitude, dst);
}

void algorithm::nonMaximumSuppression(const cv::Mat& src, cv::Mat& dst, const cv::Mat& directions)
{
	dst = cv::Mat::zeros(src.size(), CV_8UC1);

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

void algorithm::edgeDetection(const cv::Mat& src, cv::Mat& dst)
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

void algorithm::bitwiseNand(cv::Mat& src, const cv::Mat& edges)
{
	for (int y = 0; y < src.rows; y++)
		for (int x = 0; x < src.cols; x++)
			if (src.ptr<uchar>(y, x)[0] && edges.ptr<uchar>(y, x)[0])
				src.ptr<uchar>(y, x)[0] = 0;
}

void algorithm::getLargestContour(const std::vector<std::vector<cv::Point>>& contours, std::vector<cv::Point>& largestContour)
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

void algorithm::roiContour(const cv::Mat& src, cv::Mat& dst, std::vector<cv::Point>& largestContour, const cv::Mat& edges = cv::Mat(), const bool& opening = 0)
{
	cv::Mat otsu;
	cv::threshold(src, otsu, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

	if (!edges.empty())
	{
		cv::Mat dilatedEdges;
		cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(1, 3));
		cv::dilate(edges, dilatedEdges, kernel);
		bitwiseNand(otsu, dilatedEdges);
	}

	if (opening)
	{
		int iterations = 1;

		cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(1, 3));
		cv::erode(otsu, otsu, kernel);

		cv::dilate(otsu, otsu, cv::Mat());
	}

	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(otsu, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	getLargestContour(contours, largestContour);

	dst = cv::Mat::zeros(otsu.size(), otsu.type());
	cv::drawContours(dst, std::vector<std::vector<cv::Point>>{largestContour}, 0, cv::Scalar(255), cv::FILLED);
}

void algorithm::compareWithCentroid(const std::vector<cv::Vec4i>& lines, std::vector<cv::Vec4i>& firstLines, std::vector<cv::Vec4i>& secondLines, const cv::Vec2i& centroid, const bool& direction)
{
	for (const auto& line : lines)
	{
		int median = (line[direction] + line[direction + 2]) / 2;
		if (median < centroid[direction])
			firstLines.push_back(line);
		else
			secondLines.push_back(line);
	}
}

cv::Vec4i algorithm::initialTerminalPoints(std::vector<cv::Vec4i>& lines, const bool& direction)
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

bool algorithm::lineSorting(std::vector<cv::Vec4i>& sortedLines, const std::vector<cv::Vec4i>& lines, const cv::Size& size, cv::Mat src)
{
	std::vector<cv::Vec4i> verticalLines;
	std::vector<cv::Vec4i> horizontalLines;
	for (const auto& line : lines)
	{
		if (line[2] == line[0])
		{
			verticalLines.push_back(line);
			continue;
		}

		float slope = abs(static_cast<float>(line[3]) - static_cast<float>(line[1])) / abs(static_cast<float>(line[2]) - static_cast<float>(line[0]));
		float angle = atan(slope) * 180 / CV_PI;
		float distanceOy = abs(90 - angle);
		float distanceOx = abs(0 - angle);

		if (distanceOy < distanceOx)
			verticalLines.push_back(line);
		else
			horizontalLines.push_back(line);
	}

	cv::Vec2i centroid = cv::Vec2f(size.width / 2, size.height / 2);

	std::vector<cv::Vec4i> topLines, bottomLines;
	compareWithCentroid(horizontalLines, topLines, bottomLines, centroid, 1);

	if (topLines.empty() || bottomLines.empty())
		return false;

	std::vector<cv::Vec4i> leftLines, rightLines;
	compareWithCentroid(verticalLines, leftLines, rightLines, centroid, 0);

	if (leftLines.empty() || rightLines.empty())
		return false;

	sortedLines.push_back(initialTerminalPoints(leftLines, 1));
	sortedLines.push_back(initialTerminalPoints(topLines, 0));
	sortedLines.push_back(initialTerminalPoints(rightLines, 1));
	sortedLines.push_back(initialTerminalPoints(bottomLines, 0));

	//-------------------------------------------------------------------------
	cv::Mat debug;
	cv::cvtColor(src, debug, cv::COLOR_GRAY2BGR);
	for (const cv::Vec4i& line : bottomLines) {
		cv::Point start(line[0], line[1]);
		cv::Point end(line[2], line[3]);
		cv::line(debug, start, end, cv::Scalar(0, 0, 255), 1);
	}

	for (const cv::Vec4i& line : topLines) {
		cv::Point start(line[0], line[1]);
		cv::Point end(line[2], line[3]);
		cv::line(debug, start, end, cv::Scalar(0, 255, 0), 1);
	}
	for (const cv::Vec4i& line : rightLines) {
		cv::Point start(line[0], line[1]);
		cv::Point end(line[2], line[3]);
		cv::line(debug, start, end, cv::Scalar(255, 0, 0), 1);
	}
	for (const cv::Vec4i& line : leftLines) {
		cv::Point start(line[0], line[1]);
		cv::Point end(line[2], line[3]);
		cv::line(debug, start, end, cv::Scalar(0, 255, 255), 1);
	}

	for (const auto& line : sortedLines)
	{
		cv::Point startPoint(line[0], line[1]);
		cv::Point endPoint(line[2], line[3]);
		cv::line(debug, startPoint, endPoint, cv::Scalar(255, 255, 0), 1);
	}
	//-------------------------------------------------------------------------
}

cv::Point2f algorithm::intersection(const cv::Vec4i& line1, const cv::Vec4i& line2)
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

bool algorithm::cornersCoordinates(const cv::Mat& src, std::vector<cv::Point2f>& quadrilateralCoordinates, const std::vector<cv::Point>& largestContour)
{
	cv::Mat erodedRegionContour;
	cv::erode(src, erodedRegionContour, cv::Mat());

	cv::Mat edges = src - erodedRegionContour;

	cv::RotatedRect rotatedBBox = cv::minAreaRect(largestContour);
	float minLineLenght = rotatedBBox.size.height * 0.4;
	float maxLineGap = rotatedBBox.size.height * 0.5;

	std::vector<cv::Vec4i> lines;
	std::vector<cv::Vec4i> sortedLines;
	do {
		cv::HoughLinesP(edges, lines, 1, CV_PI / 180, 10, minLineLenght, maxLineGap);
		minLineLenght--;
	} while (!lineSorting(sortedLines, lines, src.size(), edges) && minLineLenght > 0);

	if (minLineLenght <= 0)
		return false;

	for (int i = 0; i < sortedLines.size(); i++)
		quadrilateralCoordinates.push_back(intersection(sortedLines[i], sortedLines[(i + 1) % 4]));

	//-------------------------------------------------------------------------
	cv::Mat debug;
	cv::cvtColor(edges, debug, cv::COLOR_GRAY2BGR);

	for (const auto& point : quadrilateralCoordinates)
		cv::circle(debug, point, 2, cv::Scalar(0, 0, 255), -1);
	//-------------------------------------------------------------------------

	return true;
}

void algorithm::resizeToPoints(const cv::Mat& src, cv::Mat& dst, std::vector<cv::Point2f>& points)
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
		if (point.x < 0)
			point.x = paddingLeft + point.x;

		if (point.y < 0)
			point.y = paddingTop + point.y;
	}

	cv::copyMakeBorder(src, dst, paddingTop, paddingBottom, paddingLeft, paddingRight, cv::BORDER_CONSTANT);
}

bool algorithm::geometricalTransformation(const cv::Mat& src, cv::Mat& dst, const std::vector<cv::Point2f>& quadrilateralCoordinates)
{
	int height = quadrilateralCoordinates[3].y - quadrilateralCoordinates[0].y;
	int width = quadrilateralCoordinates[1].x - quadrilateralCoordinates[0].x;

	if (height < 0 || width < 0)
		return false;

	std::vector<cv::Point2f> finalCoordinates;
	finalCoordinates.push_back(cv::Point2f(0, 0));
	finalCoordinates.push_back(cv::Point2f(width - 1, 0));
	finalCoordinates.push_back(cv::Point2f(width - 1, height - 1));
	finalCoordinates.push_back(cv::Point2f(0, height - 1));

	cv::Mat perspectiveTransform = cv::getPerspectiveTransform(quadrilateralCoordinates, finalCoordinates);

	dst = cv::Mat(cv::Size(width, height), src.type());
	cv::warpPerspective(src, dst, perspectiveTransform, cv::Size(width, height));

	return true;
}

void algorithm::paddingImage(const cv::Mat& src, cv::Mat& dst, const float& percent = 0, const cv::Scalar& value = cv::Scalar())
{
	int paddingX, paddingY;

	if (!percent)
	{
		paddingX = 1;
		paddingY = 1;
	}
	else
	{
		paddingX = src.cols * percent;
		paddingY = src.rows * percent;
	}

	cv::copyMakeBorder(src, dst, paddingY, paddingY, paddingX, paddingX, cv::BORDER_CONSTANT, value);
}

bool algorithm::insideContour(const cv::Mat& src, cv::Mat& dst, const cv::Mat& regionContour)
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

int algorithm::getContourHeight(const std::vector<cv::Point>& contour)
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

int algorithm::medianHeight(const std::vector<std::vector<cv::Point>>& contours)
{
	std::vector<int> heights;
	for (const auto& contour : contours)
		heights.push_back(getContourHeight(contour));

	std::sort(heights.begin(), heights.end());
	return heights[heights.size() / 2];
}

bool algorithm::denoise(const cv::Mat& src, cv::Mat& dst, const float& percent = 1)
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

	return cv::countNonZero(dst);
}

bool compareX(const cv::Rect& a, const cv::Rect& b)
{
	return a.x < b.x;
}

void algorithm::charsSeparation(const cv::Mat& src, std::vector<cv::Rect>& chars)
{
	cv::Mat stats;
	std::vector<std::pair<int, int>> areas;
	algorithm::getConnectedComponents(src, stats, areas);

	for (const auto& area : areas)
	{
		cv::Rect roi;
		int label = area.first;
		algorithm::getRoi(stats, roi, label);

		paddingRect(roi, roi, cv::Size(src.cols, src.rows), 0.1);

		chars.push_back(roi);
	}

	std::sort(chars.begin(), chars.end(), compareX);
}

void algorithm::wordSeparation(const std::vector<cv::Rect>& chars, std::vector<cv::Rect>& left, std::vector<cv::Rect>& middle, std::vector<cv::Rect>& right)
{
	int maxDistance = 0;
	int firstIndex = 0;
	for (int i = 0; i < chars.size() - 1; i++)
	{
		int distance = chars[i + 1].x - chars[i].x;
		if (distance > maxDistance)
		{
			maxDistance = distance;
			firstIndex = i;
		}
	}

	int lastIndex = chars.size() - 3;
	for (int i = 0; i < chars.size(); i++)
	{
		if (i <= firstIndex)
			left.push_back(chars[i]);
		else
			if (i > firstIndex && i < lastIndex)
				middle.push_back(chars[i]);
			else
				if (i >= lastIndex)
					right.push_back(chars[i]);
	}
}

void algorithm::getWord(const std::vector<cv::Rect>& chars, cv::Rect& word)
{
	cv::Point topLeft(chars[0].x, chars[0].y);
	for (const auto& roi : chars)
	{
		if (roi.x < topLeft.x)
			topLeft.x = roi.x;
		if (roi.y < topLeft.y)
			topLeft.y = roi.y;
	}

	cv::Point bottomRight(chars[0].x + chars[0].width, chars[0].y + chars[0].height);
	for (const auto& roi : chars)
	{
		if (roi.x + roi.width > bottomRight.x)
			bottomRight.x = roi.x + roi.width;
		if (roi.y + roi.height > bottomRight.y)
			bottomRight.y = roi.y + roi.height;
	}

	word.x = topLeft.x;
	word.y = topLeft.y;
	word.width = bottomRight.x - topLeft.x;
	word.height = bottomRight.y - topLeft.y;
}

bool algorithm::print(tesseract::TessBaseAPI& tess, const bool& levelValue)
{
	std::string text = tess.GetUTF8Text();

	if (text.empty() || std::all_of(text.begin(), text.end(), isspace))
		return false;

	tess.Recognize(0);
	tesseract::ResultIterator* iterator = tess.GetIterator();
	tesseract::PageIteratorLevel level;
	if (levelValue)
		level = tesseract::RIL_BLOCK;
	else
		level = tesseract::RIL_SYMBOL;

	std::string symbol = iterator->GetUTF8Text(level);
	float confidence = iterator->Confidence(level);

	if (levelValue)
		symbol.erase(symbol.size() - 1, 1);
	std::cout << symbol << " " << confidence << std::endl;

	return true;
}

bool algorithm::applyTesseract(const cv::Mat& src, std::string& text, const std::vector<cv::Rect>& chars, const bool& charType)
{
	tesseract::TessBaseAPI tess;
	tess.Init(NULL, "DIN1451Mittelschrift", tesseract::OEM_DEFAULT);

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

	cv::Rect word;
	getWord(chars, word);

	//-------------------------------------------------------------------------
	cv::Mat debug;
	cv::cvtColor(src, debug, cv::COLOR_GRAY2BGR);
	for (const auto& rect : chars)
		cv::rectangle(debug, rect, cv::Scalar(0, 255, 0), 1);
	cv::rectangle(debug, word, cv::Scalar(0, 0, 255), 1);
	//-------------------------------------------------------------------------

	tess.SetRectangle(word.x, word.y, word.width, word.height);

	print(tess, 1);

	for (const auto& roi : chars)
	{
		tess.SetRectangle(roi.x, roi.y, roi.width, roi.height);


		print(tess, 0);

		text = text + tess.GetUTF8Text();
		text.erase(text.size() - 1, 1);
	}

	std::cout << std::endl << std::endl;

	return true;
}

bool algorithm::readText(const cv::Mat& src, std::string& text, const std::vector<cv::Rect>& chars)
{
	std::vector<cv::Rect> left;
	std::vector<cv::Rect> middle;
	std::vector<cv::Rect> right;

	wordSeparation(chars, left, middle, right);

	if (!applyTesseract(src, text, left, 0))
		return false;
	if (!applyTesseract(src, text, middle, 1))
		return false;
	if (!applyTesseract(src, text, right, 0))
		return false;

	return true;
}

void algorithm::drawBBoxes(cv::Mat& dst, std::vector<cv::Rect>& roiConnectedComponents)
{
	for (cv::Rect& roi : roiConnectedComponents)
	{
		roi.x = roi.x * 2;
		roi.y = roi.y * 2 + dst.rows / 2;
		roi.width = roi.width * 2;
		roi.height = roi.height * 2;

		cv::rectangle(dst, roi, cv::Scalar(0, 255, 0), 4);
	}
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

	cv::Rect roi(0, bgrSrc.rows / 2, bgrSrc.cols, bgrSrc.rows / 2);
	cv::Mat cropped = bgrSrc(roi);

	cv::Mat gauss;
	cv::GaussianBlur(cropped, gauss, cv::Size(3, 3), 0);

	cv::Mat hsv;
	algorithm::BGR2HSV(gauss, hsv);

	cv::Mat binary;
	algorithm::HSV2Binary(hsv, binary, 125);

	cv::Mat stats;
	std::vector<std::pair<int, int>> areas;
	algorithm::getConnectedComponents(binary, stats, areas, 10);

	std::string plates;
	std::vector<cv::Rect> roiConnectedComponents;
	for (int i = 0; i < areas.size(); i++)
	{
		cv::Rect roi;
		int label = areas[i].first;

		algorithm::getRoi(stats, roi, label);

		if (!algorithm::sizeBBox(cropped, roi, 0.01, 0.2) || !algorithm::heightBBox(roi, 0.2, 0.6))
			continue;

		algorithm::paddingRect(roi, roi, cropped.size(), 0.05);

		cv::Mat hsvConnectedComponent = hsv(roi);
		algorithm::blueToBlack(hsvConnectedComponent, hsvConnectedComponent);

		cv::Mat bgrConnectedComponent;
		algorithm::HSV2BGR(hsvConnectedComponent, bgrConnectedComponent);

		cv::Mat grayConnectedComponent;
		cv::cvtColor(bgrConnectedComponent, grayConnectedComponent, cv::COLOR_BGR2GRAY);

		cv::Mat edges;
		algorithm::edgeDetection(grayConnectedComponent, edges);

		cv::Mat regionContour;
		std::vector<cv::Point> largestContour;
		algorithm::roiContour(grayConnectedComponent, regionContour, largestContour, edges, 1);

		std::vector<cv::Point2f> quadrilateralCoordinates;
		if (!algorithm::cornersCoordinates(regionContour, quadrilateralCoordinates, largestContour))
			continue;

		cv::Mat resizedConnectedComponent;
		algorithm::resizeToPoints(grayConnectedComponent, resizedConnectedComponent, quadrilateralCoordinates);

		cv::Mat transformedConnectedComponent;
		if (!algorithm::geometricalTransformation(resizedConnectedComponent, transformedConnectedComponent, quadrilateralCoordinates))
			continue;

		algorithm::paddingImage(transformedConnectedComponent, transformedConnectedComponent, 0, cv::Scalar(255));

		cv::Mat first = regionContour.clone();
		algorithm::roiContour(transformedConnectedComponent, regionContour, largestContour);

		cv::Mat textConnectedComponent;
		if (!algorithm::insideContour(transformedConnectedComponent, textConnectedComponent, regionContour))
			continue;

		cv::Mat denoiseConnectedComponent;
		if (!algorithm::denoise(textConnectedComponent, denoiseConnectedComponent, 0.1))
			continue;

		algorithm::paddingImage(denoiseConnectedComponent, denoiseConnectedComponent, 0.1);

		std::vector<cv::Rect> chars;
		algorithm::charsSeparation(denoiseConnectedComponent, chars);

		std::string plate;
		if (!algorithm::readText(denoiseConnectedComponent, plate, chars))
			continue;

		roiConnectedComponents.push_back(roi);
		plates = plates + plate + " ";
	}

	algorithm::drawBBoxes(bgrDst, roiConnectedComponents);

	cv::cvtColor(bgrDst, dst, cv::COLOR_BGR2RGB);

	return std::string();
}
