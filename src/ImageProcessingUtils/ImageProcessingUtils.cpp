#include "ImageProcessingUtils.h"

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

void algorithm::getKernel(const cv::Mat& src, cv::Size& kernel, const float& percent = 0)
{
	kernel = cv::Size(src.rows * percent, src.cols * percent);

	if (kernel.height < 3)
		kernel.height = 3;
	if (kernel.width < 3)
		kernel.width = 3;

	if (kernel.height % 2 == 0)
		kernel.height--;
	if (kernel.width % 2 == 0)
		kernel.width--;
}

void algorithm::BGR2HSV(const cv::Mat& src, cv::Mat& dst)
{
	dst = cv::Mat(src.size(), CV_8UC3);

	for (int y = 0; y < src.rows; y++)
		for (int x = 0; x < src.cols; x++)
		{
			double b = src.ptr<uchar>(y, x)[0] / 255.0;
			double g = src.ptr<uchar>(y, x)[1] / 255.0;
			double r = src.ptr<uchar>(y, x)[2] / 255.0;

			double cmax = std::max(r, std::max(g, b));
			double cmin = std::min(r, std::min(g, b));
			double diff = cmax - cmin;
			double h, s;

			if (cmax == cmin)
				h = 0;
			else if (cmax == r)
				h = fmod(60 * ((g - b) / diff) + 360, 360) / 2;
			else if (cmax == g)
				h = fmod(60 * ((b - r) / diff) + 120, 360) / 2;
			else if (cmax == b)
				h = fmod(60 * ((r - g) / diff) + 240, 360) / 2;

			if (cmax == 0)
				s = 0;
			else
				s = (diff / cmax) * 255;

			double v = cmax * 255;

			dst.ptr<uchar>(y, x)[0] = static_cast<uchar>(h);
			dst.ptr<uchar>(y, x)[1] = static_cast<uchar>(s);
			dst.ptr<uchar>(y, x)[2] = static_cast<uchar>(v);
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

void algorithm::getRoi(const cv::Mat& stats, const int& label, cv::Rect& roi)
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

int algorithm::insideContour(const cv::Mat& src, cv::Mat& dst, cv::Mat& regionContour, std::vector<cv::Point>& largestContour)
{
	cv::Mat otsu;
	cv::threshold(src, otsu, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(otsu, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	getLargestContour(contours, largestContour);

	otsu = 255 - otsu;

	regionContour = cv::Mat::zeros(otsu.size(), otsu.type());
	cv::drawContours(regionContour, std::vector<std::vector<cv::Point>>{largestContour}, 0, cv::Scalar(255), cv::FILLED);
	cv::morphologyEx(regionContour, regionContour, cv::MORPH_OPEN, cv::Mat());

	dst = cv::Mat::zeros(otsu.size(), otsu.type());
	otsu.copyTo(dst, regionContour);

	cv::copyMakeBorder(regionContour, regionContour, 2, 2, 2, 2, cv::BORDER_CONSTANT);

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

bool algorithm::denoise(const cv::Mat& src, cv::Mat& dst, const float& percent = 0)
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

	if (contours.size() < 4)
		return false;

	cv::Mat contoursRegion = cv::Mat::zeros(src.size(), src.type());
	for (const auto& contour : contours)
		cv::drawContours(contoursRegion, std::vector<std::vector<cv::Point>>{contour}, 0, cv::Scalar(255), cv::FILLED);

	src.copyTo(dst, contoursRegion);

	return cv::countNonZero(dst);
}

void algorithm::compareWithCentroid(const std::vector<cv::Vec4i>& lines, std::vector<cv::Vec4i>& firstLines, std::vector<cv::Vec4i>& secondLines, const cv::Vec2i& centroid, const bool& direction)
{
	for (const auto& line : lines)
		if (line[direction] < centroid[direction])
			firstLines.push_back(line);
		else
			secondLines.push_back(line);
}

cv::Vec4i algorithm::initialTerminalPoints(const std::vector<cv::Vec4i>& lines, const bool& direction)
{
	cv::Vec4i segment = lines[0];
	for (const auto& line : lines)
	{
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

void algorithm::lineSorting(const cv::Mat& src, const std::vector<cv::Vec4i>& lines, std::vector<cv::Vec4i>& sortedLines)
{
	std::vector<cv::Vec4i> verticalLines;
	std::vector<cv::Vec4i> horizontalLines;
	for (const auto& line : lines)
	{
		float slope = abs(line[3] - line[1]) / abs(line[2] - line[0]);
		float angle = atan(slope) * 180 / CV_PI;
		float distanceOy = abs(90 - angle);
		float distanceOx = abs(0 - angle);

		if (distanceOy < distanceOx)
			verticalLines.push_back(line);
		else
			horizontalLines.push_back(line);
	}

	cv::Vec2i centroid = cv::Vec2f(src.cols / 2, src.rows / 2);

	std::vector<cv::Vec4i> topLines, bottomLines;
	compareWithCentroid(horizontalLines, topLines, bottomLines, centroid, 1);

	std::vector<cv::Vec4i> leftLines, rightLines;
	compareWithCentroid(verticalLines, leftLines, rightLines, centroid, 0);

	sortedLines.push_back(initialTerminalPoints(leftLines, 1));
	sortedLines.push_back(initialTerminalPoints(topLines, 0));
	sortedLines.push_back(initialTerminalPoints(rightLines, 1));
	sortedLines.push_back(initialTerminalPoints(bottomLines, 0));
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

void algorithm::geometricTransformation(const cv::Mat& src, cv::Mat& dst, const cv::Mat& regionContour, const std::vector<cv::Point>& largestContour)
{
	cv::Mat erodedRegionContour;
	cv::erode(regionContour, erodedRegionContour, cv::Mat());

	cv::Mat edges = regionContour - erodedRegionContour;

	cv::RotatedRect rotatedBBox = cv::minAreaRect(largestContour);
	float minLineLenght = rotatedBBox.size.height * 0.8;

	std::vector<cv::Vec4i> lines;
	cv::HoughLinesP(edges, lines, 1, CV_PI / 180, 20, minLineLenght, 70);

	std::vector<cv::Vec4i> sortedLines;
	lineSorting(src, lines, sortedLines);

	std::vector<cv::Point2f> cornersCoordinates;
	for (int i = 0; i < sortedLines.size(); i++)
		cornersCoordinates.push_back(intersection(sortedLines[i], sortedLines[(i + 1) % 4]));

	int height = cornersCoordinates[3].y - cornersCoordinates[1].y;
	int width = cornersCoordinates[1].x - cornersCoordinates[0].x;
	dst = cv::Mat::zeros(cv::Size(width, height), src.type());

	std::vector<cv::Point2f> finalCoordinates;
	finalCoordinates.push_back(cv::Point2f(0, 0));
	finalCoordinates.push_back(cv::Point2f(width - 1, 0));
	finalCoordinates.push_back(cv::Point2f(width - 1, height - 1));
	finalCoordinates.push_back(cv::Point2f(0, height - 1));

	cv::Mat perspectiveTransform = cv::getPerspectiveTransform(cornersCoordinates, finalCoordinates);

	cv::Mat transformed = cv::Mat::zeros(cv::Size(width, height), src.type());;
	cv::warpPerspective(src, transformed, perspectiveTransform, cv::Size(width, height));

	cv::threshold(transformed, dst, 0, 255, cv::THRESH_BINARY);

	cv::Mat debug;
	cv::cvtColor(edges, debug, cv::COLOR_GRAY2BGR);
	for (const auto& point : cornersCoordinates)
		cv::circle(debug, point, 1, cv::Scalar(0, 0, 255), -1);
}

bool algorithm::readText(const cv::Mat& src, std::string& text)
{
	tesseract::TessBaseAPI tess;
	tess.Init(NULL, "DIN1451Mittelschrift", tesseract::OEM_DEFAULT);
	tess.SetVariable("tessedit_char_whitelist", "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");

	tess.SetImage(src.data, src.cols, src.rows, 1, src.step);
	text = tess.GetUTF8Text();

	text.erase(std::remove_if(text.begin(), text.end(), [](uchar x) { return std::isspace(x); }), text.end());

	if (text.empty() || std::all_of(text.begin(), text.end(), isspace))
		return false;

	tess.Recognize(0);
	tesseract::ResultIterator* iterator = tess.GetIterator();
	tesseract::PageIteratorLevel level = tesseract::RIL_BLOCK;
	if (iterator != nullptr)
	{
		do
		{
			std::string word = iterator->GetUTF8Text(level);
			float confidence = iterator->Confidence(level);
			std::cout << word << confidence << std::endl;
		} while (iterator->Next(level));
	}

	iterator = tess.GetIterator();
	level = tesseract::RIL_SYMBOL;
	if (iterator != nullptr)
	{
		do
		{
			std::string symbol = iterator->GetUTF8Text(level);
			float confidence = iterator->Confidence(level);
			std::cout << symbol << " " << confidence << std::endl;
		} while (iterator->Next(level));
	}

	std::cout << std::endl << std::endl;

	return true;
}

void algorithm::drawBBoxes(cv::Mat& dst, std::vector<cv::Rect> roiConnectedComponents)
{
	for (cv::Rect& roi : roiConnectedComponents)
	{
		roi.y = roi.y + dst.rows / 2;
		cv::rectangle(dst, roi, cv::Scalar(0, 255, 0), 2);
	}
}

std::string textFromImage(const cv::Mat& src, cv::Mat& dst)
{
	cv::Mat srcBGR;
	if (src.type() == CV_8UC4)
		cv::cvtColor(src, srcBGR, cv::COLOR_BGRA2BGR);
	else
		srcBGR = src.clone();

	cv::resize(srcBGR, srcBGR, cv::Size(srcBGR.cols / 2, srcBGR.rows / 2));

	cv::Mat dstBGR = srcBGR.clone();

	cv::Rect roi(0, srcBGR.rows / 2, srcBGR.cols, srcBGR.rows / 2);
	cv::Mat cropped = srcBGR(roi);

	cv::Size kernel;
	algorithm::getKernel(cropped, kernel, 0.01);

	cv::Mat gauss;
	cv::GaussianBlur(cropped, gauss, kernel, 0);

	cv::Mat gray;
	cv::cvtColor(gauss, gray, cv::COLOR_BGR2GRAY);

	cv::Mat hsv;
	algorithm::BGR2HSV(gauss, hsv);

	cv::Mat binary;
	algorithm::HSV2Binary(hsv, binary, 125);

	cv::Mat stats;
	std::vector<std::pair<int, int>> areas;
	algorithm::getConnectedComponents(binary, stats, areas, 10);

	std::vector<cv::Rect> roiConnectedComponents;
	std::string plates;
	for (int i = 0; i < areas.size(); i++)
	{
		cv::Rect roi;
		int label = areas[i].first;

		algorithm::getRoi(stats, label, roi);

		if (!algorithm::sizeBBox(cropped, roi, 0.01, 0.2) || !algorithm::heightBBox(roi, 0.2, 0.6))
			continue;

		cv::Mat grayConnectedComponent = gray(roi);
		cv::Mat textConnectedComponent;
		cv::Mat regionContour;
		std::vector<cv::Point> largestContour;
		if (!algorithm::insideContour(grayConnectedComponent, textConnectedComponent, regionContour, largestContour))
			continue;

		cv::Mat denoiseConnectedComponent;
		if (!algorithm::denoise(textConnectedComponent, denoiseConnectedComponent, 0.2))
			continue;

		cv::Mat transformedConnectedComponent;
		algorithm::geometricTransformation(denoiseConnectedComponent, transformedConnectedComponent, regionContour, largestContour);

		std::string plate;
		if (!algorithm::readText(transformedConnectedComponent, plate))
			continue;

		roiConnectedComponents.push_back(roi);
		plates = plates + plate + " ";
	}

	algorithm::drawBBoxes(dstBGR, roiConnectedComponents);

	cv::cvtColor(dstBGR, dst, cv::COLOR_BGR2RGB);

	return std::string();
}
