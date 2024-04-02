#include "CppUnitTest.h"
#include "../src/ImageProcessingUtils/ImageProcessingUtils.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(AlgorithmTests)
{
public:
	TEST_METHOD(BGR2HSV_InvalidInput)
	{
		cv::Mat src, dst;
		Algorithm::BGR2HSV(src, dst);
	}

	TEST_METHOD(BGR2HSV_InvalidChannelsNumber)
	{
		cv::Mat src, dst;

		src = cv::Mat::zeros(100, 100, CV_8UC1);
		Algorithm::BGR2HSV(src, dst);

		src = cv::Mat::zeros(100, 100, CV_8UC4);
		Algorithm::BGR2HSV(src, dst);
	}

	TEST_METHOD(BGR2HSV_InvalidType)
	{
		cv::Mat src, dst;

		src = cv::Mat::zeros(100, 100, CV_16FC3);
		Algorithm::BGR2HSV(src, dst);
	}

	TEST_METHOD(HSV2Binary_InvalidInput)
	{
		cv::Mat src, dst;
		uchar threshold;
		Algorithm::HSV2Binary(src, dst, threshold);
	}

	TEST_METHOD(HSV2Binary_InvalidChannelsNumber)
	{
		cv::Mat src, dst;

		src = cv::Mat::zeros(100, 100, CV_8UC1);
		Algorithm::HSV2Binary(src, dst);

		src = cv::Mat::zeros(100, 100, CV_8UC4);
		Algorithm::HSV2Binary(src, dst);
	}

	TEST_METHOD(HSV2Binary_InvalidType)
	{
		cv::Mat src, dst;

		src = cv::Mat::zeros(100, 100, CV_16FC3);
		Algorithm::HSV2Binary(src, dst);
	}

	TEST_METHOD(getConnectedComponents_InvalidInput)
	{
		cv::Mat src, stats;
		std::vector<std::pair<int, int>> areas;
		int newSize;
		Algorithm::getConnectedComponents(src, stats, areas, newSize);

		src = cv::Mat::zeros(100, 100, CV_8UC1);
		newSize = -10;
		Algorithm::getConnectedComponents(src, stats, areas, newSize);

		src.ptr<uchar>(0, 0)[0] = 255;
		src.ptr<uchar>(0, 1)[0] = 255;
		Algorithm::getConnectedComponents(src, stats, areas);

		Assert::AreEqual(1, (int)areas.size());
	}

	TEST_METHOD(getConnectedComponents_InvalidChannelsNumber)
	{
		cv::Mat src, stats;
		std::vector<std::pair<int, int>> areas;

		src = cv::Mat::zeros(100, 100, CV_8UC3);
		Algorithm::getConnectedComponents(src, stats, areas);
	}

	TEST_METHOD(getConnectedComponents_InvalidType)
	{
		cv::Mat src, stats;
		std::vector<std::pair<int, int>> areas;

		src = cv::Mat::zeros(100, 100, CV_16FC1);
		Algorithm::getConnectedComponents(src, stats, areas);
	}

	TEST_METHOD(getRoi_InvalidInput)
	{
		cv::Mat stats;
		cv::Rect roi;
		int label;
		Algorithm::getRoi(stats, roi, label);

		stats = cv::Mat::zeros(100, 100, CV_8UC1);
		label = -1;
		Algorithm::getRoi(stats, roi, label);

		label = 1;
		Algorithm::getRoi(stats, roi, label);
	}

	TEST_METHOD(getRoi_InvalidChannelsNumber)
	{
		cv::Mat stats;
		cv::Rect roi;
		int label = 1;

		stats = cv::Mat::zeros(100, 100, CV_8UC3);
		Algorithm::getRoi(stats, roi, label);
	}

	TEST_METHOD(getRoi_InvalidType)
	{
		cv::Mat stats;
		cv::Rect roi;
		int label = 1;

		stats = cv::Mat::zeros(100, 100, CV_16FC1);
		Algorithm::getRoi(stats, roi, label);
	}

	TEST_METHOD(sizeBBox_InvalidInput)
	{
		cv::Mat src;
		cv::Rect roi;
		float min, max;
		Algorithm::sizeBBox(src, roi, min, max);

		min = 0.5;
		max = 0.4;
		Algorithm::sizeBBox(src, roi, min, max);

		min = -0.5;
		max = -0.4;
		Algorithm::sizeBBox(src, roi, min, max);

		min = 5;
		max = 4;
		Algorithm::sizeBBox(src, roi, min, max);
	}

	TEST_METHOD(heightBBox_InvalidInput)
	{
		cv::Rect roi;
		float min, max;
		Algorithm::heightBBox(roi, min, max);

		min = 0.5;
		max = 0.4;
		Algorithm::heightBBox(roi, min, max);

		min = -0.5;
		max = -0.4;
		Algorithm::heightBBox(roi, min, max);

		min = 5;
		max = 4;
		Algorithm::heightBBox(roi, min, max);
	}

	TEST_METHOD(paddingRect_InvalidInput)
	{
		cv::Rect src, dst;
		float percent;
		bool square;
		cv::Size size;
		Algorithm::paddingRect(src, dst, percent, square, size);

		percent = -1;
		Algorithm::paddingRect(src, dst, percent);

		square = true;
		Algorithm::paddingRect(src, dst, percent, square);

		size = cv::Size(-1, -1);
		Algorithm::paddingRect(src, dst, percent, square, size);

		size = cv::Size(100, 100);
		Algorithm::paddingRect(src, dst, percent, square, size);
	}

	TEST_METHOD(blueToBlack_InvalidInput)
	{
		cv::Mat src, dst;
		Algorithm::blueToBlack(src, dst);
	}

	TEST_METHOD(blueToBlack_InvalidChannelsNumber)
	{
		cv::Mat src, dst;

		src = cv::Mat::zeros(100, 100, CV_8UC1);
		Algorithm::blueToBlack(src, dst);

		src = cv::Mat::zeros(100, 100, CV_8UC4);
		Algorithm::blueToBlack(src, dst);
	}

	TEST_METHOD(blueToBlack_InvalidType)
	{
		cv::Mat src, dst;

		src = cv::Mat::zeros(100, 100, CV_16FC3);
		Algorithm::blueToBlack(src, dst);
	}

	TEST_METHOD(HSV2BGR_InvalidInput)
	{
		cv::Mat src, dst;
		Algorithm::HSV2BGR(src, dst);
	}

	TEST_METHOD(HSV2BGR_InvalidChannelsNumber)
	{
		cv::Mat src, dst;

		src = cv::Mat::zeros(100, 100, CV_8UC1);
		Algorithm::HSV2BGR(src, dst);

		src = cv::Mat::zeros(100, 100, CV_8UC4);
		Algorithm::HSV2BGR(src, dst);
	}

	TEST_METHOD(HSV2BGR_InvalidType)
	{
		cv::Mat src, dst;

		src = cv::Mat::zeros(100, 100, CV_16FC1);
		Algorithm::HSV2BGR(src, dst);
	}

	TEST_METHOD(histogram_InvalidInput)
	{
		cv::Mat src, dst;
		Algorithm::histogram(src, dst);
	}

	TEST_METHOD(histogram_InvalidChannelsNumber)
	{
		cv::Mat src, dst;

		src = cv::Mat::zeros(100, 100, CV_16FC3);
		Algorithm::histogram(src, dst);
	}

	TEST_METHOD(histogram_InvalidType)
	{
		cv::Mat src, dst;

		src = cv::Mat::zeros(100, 100, CV_16SC1);
		Algorithm::histogram(src, dst);
	}

	TEST_METHOD(cumulativeHistogram_InvalidInput)
	{
		cv::Mat src, dst;
		Algorithm::cumulativeHistogram(src, dst);
	}

	TEST_METHOD(cumulativeHistogram_InvalidChannelsNumber)
	{
		cv::Mat src, dst;

		src = cv::Mat::zeros(100, 100, CV_16FC3);
		Algorithm::cumulativeHistogram(src, dst);
	}

	TEST_METHOD(cumulativeHistogram_InvalidType)
	{
		cv::Mat src, dst;

		src = cv::Mat::zeros(100, 100, CV_16SC1);
		Algorithm::cumulativeHistogram(src, dst);
	}

	TEST_METHOD(histogramLine_InvalidInput)
	{
		cv::Mat src;
		cv::Vec4f dst;
		Algorithm::histogramLine(src, dst);
	}

	TEST_METHOD(histogramLine_InvalidChannelsNumber)
	{
		cv::Mat src;
		cv::Vec4f dst;

		src = cv::Mat::zeros(100, 100, CV_8UC3);
		Algorithm::histogramLine(src, dst);
	}

	TEST_METHOD(histogramLine_InvalidType)
	{
		cv::Mat src;
		cv::Vec4f dst;

		src = cv::Mat::zeros(100, 100, CV_16SC1);
		Algorithm::histogramLine(src, dst);
	}

	TEST_METHOD(distance_InvalidInput)
	{
		float x, y;
		cv::Vec4f line;
		Algorithm::distance(x, y, line);
	}

	TEST_METHOD(distance_InvalidLine)
	{
		float x = 0, y = 0;
		cv::Vec4f line = cv::Vec4f(0, 0, 0, 0);
		Algorithm::distance(x, y, line);
	}

	TEST_METHOD(triangleThresholding_InvalidInput)
	{
		cv::Mat src, dst;
		Algorithm::triangleThresholding(src, dst);
	}

	TEST_METHOD(triangleThresholding_InvalidChannelsNumber)
	{
		cv::Mat src, dst;

		src = cv::Mat::zeros(100, 100, CV_8UC3);
		Algorithm::triangleThresholding(src, dst);
	}

	TEST_METHOD(triangleThresholding_InvalidType)
	{
		cv::Mat src, dst;

		src = cv::Mat::zeros(100, 100, CV_16FC1);
		Algorithm::triangleThresholding(src, dst);
	}

	TEST_METHOD(binarySobel_InvalidInput)
	{
		cv::Mat src, dst, direction;
		Algorithm::binarySobel(src, dst, direction);
	}

	TEST_METHOD(binarySobel_InvalidChannelsNumber)
	{
		cv::Mat src, dst, direction;

		src = cv::Mat::zeros(100, 100, CV_8UC1);
		Algorithm::binarySobel(src, dst, direction);


		src = cv::Mat::zeros(100, 100, CV_8UC4);
		Algorithm::binarySobel(src, dst, direction);
	}

	TEST_METHOD(binarySobel_InvalidType)
	{
		cv::Mat src, dst, direction;

		src = cv::Mat::zeros(100, 100, CV_16FC3);
		Algorithm::binarySobel(src, dst, direction);
	}

	TEST_METHOD(nonMaximumSuppression_InvalidInput)
	{
		cv::Mat src, dst, direction;
		Algorithm::nonMaximumSuppression(src, dst, direction);

		src = cv::Mat::zeros(100, 200, CV_8UC1);
		direction = cv::Mat::zeros(200, 100, CV_8UC1);
		Algorithm::nonMaximumSuppression(src, dst, direction);
	}

	TEST_METHOD(nonMaximumSuppression_InvalidChannelsNumber)
	{
		cv::Mat src, dst, direction;

		src = cv::Mat::zeros(100, 100, CV_8UC3);
		direction = cv::Mat::zeros(100, 100, CV_8UC3);
		Algorithm::nonMaximumSuppression(src, dst, direction);
	}

	TEST_METHOD(nonMaximumSuppression_InvalidType)
	{
		cv::Mat src, dst, direction;

		src = cv::Mat::zeros(100, 100, CV_16FC1);
		direction = cv::Mat::zeros(100, 100, CV_8UC1);
		Algorithm::nonMaximumSuppression(src, dst, direction);
	}

	TEST_METHOD(edgeDetection_InvalidInput)
	{
		cv::Mat src, dst;
		Algorithm::edgeDetection(src, dst);
	}

	TEST_METHOD(edgeDetection_InvalidChannelsNumber)
	{
		cv::Mat src, dst;

		src = cv::Mat::zeros(100, 100, CV_8UC1);
		Algorithm::edgeDetection(src, dst);


		src = cv::Mat::zeros(100, 100, CV_8UC4);
		Algorithm::edgeDetection(src, dst);
	}

	TEST_METHOD(edgeDetection_InvalidType)
	{
		cv::Mat src, dst;

		src = cv::Mat::zeros(100, 100, CV_16FC3);
		Algorithm::edgeDetection(src, dst);
	}

	TEST_METHOD(bitwiseNand_InvalidInput)
	{
		cv::Mat src, edges;
		Algorithm::bitwiseNand(src, edges);

		src = cv::Mat::zeros(200, 100, CV_8UC1);
		edges = cv::Mat::zeros(100, 200, CV_8UC1);
		Algorithm::bitwiseNand(src, edges);
	}

	TEST_METHOD(bitwiseNand_InvalidChannelsNumber)
	{
		cv::Mat src, edges;

		src = cv::Mat::zeros(100, 100, CV_8UC3);
		edges = cv::Mat::zeros(100, 100, CV_8UC3);
		Algorithm::bitwiseNand(src, edges);
	}

	TEST_METHOD(bitwiseNand_InvalidType)
	{
		cv::Mat src, edges;

		src = cv::Mat::zeros(100, 100, CV_16FC1);
		edges = cv::Mat::zeros(100, 100, CV_16FC1);
		Algorithm::bitwiseNand(src, edges);
	}

	TEST_METHOD(getLargestContour_InvalidInput)
	{
		std::vector<std::vector<cv::Point>> contours;
		std::vector<cv::Point> largestContour;
		Algorithm::getLargestContour(contours, largestContour);
	}

	TEST_METHOD(roiContour_InvalidType)
	{
		cv::Mat src, dst, edges;
		std::vector<cv::Point> largestContour;
		float percent;

		Algorithm::roiContour(src, dst, largestContour, edges, percent);

		src = cv::Mat::zeros(100, 200, CV_8UC1);
		edges = cv::Mat::zeros(200, 100, CV_8UC1);
		percent = -1;
		Algorithm::roiContour(src, dst, largestContour, edges, percent);
	}

	TEST_METHOD(roiContour_InvalidChannelsNumber)
	{
		cv::Mat src, dst, edges;
		std::vector<cv::Point> largestContour;

		src = cv::Mat::zeros(100, 100, CV_8UC3);
		edges = cv::Mat::zeros(100, 100, CV_8UC3);
		Algorithm::roiContour(src, dst, largestContour, edges);
	}

	TEST_METHOD(roiContour_InvalidInput)
	{
		cv::Mat src, dst, edges;
		std::vector<cv::Point> largestContour;

		src = cv::Mat::zeros(100, 100, CV_16FC1);
		edges = cv::Mat::zeros(100, 100, CV_16FC1);
		Algorithm::roiContour(src, dst, largestContour, edges);
	}

	TEST_METHOD(lineThroughPoint_InvalidInput)
	{
		cv::Vec4f line;
		double slope;
		cv::Point point;
		bool direction;

		Algorithm::lineThroughPoint(line, slope, point, direction);

		slope = 0;
		point = cv::Point(-1, -1);
		direction = -1;

		Algorithm::lineThroughPoint(line, slope, point, direction);

		direction = true;
		Algorithm::lineThroughPoint(line, slope, point, true);

		direction = false;
		Algorithm::lineThroughPoint(line, slope, point, false);
	}

	TEST_METHOD(compareWithLine_InvalidInput)
	{
		std::vector<cv::Vec4i> lines;
		std::vector<cv::Vec4i> firstLines;
		std::vector<cv::Vec4i> secondLines;
		cv::Vec4f referenceLine;
		Algorithm::compareWithLine(lines, firstLines, secondLines, referenceLine);

		lines.push_back(cv::Vec4i(0, 0, 0, 0));
		Algorithm::compareWithLine(lines, firstLines, secondLines, referenceLine);

		referenceLine = cv::Vec4f(0, 0, 0, 0);
		Algorithm::compareWithLine(lines, firstLines, secondLines, referenceLine);

		referenceLine = cv::Vec4f(-1, -1, -1, -1);
		Algorithm::compareWithLine(lines, firstLines, secondLines, referenceLine);
	}

	TEST_METHOD(initialTerminalPoints)
	{
		std::vector<cv::Vec4i> lines;
		bool direction;
		Algorithm::initialTerminalPoints(lines, direction);

		lines.push_back(cv::Vec4i(0, 0, 0, 0));
		Algorithm::initialTerminalPoints(lines, direction);
	}

	TEST_METHOD(lineSorting_InvalidInput)
	{
		std::vector<cv::Vec4i> sortedLines;
		std::vector<cv::Vec4i> lines;
		cv::Size size;
		Algorithm::lineSorting(sortedLines, lines, size);

		lines.push_back(cv::Vec4i(0, 10, 0, 0));
		Algorithm::lineSorting(sortedLines, lines, size);

		lines.clear();
		lines.push_back(cv::Vec4i(10, 0, 0, 0));
		Algorithm::lineSorting(sortedLines, lines, size);

		lines.push_back(cv::Vec4i(0, 10, 0, 0));
		Algorithm::lineSorting(sortedLines, lines, size);

		size = cv::Size(-1, -1);
		Algorithm::lineSorting(sortedLines, lines, size);
	}

	TEST_METHOD(intersection_InvalidInput)
	{
		cv::Vec4f line1, line2;
		Algorithm::intersection(line1, line2);

		line2 = cv::Vec4f(0, 0, 0, 10);
		Algorithm::intersection(line1, line2);

		line2 = cv::Vec4f(10, 0, 10, 10);
		Algorithm::intersection(line1, line2);

		line2 = line1;
		Algorithm::intersection(line1, line2);
	}

	TEST_METHOD(cornersCoordinates_InvalidInput)
	{
		cv::Mat src;
		std::vector<cv::Point2f> quadrilateralCoordinates;
		std::vector<cv::Point> largestContour;
		Algorithm::cornersCoordinates(src, quadrilateralCoordinates, largestContour);

		src = cv::Mat::zeros(100, 100, CV_8UC1);
		Algorithm::cornersCoordinates(src, quadrilateralCoordinates, largestContour);

		largestContour = std::vector<cv::Point>(4, cv::Point(0, 0));
		Algorithm::cornersCoordinates(src, quadrilateralCoordinates, largestContour);
	}

	TEST_METHOD(cornersCoordinates_InvalidChannelsNumber)
	{
		cv::Mat src;
		std::vector<cv::Point2f> quadrilateralCoordinates;
		std::vector<cv::Point> largestContour(4, cv::Point(0, 0));

		src = cv::Mat::zeros(100, 100, CV_8UC3);
		Algorithm::cornersCoordinates(src, quadrilateralCoordinates, largestContour);
	}

	TEST_METHOD(cornersCoordinates_InvalidType)
	{
		cv::Mat src;
		std::vector<cv::Point2f> quadrilateralCoordinates;
		std::vector<cv::Point> largestContour(4, cv::Point(0, 0));

		src = cv::Mat::zeros(100, 100, CV_16FC1);
		Algorithm::cornersCoordinates(src, quadrilateralCoordinates, largestContour);
	}

	TEST_METHOD(resizeToPoints_InvalidInput)
	{
		cv::Mat src, dst;
		std::vector<cv::Point2f> points;
		float percent;
		Algorithm::resizeToPoints(src, dst, points, percent);

		src = cv::Mat::zeros(100, 100, CV_8UC1);
		percent = -1;
		Algorithm::resizeToPoints(src, dst, points, percent);

		points = std::vector<cv::Point2f>(4, cv::Point2f(0, 0));
		Algorithm::resizeToPoints(src, dst, points, percent);
	}

	TEST_METHOD(resizeToPoints_InvalidChannelsNumber)
	{
		cv::Mat src;
		cv::Mat dst;
		std::vector<cv::Point2f> points(4, cv::Point2f(0, 0));

		src = cv::Mat::zeros(100, 100, CV_8UC3);
		Algorithm::resizeToPoints(src, dst, points);
	}

	TEST_METHOD(resizeToPoints_InvalidType)
	{
		cv::Mat src;
		cv::Mat dst;
		std::vector<cv::Point2f> points(4, cv::Point2f(0, 0));

		src = cv::Mat::zeros(100, 100, CV_16FC1);
		Algorithm::resizeToPoints(src, dst, points);
	}

	TEST_METHOD(geometricalTransformation_InvalidInput)
	{
		cv::Mat src, dst;
		std::vector<cv::Point2f> quadrilateralCoordinates;
		float percent;
		Algorithm::geometricalTransformation(src, dst, quadrilateralCoordinates, percent);

		src = cv::Mat::zeros(100, 100, CV_8UC1);
		quadrilateralCoordinates = std::vector<cv::Point2f>(4, cv::Point2f(0, 0));
		percent = -1;
		Algorithm::geometricalTransformation(src, dst, quadrilateralCoordinates, percent);
	}

	TEST_METHOD(geometricalTransformation_InvalidChannelsNumber)
	{
		cv::Mat src;
		cv::Mat dst;
		std::vector<cv::Point2f> quadrilateralCoordinates(4, cv::Point2f(0, 0));

		src = cv::Mat::zeros(100, 100, CV_8UC3);
		Algorithm::geometricalTransformation(src, dst, quadrilateralCoordinates);
	}

	TEST_METHOD(geometricalTransformation_InvalidType)
	{
		cv::Mat src;
		cv::Mat dst;
		std::vector<cv::Point2f> quadrilateralCoordinates(4, cv::Point2f(0, 0));

		src = cv::Mat::zeros(100, 100, CV_16FC1);
		Algorithm::geometricalTransformation(src, dst, quadrilateralCoordinates);
	}

	TEST_METHOD(insideContour_InvalidInput)
	{
		cv::Mat src, dst, regionContour;
		Algorithm::insideContour(src, dst, regionContour);

		src = cv::Mat::zeros(100, 100, CV_8UC1);
		Algorithm::insideContour(src, dst, regionContour);

		src = cv::Mat::zeros(100, 200, CV_8UC1);
		regionContour = cv::Mat::zeros(200, 100, CV_8UC1);
		Algorithm::insideContour(src, dst, regionContour);
	}

	TEST_METHOD(insideContour_InvalidChannelsNumber)
	{
		cv::Mat src, dst, regionContour;

		src = cv::Mat::zeros(100, 100, CV_8UC3);
		regionContour = cv::Mat::zeros(100, 100, CV_8UC3);
		Algorithm::insideContour(src, dst, regionContour);

		src = cv::Mat::zeros(100, 100, CV_8UC1);
		regionContour = cv::Mat::zeros(100, 100, CV_8UC3);
		Algorithm::insideContour(src, dst, regionContour);

		src = cv::Mat::zeros(100, 100, CV_8UC3);
		regionContour = cv::Mat::zeros(100, 100, CV_8UC1);
		Algorithm::insideContour(src, dst, regionContour);
	}

	TEST_METHOD(insideContour_InvalidType)
	{
		cv::Mat src, dst, regionContour;

		src = cv::Mat::zeros(100, 100, CV_8UC1);
		regionContour = cv::Mat::zeros(100, 100, CV_16FC1);
		Algorithm::insideContour(src, dst, regionContour);

		src = cv::Mat::zeros(100, 100, CV_16FC1);
		regionContour = cv::Mat::zeros(100, 100, CV_8UC1);
		Algorithm::insideContour(src, dst, regionContour);

		src = cv::Mat::zeros(100, 100, CV_16FC1);
		regionContour = cv::Mat::zeros(100, 100, CV_16FC1);
		Algorithm::insideContour(src, dst, regionContour);
	}

	TEST_METHOD(getContourHeight_InvalidInput)
	{
		std::vector<cv::Point> contour;
		Algorithm::getContourHeight(contour);

		contour.push_back(cv::Point(0, 0));
		Algorithm::getContourHeight(contour);
	}

	TEST_METHOD(medianHeight_InvalidInput)
	{
		std::vector<std::vector<cv::Point>> contours;
		Algorithm::medianHeight(contours);

		contours.push_back(std::vector<cv::Point>(1, cv::Point(0, 0)));
		Algorithm::medianHeight(contours);
	}

	TEST_METHOD(denoise_InvalidInput)
	{
		cv::Mat src, dst;
		float percent;
		Algorithm::denoise(src, dst, percent);

		src = cv::Mat::zeros(100, 100, CV_8UC1);
		percent = -1;
		Algorithm::denoise(src, dst, percent);

		for (int i = 0; i < 12; i = i + 2)
		{
			src.ptr<uchar>(i, 0)[0] = 255;
			src.ptr<uchar>(i, 1)[0] = 255;
		}
		Algorithm::denoise(src, dst, percent);
	}

	TEST_METHOD(denoise_InvalidChannelsNumber)
	{
		cv::Mat src = cv::Mat::zeros(100, 100, CV_8UC3);
		cv::Mat dst;
		Algorithm::denoise(src, dst);
	}

	TEST_METHOD(denoise_InvalidType)
	{
		cv::Mat src = cv::Mat::zeros(100, 100, CV_16FC1);
		cv::Mat dst;
		Algorithm::denoise(src, dst);
	}

	TEST_METHOD(charsBBoxes_InvalidInput)
	{
		cv::Mat src;
		std::vector<cv::Rect> chars;
		Algorithm::charsBBoxes(src, chars);

		src = cv::Mat::zeros(100, 100, CV_8UC1);
		Algorithm::charsBBoxes(src, chars);
	}

	TEST_METHOD(charsBBoxes_InvalidChannelsNumber)
	{
		cv::Mat src;
		std::vector<cv::Rect> chars;

		src = cv::Mat::zeros(100, 100, CV_8UC3);
		Algorithm::charsBBoxes(src, chars);
	}

	TEST_METHOD(charsBBoxes_InvalidType)
	{
		cv::Mat src;
		std::vector<cv::Rect> chars;

		src = cv::Mat::zeros(100, 100, CV_16FC1);
		Algorithm::charsBBoxes(src, chars);
	}

	TEST_METHOD(firstIndexes_InvalidInput)
	{
		std::vector<cv::Rect> chars;
		std::array<int, 3> indexes;
		Algorithm::firstIndexes(chars, indexes);

		chars.push_back(cv::Rect(0, 0, 0, 0));
		Algorithm::firstIndexes(chars, indexes);
	}

	TEST_METHOD(paddingChars_InvalidInput)
	{
		std::vector<cv::Rect> src;
		std::vector<cv::Rect> dst;
		float percent;
		Algorithm::paddingChars(src, dst, percent);

		src.push_back(cv::Rect(0, 0, 0, 0));
		percent = -1;
		Algorithm::paddingChars(src, dst);
	}

	TEST_METHOD(charsSpacing_InvalidInput)
	{
		cv::Mat src, dst;
		std::vector<cv::Rect> chars, paddedChars;
		Algorithm::charsSpacing(src, dst, chars, paddedChars);

		src = cv::Mat::zeros(100, 100, CV_8UC1);
		chars.push_back(cv::Rect(0, 0, 0, 0));
		paddedChars.push_back(cv::Rect(0, 0, 0, 0));
		Algorithm::charsSpacing(src, dst, chars, paddedChars);

		chars.clear();
		paddedChars.push_back(cv::Rect(0, 0, 0, 0));
		Algorithm::charsSpacing(src, dst, chars, paddedChars);

		chars.push_back(cv::Rect(0, 0, 0, 0));
		paddedChars.clear();
		Algorithm::charsSpacing(src, dst, chars, paddedChars);
	}

	TEST_METHOD(charsSpacing_InvalidChannelsNumber)
	{
		cv::Mat src;
		cv::Mat dst;
		std::vector<cv::Rect> chars(1, cv::Rect(0, 0, 0, 0));
		std::vector<cv::Rect> paddedChars(1, cv::Rect(0, 0, 0, 0));

		src = cv::Mat::zeros(100, 100, CV_8UC3);
		Algorithm::charsSpacing(src, dst, chars, paddedChars);
	}

	TEST_METHOD(charsSpacing_InvalidType)
	{
		cv::Mat src;
		cv::Mat dst;
		std::vector<cv::Rect> chars(1, cv::Rect(0, 0, 0, 0));
		std::vector<cv::Rect> paddedChars(1, cv::Rect(0, 0, 0, 0));

		src = cv::Mat::zeros(100, 100, CV_16FC1);
		Algorithm::charsSpacing(src, dst, chars, paddedChars);
	}

	TEST_METHOD(wordsSeparation_InvalidInput)
	{
		std::vector<cv::Rect> chars;
		std::array<std::vector<cv::Rect>, 3> words;
		std::array<int, 3> indexes;
		Algorithm::wordsSeparation(chars, words, indexes);

		chars.push_back(cv::Rect(0, 0, 0, 0));
		Algorithm::wordsSeparation(chars, words, indexes);
	}

	TEST_METHOD(verifyOutputText_InvalidInput)
	{
		tesseract::TessBaseAPI tess;
		tess.Init(NULL, "DIN1451Mittelschrift", tesseract::OEM_LSTM_ONLY);
		float confidence;
		Algorithm::verifyOutputText(tess, confidence);

		cv::Mat image = cv::Mat::zeros(100, 100, CV_8UC1);
		tess.SetImage(image.data, image.cols, image.rows, 1, image.cols);
		Algorithm::verifyOutputText(tess, confidence);
	}

	TEST_METHOD(resizeCharTemplate_InvalidInput)
	{
		cv::Mat src, dst;
		cv::Size size;
		Algorithm::resizeCharTemplate(src, dst, size);

		src = cv::Mat::zeros(100, 100, CV_8UC1);
		size = cv::Size(-1, -1);
		Algorithm::resizeCharTemplate(src, dst, size);

		size = cv::Size(200, 200);
		Algorithm::resizeCharTemplate(src, dst, size);

		src.ptr<uchar>(0, 0)[0] = 255;
		src.ptr<uchar>(0, 1)[0] = 255;
		Algorithm::resizeCharTemplate(src, dst, size);
	}

	TEST_METHOD(resizeCharTemplate_InvalidChannelsNumber)
	{
		cv::Mat src, dst;
		cv::Size size(100, 100);

		src = cv::Mat::zeros(100, 100, CV_8UC3);
		Algorithm::resizeCharTemplate(src, dst, size);
	}

	TEST_METHOD(resizeCharTemplate_InvalidType)
	{
		cv::Mat src, dst;
		cv::Size size(100, 100);

		src = cv::Mat::zeros(100, 100, CV_16FC1);
		Algorithm::resizeCharTemplate(src, dst, size);
	}

	TEST_METHOD(padding_InvalidInput)
	{
		int firstSize, secondSize, firstPadding, secondPadding;
		Algorithm::padding(firstSize, secondSize, firstPadding, secondPadding);

		firstSize = 0;
		secondSize = 100;
		Algorithm::padding(firstSize, secondSize, firstPadding, secondPadding);

		firstSize = 100;
		secondSize = 0;
		Algorithm::padding(firstSize, secondSize, firstPadding, secondPadding);

		firstSize = 100;
		secondSize = 100;
		Algorithm::padding(firstSize, secondSize, firstPadding, secondPadding);

		firstSize = -1;
		secondSize = -1;
		Algorithm::padding(firstSize, secondSize, firstPadding, secondPadding);
	}

	TEST_METHOD(matching_InvalidInput)
	{
		cv::Mat src;
		float dice, percent;
		Algorithm::matching(src, dice, percent);

		src = cv::Mat::zeros(100, 100, CV_8UC1);
		percent = -1;
		Algorithm::matching(src, dice, percent);
	}

	TEST_METHOD(matching_InvalidChannelsNumber)
	{
		cv::Mat src;
		float dice;

		src = cv::Mat::zeros(100, 100, CV_8UC3);
		Algorithm::matching(src, dice);
	}

	TEST_METHOD(matching_InvalidType)
	{
		cv::Mat src;
		float dice;

		src = cv::Mat::zeros(100, 100, CV_16FC1);
		Algorithm::matching(src, dice);
	}

	TEST_METHOD(applyTesseract_InvalidInput)
	{
		cv::Mat src;
		std::string text;
		std::vector<cv::Rect> chars, paddedChars;
		bool charType;
		float confidence;
		Algorithm::applyTesseract(src, text, chars, paddedChars, charType, confidence);

		paddedChars.push_back(cv::Rect(0, 0, 10, 10));
		Algorithm::applyTesseract(src, text, chars, paddedChars, charType, confidence);

		chars.push_back(cv::Rect(0, 0, 1, 1));
		Algorithm::applyTesseract(src, text, chars, paddedChars, charType, confidence);

		src = cv::Mat::zeros(100, 100, CV_8UC1);
		paddedChars.push_back(cv::Rect(0, 0, 1, 1));
		chars.push_back(cv::Rect(0, 0, 10, 10));
		Algorithm::applyTesseract(src, text, chars, paddedChars, charType, confidence);
	}

	TEST_METHOD(applyTesseract_InvalidChannelsNumber)
	{
		cv::Mat src;
		std::string text;
		std::vector<cv::Rect> chars, paddedChars;
		bool charType;
		float confidence;

		src = cv::Mat::zeros(100, 100, CV_8UC3);
		chars.push_back(cv::Rect(0, 0, 1, 1));
		paddedChars.push_back(cv::Rect(0, 0, 10, 10));
		Algorithm::applyTesseract(src, text, chars, paddedChars, charType, confidence);
	}

	TEST_METHOD(applyTesseract_InvalidType)
	{
		cv::Mat src;
		std::string text;
		std::vector<cv::Rect> chars, paddedChars;
		bool charType;
		float confidence;

		src = cv::Mat::zeros(100, 100, CV_16FC1);
		chars.push_back(cv::Rect(0, 0, 1, 1));
		paddedChars.push_back(cv::Rect(0, 0, 10, 10));
		Algorithm::applyTesseract(src, text, chars, paddedChars, charType, confidence);
	}

	TEST_METHOD(readText_InvalidInput)
	{
		cv::Mat src;
		std::string text;
		float confidence;
		std::array<std::vector<cv::Rect>, 3> words;
		std::array<std::vector<cv::Rect>, 3> paddedWords;
		Algorithm::readText(src, text, confidence, words, paddedWords);

		src = cv::Mat::zeros(100, 100, CV_8UC1);
		Algorithm::readText(src, text, confidence, words, paddedWords);
	}

	TEST_METHOD(readText_InvalidChannelsNumber)
	{
		cv::Mat src;
		std::string text;
		float confidence;
		std::array<std::vector<cv::Rect>, 3> words;
		std::array<std::vector<cv::Rect>, 3> paddedWords;

		src = cv::Mat::zeros(100, 100, CV_8UC3);
		Algorithm::readText(src, text, confidence, words, paddedWords);
	}

	TEST_METHOD(readText_InvalidType)
	{
		cv::Mat src;
		std::string text;
		float confidence;
		std::array<std::vector<cv::Rect>, 3> words;
		std::array<std::vector<cv::Rect>, 3> paddedWords;

		src = cv::Mat::zeros(100, 100, CV_16FC1);
		Algorithm::readText(src, text, confidence, words, paddedWords);
	}

	TEST_METHOD(drawBBoxes_InvalidInput)
	{
		cv::Mat dst;
		cv::Rect roi;
		std::string time, text;
		float confidence;
		Algorithm::drawBBoxes(dst, roi, time, text, confidence);

		dst = cv::Mat::zeros(100, 100, CV_8UC3);
		roi = cv::Rect(0, 0, 0, 0);
		time = "";
		text = "";
		confidence = -1;
		Algorithm::drawBBoxes(dst, roi, time, text, confidence);
	}

	TEST_METHOD(drawBBoxes_InvalidChannelsNumber)
	{
		cv::Mat dst;
		cv::Rect roi;
		std::string time, text;
		float confidence;

		dst = cv::Mat::zeros(100, 100, CV_8UC1);
		Algorithm::drawBBoxes(dst, roi, time, text, confidence);

		dst = cv::Mat::zeros(100, 100, CV_8UC4);
		Algorithm::drawBBoxes(dst, roi, time, text, confidence);
	}

	TEST_METHOD(drawBBoxes_InvalidType)
	{
		cv::Mat dst;
		cv::Rect roi;
		std::string time, text;
		float confidence;

		dst = cv::Mat::zeros(100, 100, CV_16FC1);
		Algorithm::drawBBoxes(dst, roi, time, text, confidence);
	}

	TEST_METHOD(textFromImage_InvalidInput)
	{
		cv::Mat src, dst;
		textFromImage(src, dst);

		src = cv::Mat::zeros(100, 100, CV_8UC3);
		textFromImage(src, dst);
	}

	TEST_METHOD(textFromImage_InvalidChannelsNumber)
	{
		cv::Mat src, dst;

		src = cv::Mat::zeros(100, 100, CV_8UC1);
		textFromImage(src, dst);

		src = cv::Mat::zeros(100, 100, CV_8UC4);
		textFromImage(src, dst);
	}

	TEST_METHOD(textFromImage_InvalidType)
	{
		cv::Mat src, dst;

		src = cv::Mat::zeros(100, 100, CV_16FC1);
		textFromImage(src, dst);
	}
};