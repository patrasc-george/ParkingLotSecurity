#include <filesystem>

#include "CppUnitTest.h"
#include "../src/ImageProcessingUtils/ImageProcessingUtils.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

std::string absolutePath(std::string relativePath)
{
	std::filesystem::path path = std::filesystem::current_path().parent_path().parent_path();
	return std::string(path.string() + "/../tests/data/" + relativePath);
}

float RMS(const cv::Mat& first, const cv::Mat& second)
{
	if (first.size() != second.size())
		return 1;

	float result = 0;
	for (int i = 0; i < first.rows; i++)
		for (int j = 0; j < first.cols; j++)
		{
			cv::Vec3b firstPixel;
			cv::Vec3b secondPixel;

			if (first.channels() == 1)
			{
				firstPixel = cv::Vec3b(first.ptr<uchar>(i, j)[0], first.ptr<uchar>(i, j)[0], first.ptr<uchar>(i, j)[0]);
				secondPixel = cv::Vec3b(second.ptr<uchar>(i, j)[0], second.ptr<uchar>(i, j)[0], second.ptr<uchar>(i, j)[0]);
			}
			else
			{
				firstPixel = first.ptr<cv::Vec3b>(i, j)[0];
				secondPixel = second.ptr<cv::Vec3b>(i, j)[0];
			}
			float sumDifferences = 0;

			for (int k = 0; k < 3; k++)
			{
				float difference = cv::abs(firstPixel[k] - secondPixel[k]);
				sumDifferences = sumDifferences + std::pow(difference, 2);
			}

			result = result + sqrt(sumDifferences);
		}
	return result / (first.rows * first.cols * (255 / sqrt(3)));
}

TEST_CLASS(AlgorithmTests)
{
public:
	TEST_METHOD(BGR2HSV_InvalidInput)
	{
		cv::Mat src, dst;

		Algorithm::BGR2HSV(src, dst);
		Assert::IsTrue(dst.empty());

		src = cv::Mat::zeros(100, 100, CV_16FC3);
		Algorithm::BGR2HSV(src, dst);
		Assert::IsTrue(dst.empty());
	}

	TEST_METHOD(BGR2HSV_ValidInput)
	{
		cv::Mat src, dst, aux;

		src = cv::imread(absolutePath("lenna.jpg"));
		cv::cvtColor(src, aux, cv::COLOR_BGR2HSV);
		Algorithm::BGR2HSV(src, dst);
		float error = RMS(dst, aux);
		Assert::IsTrue(error < 0.05);
	}

	TEST_METHOD(HSV2Binary_InvalidInput)
	{
		cv::Mat src, dst;
		uchar threshold;

		Algorithm::HSV2Binary(src, dst, threshold);
		Assert::IsTrue(dst.empty());

		src = cv::Mat::zeros(100, 100, CV_16FC3);
		threshold = 128;
		Algorithm::HSV2Binary(src, dst, threshold);
		Assert::IsTrue(dst.empty());
	}

	TEST_METHOD(HSV2Binary_ValidInput)
	{
		cv::Mat src, dst;
		uchar threshold = 125;

		src = cv::imread(absolutePath("lenna.jpg"));

		std::vector<cv::Mat> channels(3);
		cv::split(src, channels);
		channels[1] = 100;
		channels[2] = 150;
		cv::merge(channels, src);

		Algorithm::HSV2Binary(src, dst, threshold);
		Assert::IsTrue(!cv::countNonZero(255 - dst));
	}

	TEST_METHOD(getConnectedComponents_InvalidInput)
	{
		cv::Mat src, stats;
		std::vector<std::pair<int, int>> areas;
		int newSize;

		Algorithm::getConnectedComponents(src, stats, areas, newSize);
		Assert::IsTrue(areas.empty());

		src = cv::Mat::zeros(100, 100, CV_16FC1);
		Algorithm::getConnectedComponents(src, stats, areas);
		Assert::IsTrue(areas.empty());

		src = cv::Mat::zeros(100, 100, CV_8UC1);
		newSize = -10;
		Algorithm::getConnectedComponents(src, stats, areas, newSize);
		Assert::IsTrue(areas.empty());
	}

	TEST_METHOD(getConnectedComponents_ValidInput)
	{
		cv::Mat src, stats;
		std::vector<std::pair<int, int>> areas;
		int newSize;

		src = cv::Mat::zeros(100, 100, CV_8UC1);

		cv::rectangle(src, cv::Point(10, 10), cv::Point(20, 20), cv::Scalar(255), cv::FILLED);

		Algorithm::getConnectedComponents(src, stats, areas, newSize);
		Assert::AreEqual(1, (int)areas.size());
	}

	TEST_METHOD(getRoi_InvalidInput)
	{
		cv::Mat stats;
		cv::Rect roi;
		int label;

		Algorithm::getRoi(stats, roi, label);
		Assert::IsTrue(!roi.area());

		label = 1;
		Algorithm::getRoi(stats, roi, label);
		Assert::IsTrue(!roi.area());

		stats = cv::Mat::zeros(100, 100, CV_32SC1);
		label = -1;
		Algorithm::getRoi(stats, roi, label);
		Assert::IsTrue(!roi.area());

		label = 1;
		Algorithm::getRoi(stats, roi, label);
		Assert::IsTrue(!roi.area());

		stats = cv::Mat::zeros(100, 100, CV_16FC1);
		Algorithm::getRoi(stats, roi, label);
		Assert::IsTrue(!roi.area());
	}

	TEST_METHOD(getRoi_ValidInput)
	{
		cv::Mat stats;
		cv::Rect roi;
		int label;

		stats = cv::Mat::zeros(100, 100, CV_32SC1);
		label = 1;

		stats.ptr<int>(label, cv::CC_STAT_LEFT)[0] = 0;
		stats.ptr<int>(label, cv::CC_STAT_TOP)[0] = 0;
		stats.ptr<int>(label, cv::CC_STAT_WIDTH)[0] = 100;
		stats.ptr<int>(label, cv::CC_STAT_HEIGHT)[0] = 100;

		Algorithm::getRoi(stats, roi, label);
		Assert::AreEqual(0, roi.x);
		Assert::AreEqual(0, roi.y);
		Assert::AreEqual(100, roi.width);
		Assert::AreEqual(100, roi.height);
	}

	TEST_METHOD(sizeBBox_InvalidInput)
	{
		cv::Mat src;
		cv::Rect roi;
		float min, max;
		bool result;

		result = Algorithm::sizeBBox(src, roi, min, max);
		Assert::IsTrue(!result);

		min = 0.5;
		max = 0.4;
		result = Algorithm::sizeBBox(src, roi, min, max);
		Assert::IsTrue(!result);

		min = -0.5;
		max = -0.4;
		result = Algorithm::sizeBBox(src, roi, min, max);
		Assert::IsTrue(!result);

		min = 5;
		max = 4;
		result = Algorithm::sizeBBox(src, roi, min, max);
		Assert::IsTrue(!result);
	}

	TEST_METHOD(sizeBBox_ValidInput)
	{
		cv::Mat src;
		cv::Rect roi;
		float min, max;
		bool result;

		src = cv::Mat::zeros(100, 100, CV_8UC1);
		roi = cv::Rect(0, 0, 100, 10);
		min = 0.09;
		max = 0.11;
		result = Algorithm::sizeBBox(src, roi, min, max);
		Assert::IsTrue(result);
	}

	TEST_METHOD(heightBBox_InvalidInput)
	{
		cv::Rect roi;
		float min, max;
		bool result;

		result = Algorithm::heightBBox(roi, min, max);
		Assert::IsTrue(!result);

		min = 0.5;
		max = 0.4;
		result = Algorithm::heightBBox(roi, min, max);
		Assert::IsTrue(!result);

		min = -0.5;
		max = -0.4;
		result = Algorithm::heightBBox(roi, min, max);
		Assert::IsTrue(!result);

		min = 5;
		max = 4;
		result = Algorithm::heightBBox(roi, min, max);
		Assert::IsTrue(!result);
	}

	TEST_METHOD(heightBBox_ValidInput)
	{
		cv::Rect roi;
		float min, max;
		bool result;

		roi = cv::Rect(0, 0, 100, 10);
		min = 0.09;
		max = 0.11;
		result = Algorithm::heightBBox(roi, min, max);
		Assert::IsTrue(result);
	}

	TEST_METHOD(paddingRect_InvalidInput)
	{
		cv::Rect src(0, 0, 10, 10), dst;
		float percent;
		bool square;
		cv::Size size;

		percent = 0;
		square = false;
		Algorithm::paddingRect(src, dst, percent, square, size);
		Assert::AreEqual(dst.x, src.x - 3);
		Assert::AreEqual(dst.y, src.y - 3);
		Assert::AreEqual(dst.width, src.width + 6);
		Assert::AreEqual(dst.height, src.height + 6);

		square = true;
		Algorithm::paddingRect(src, dst, percent, square);
		Assert::AreEqual(dst.x, src.x - 3);
		Assert::AreEqual(dst.y, src.y - 3);
		Assert::AreEqual(dst.width, src.width + 6);
		Assert::AreEqual(dst.height, src.height + 6);

		size = cv::Size(100, 100);
		Algorithm::paddingRect(src, dst, percent, square, size);
		Assert::AreEqual(dst.x, src.x);
		Assert::AreEqual(dst.y, src.y);
		Assert::AreEqual(dst.width, src.width + 6);
		Assert::AreEqual(dst.height, src.height + 6);

		dst = cv::Rect(0, 0, 0, 0);
		percent = -1;
		Algorithm::paddingRect(src, dst, percent);
		Assert::AreEqual(dst.x, 0);
		Assert::AreEqual(dst.y, 0);
		Assert::AreEqual(dst.width, 0);
		Assert::AreEqual(dst.height, 0);

		size = cv::Size(-1, -1);
		Algorithm::paddingRect(src, dst, percent, square, size);
		Assert::AreEqual(dst.x, 0);
		Assert::AreEqual(dst.y, 0);
		Assert::AreEqual(dst.width, 0);
		Assert::AreEqual(dst.height, 0);
	}

	TEST_METHOD(paddingRect_ValidInput)
	{
		cv::Rect src(0, 0, 200, 100), dst;
		float percent;
		bool square;
		cv::Size size;

		percent = 0.1;
		Algorithm::paddingRect(src, dst, percent);
		Assert::AreEqual(dst.x, src.x - 20);
		Assert::AreEqual(dst.y, src.y - 10);
		Assert::AreEqual(dst.width, src.width + 40);
		Assert::AreEqual(dst.height, src.height + 20);

		square = true;
		Algorithm::paddingRect(src, dst, percent, square);
		Assert::AreEqual(dst.x, src.x - 20);
		Assert::AreEqual(dst.y, src.y - 20);
		Assert::AreEqual(dst.width, src.width + 40);
		Assert::AreEqual(dst.height, src.height + 40);

		size = cv::Size(1000, 1000);
		Algorithm::paddingRect(src, dst, percent, square, size);
		Assert::AreEqual(dst.x, 0);
		Assert::AreEqual(dst.y, 0);
		Assert::AreEqual(dst.width, src.width + 40);
		Assert::AreEqual(dst.height, src.height + 40);
	}

	TEST_METHOD(blueToBlack_InvalidInput)
	{
		cv::Mat src, dst;

		Algorithm::blueToBlack(src, dst);
		Assert::IsTrue(dst.empty());

		src = cv::Mat::zeros(100, 100, CV_16FC3);
		Algorithm::blueToBlack(src, dst);
		Assert::IsTrue(dst.empty());
	}

	TEST_METHOD(blueToBlack_ValidInput)
	{
		cv::Mat src, dst;

		src = cv::Mat::zeros(100, 100, CV_8UC3);

		for (int i = 0; i < src.rows; i++)
			for (int j = 0; j < src.cols / 2; j++)
			{
				src.ptr<uchar>(i, j)[0] = 115;
				src.ptr<uchar>(i, j)[1] = 160;
				src.ptr<uchar>(i, j)[2] = 150;
			}

		Algorithm::blueToBlack(src, dst);
		cv::cvtColor(dst, dst, cv::COLOR_BGR2GRAY);
		Assert::IsTrue(!cv::countNonZero(dst));
	}

	TEST_METHOD(HSV2BGR_InvalidInput)
	{
		cv::Mat src, dst;

		Algorithm::HSV2BGR(src, dst);
		Assert::IsTrue(dst.empty());

		src = cv::Mat::zeros(100, 100, CV_16FC1);
		Algorithm::HSV2BGR(src, dst);
		Assert::IsTrue(dst.empty());
	}

	TEST_METHOD(HSV2BGR_ValidInput)
	{
		cv::Mat src, dst, aux;

		src = cv::imread(absolutePath("lenna.jpg"));

		cv::cvtColor(src, src, cv::COLOR_BGR2HSV);
		cv::cvtColor(src, aux, cv::COLOR_HSV2BGR);
		Algorithm::HSV2BGR(src, dst);

		float error = RMS(dst, aux);
		Assert::IsTrue(error < 0.001);
	}

	TEST_METHOD(histogram_InvalidInput)
	{
		cv::Mat src, dst;

		Algorithm::histogram(src, dst);
		Assert::IsTrue(dst.empty());

		src = cv::Mat::zeros(100, 100, CV_16SC1);
		Algorithm::histogram(src, dst);
		Assert::IsTrue(dst.empty());
	}

	TEST_METHOD(histogram_ValidInput)
	{
		cv::Mat src, dst;

		src = cv::Mat::zeros(100, 100, CV_8UC1);

		for (int i = 0; i < src.rows; i++)
			for (int j = 0; j < src.cols / 2; j++)
				src.ptr<uchar>(i, j)[0] = 1;

		Algorithm::histogram(src, dst);
		Assert::IsTrue(dst.size() == cv::Size(1, 2) && dst.ptr<float>(0, 0)[0] == 5000 && dst.ptr<float>(1, 0)[0] == 5000);
	}

	TEST_METHOD(cumulativeHistogram_InvalidInput)
	{
		cv::Mat src, dst;

		Algorithm::cumulativeHistogram(src, dst);
		Assert::IsTrue(dst.empty());

		src = cv::Mat::zeros(100, 100, CV_16SC1);
		Algorithm::cumulativeHistogram(src, dst);
		Assert::IsTrue(dst.empty());
	}

	TEST_METHOD(cumulativeHistogram_ValidInput)
	{
		cv::Mat src, dst;

		src = cv::Mat(3, 1, CV_32FC1);

		src.ptr<float>(0, 0)[0] = 1;
		src.ptr<float>(1, 0)[0] = 2;
		src.ptr<float>(2, 0)[0] = 3;

		Algorithm::cumulativeHistogram(src, dst);
		Assert::IsTrue(dst.ptr<float>(0, 0)[0] == 1 && dst.ptr<float>(0, 1)[0] == 3 && dst.ptr<float>(0, 2)[0] == 6);
	}

	TEST_METHOD(histogramLine_InvalidInput)
	{
		cv::Mat src;
		cv::Vec4f dst;

		Algorithm::histogramLine(src, dst);
		Assert::IsTrue(dst == cv::Vec4f(0, 0, 0, 0));

		src = cv::Mat::zeros(100, 100, CV_16SC1);
		Algorithm::histogramLine(src, dst);
		Assert::IsTrue(dst == cv::Vec4f(0, 0, 0, 0));
	}

	TEST_METHOD(histogramLine_ValidInput)
	{
		cv::Mat src;
		cv::Vec4f dst;

		src = cv::Mat(1, 3, CV_32FC1);
		src.ptr<float>(0, 0)[0] = 1;
		src.ptr<float>(0, 1)[0] = 3;
		src.ptr<float>(0, 2)[0] = 6;

		Algorithm::histogramLine(src, dst);
		Assert::IsTrue(dst == cv::Vec4f(0, 1, 2, 6));
	}

	TEST_METHOD(distance_InvalidInput)
	{
		float x, y, dist;
		cv::Vec4f line;

		dist = Algorithm::distance(x, y, line);
		Assert::AreEqual(dist, 0.0f);

		x = 0;
		y = 0;
		line = cv::Vec4f(0, 0, 0, 0);
		dist = Algorithm::distance(x, y, line);
		Assert::AreEqual(dist, 0.0f);
	}

	TEST_METHOD(distance_ValidInput)
	{
		float x, y, dist;
		cv::Vec4f line;

		x = 0;
		y = 0;
		line = cv::Vec4f(0, 5, 10, 5);
		dist = Algorithm::distance(x, y, line);
		Assert::AreEqual(dist, 5.0f);
	}

	TEST_METHOD(triangleThresholding_InvalidInput)
	{
		cv::Mat src, dst;

		Algorithm::triangleThresholding(src, dst);
		Assert::IsTrue(dst.empty());

		src = cv::Mat::zeros(100, 100, CV_16FC1);
		Algorithm::triangleThresholding(src, dst);
		Assert::IsTrue(dst.empty());
	}

	TEST_METHOD(triangleThresholding_ValidInput)
	{
		cv::Mat src, dst, aux;

		src = cv::Mat::zeros(100, 100, CV_8UC1);

		aux = src.clone();
		for (int i = 0; i < src.rows; i++)
			for (int j = 0; j < src.cols / 2; j++)
			{
				src.ptr<uchar>(i, j)[0] = 125;
				aux.ptr<uchar>(i, j)[0] = 255;
			}

		Algorithm::triangleThresholding(src, dst);
		float error = RMS(dst, aux);
		Assert::IsTrue(error < 0.01);
	}

	TEST_METHOD(binarySobel_InvalidInput)
	{
		cv::Mat src, dst, direction;

		Algorithm::binarySobel(src, dst, direction);
		Assert::IsTrue(dst.empty() && direction.empty());

		src = cv::Mat::zeros(100, 100, CV_16FC3);
		Algorithm::binarySobel(src, dst, direction);
		Assert::IsTrue(dst.empty() && direction.empty());
	}

	TEST_METHOD(binarySobel_ValidInput)
	{
		cv::Mat src, dst, direction, aux;

		src = cv::Mat::zeros(100, 100, CV_8UC1);

		aux = cv::Mat::zeros(100, 100, CV_8UC1);
		for (int i = 0; i < src.rows; i++)
			for (int j = 0; j < src.cols / 2; j++)
				src.ptr<uchar>(i, j)[0] = 125;
		for (int i = 0; i < src.rows; i++)
			for (int j = src.cols / 2 - 1; j < src.cols / 2 + 1; j++)
				aux.ptr<uchar>(i, j)[0] = 255;

		Algorithm::binarySobel(src, dst, direction);
		float error = RMS(dst, aux);
		Assert::IsTrue(error < 0.01);
	}

	TEST_METHOD(nonMaximumSuppression_InvalidInput)
	{
		cv::Mat src, dst, direction;

		Algorithm::nonMaximumSuppression(src, dst, direction);
		Assert::IsTrue(dst.empty());

		src = cv::Mat::zeros(100, 200, CV_8UC1);
		direction = cv::Mat::zeros(200, 100, CV_8UC1);
		Algorithm::nonMaximumSuppression(src, dst, direction);
		Assert::IsTrue(dst.empty());

		src = cv::Mat::zeros(100, 100, CV_16FC1);
		direction = cv::Mat::zeros(100, 100, CV_8UC1);
		Algorithm::nonMaximumSuppression(src, dst, direction);
		Assert::IsTrue(dst.empty());
	}

	TEST_METHOD(nonMaximumSuppression_ValidInput)
	{
		cv::Mat src, dst, direction, aux;

		src = cv::Mat::zeros(100, 100, CV_8UC1);
		dst = cv::Mat::zeros(100, 100, CV_8UC1);
		direction = cv::Mat::zeros(100, 100, CV_32FC1);

		for (int i = 0; i < src.rows; i++)
			for (int j = src.cols / 2 - 1; j < src.cols / 2 + 1; j++)
			{
				src.ptr<uchar>(i, j)[0] = 255;
				direction.ptr<float>(i, j)[0] = 180;
			}
		aux = cv::Mat::zeros(100, 100, CV_8UC1);
		for (int i = 2; i < src.rows - 2; i++)
			aux.ptr<uchar>(i, src.cols / 2)[0] = 255;

		Algorithm::nonMaximumSuppression(src, dst, direction);
		float error = RMS(dst, aux);
		Assert::IsTrue(error < 0.01);
	}

	TEST_METHOD(edgeDetection_InvalidInput)
	{
		cv::Mat src, dst;

		Algorithm::edgeDetection(src, dst);
		Assert::IsTrue(dst.empty());

		src = cv::Mat::zeros(100, 100, CV_16FC3);
		Algorithm::edgeDetection(src, dst);
		Assert::IsTrue(dst.empty());
	}

	TEST_METHOD(edgeDetection_ValidInput)
	{
		cv::Mat src, dst, aux;

		src = cv::Mat::zeros(100, 100, CV_8UC1);
		dst = cv::Mat::zeros(100, 100, CV_8UC1);

		for (int i = 0; i < src.rows; i++)
			for (int j = 0; j < src.cols / 2; j++)
				src.ptr<uchar>(i, j)[0] = 125;

		aux = cv::Mat::zeros(100, 100, CV_8UC1);
		for (int i = 2; i < src.rows - 2; i++)
			aux.ptr<uchar>(i, src.cols / 2)[0] = 255;

		Algorithm::edgeDetection(src, dst);
		float error = RMS(dst, aux);
		Assert::IsTrue(error < 0.01);
	}

	TEST_METHOD(bitwiseNand_InvalidInput)
	{
		cv::Mat src, edges;

		Algorithm::bitwiseNand(src, edges);
		Assert::IsTrue(src.empty());

		src = cv::Mat::zeros(200, 100, CV_8UC1);
		edges = cv::Mat::zeros(100, 200, CV_8UC1);
		Algorithm::bitwiseNand(src, edges);
		Assert::IsTrue(!cv::countNonZero(src));
	}

	TEST_METHOD(bitwiseNand_ValidInput)
	{
		cv::Mat src, edges, aux;

		src = cv::Mat::zeros(100, 100, CV_8UC1);
		edges = cv::Mat::zeros(100, 100, CV_8UC1);

		for (int i = 0; i < src.rows; i++)
			for (int j = 0; j < 3 * src.cols / 4; j++)
				src.ptr<uchar>(i, j)[0] = 255;
		for (int i = 0; i < edges.rows; i++)
			for (int j = 1 * edges.cols / 4; j < src.cols; j++)
				edges.ptr<uchar>(i, j)[0] = 255;
		aux = cv::Mat::zeros(100, 100, CV_8UC1);
		for (int i = 0; i < edges.rows; i++)
			for (int j = 0; j < 1 * edges.cols / 4; j++)
				aux.ptr<uchar>(i, j)[0] = 255;

		Algorithm::bitwiseNand(src, edges);
		float error = RMS(src, aux);
		Assert::IsTrue(error < 0.01);
	}

	TEST_METHOD(getLargestContour_InvalidInput)
	{
		std::vector<std::vector<cv::Point>> contours;
		std::vector<cv::Point> largestContour;

		Algorithm::getLargestContour(contours, largestContour);
		Assert::IsTrue(largestContour.empty());
	}

	TEST_METHOD(getLargestContour_ValidInput)
	{
		std::vector<std::vector<cv::Point>> contours;
		std::vector<cv::Point> largestContour;
		cv::Mat aux;

		aux = cv::Mat::zeros(100, 100, CV_8UC1);
		cv::rectangle(aux, cv::Point(10, 10), cv::Point(20, 20), cv::Scalar(255), cv::FILLED);
		cv::circle(aux, cv::Point(50, 50), 10, cv::Scalar(255), cv::FILLED);
		cv::findContours(aux, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

		Algorithm::getLargestContour(contours, largestContour);
		Assert::IsTrue(largestContour == contours[0]);
	}

	TEST_METHOD(roiContour_InvalidInput)
	{
		cv::Mat src, dst, edges;
		std::vector<cv::Point> largestContour;
		float percent;

		src = cv::Mat::zeros(100, 100, CV_16FC1);
		edges = cv::Mat::zeros(100, 100, CV_16FC1);
		Algorithm::roiContour(src, dst, largestContour, edges);
		Assert::IsTrue(dst.empty());

		Algorithm::roiContour(src, dst, largestContour, edges, percent);
		Assert::IsTrue(dst.empty());

		src = cv::Mat::zeros(100, 200, CV_8UC1);
		edges = cv::Mat::zeros(200, 100, CV_8UC1);
		percent = -1;
		Algorithm::roiContour(src, dst, largestContour, edges, percent);
		Assert::IsTrue(dst.empty());
	}

	TEST_METHOD(roiContour_ValidInput)
	{
		cv::Mat src, dst, edges, aux;
		std::vector<cv::Point> largestContour;
		std::vector<std::vector<cv::Point>> contours;

		src = cv::Mat::zeros(100, 100, CV_8UC1);
		edges = cv::Mat::zeros(100, 100, CV_8UC1);

		cv::rectangle(src, cv::Point(10, 10), cv::Point(20, 20), cv::Scalar(255), cv::FILLED);
		cv::circle(src, cv::Point(50, 50), 10, cv::Scalar(255), cv::FILLED);
		aux = cv::Mat::zeros(100, 100, CV_8UC1);
		cv::findContours(src, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
		cv::drawContours(aux, contours, 0, cv::Scalar(255), cv::FILLED);
		Algorithm::edgeDetection(aux, edges);
		Algorithm::bitwiseNand(aux, edges);

		edges = cv::Mat::zeros(src.size(), CV_8UC1);
		Algorithm::edgeDetection(src, edges);
		Algorithm::roiContour(src, dst, largestContour, edges);
		float error = RMS(dst, aux);
		Assert::IsTrue(error < 0.01);
	}

	TEST_METHOD(lineThroughPoint_InvalidInput)
	{
		cv::Vec4f line;
		double slope;
		cv::Point point;
		bool direction;

		Algorithm::lineThroughPoint(line, slope, point, direction);
		Assert::IsTrue(line == cv::Vec4f(0, 0, 0, 0));

		slope = 0;
		point = cv::Point(-1, -1);
		Algorithm::lineThroughPoint(line, slope, point, direction);
		Assert::IsTrue(line == cv::Vec4f(0, 0, 0, 0));

		direction = true;
		Algorithm::lineThroughPoint(line, slope, point, true);
		Assert::IsTrue(line == cv::Vec4f(0, 0, 0, 0));

		direction = false;
		Algorithm::lineThroughPoint(line, slope, point, false);
		Assert::IsTrue(line == cv::Vec4f(0, 0, 0, 0));
	}

	TEST_METHOD(lineThroughPoint_ValidInput)
	{
		cv::Vec4f line;
		double slope;
		cv::Point point;
		bool direction;

		slope = 0;
		point = cv::Point(50, 50);
		direction = true;
		Algorithm::lineThroughPoint(line, slope, point, direction);
		Assert::IsTrue(line == cv::Vec4f(0, 50, 100, 50));
	}

	TEST_METHOD(compareWithLine_InvalidInput)
	{
		std::vector<cv::Vec4i> lines;
		std::vector<cv::Vec4i> firstLines;
		std::vector<cv::Vec4i> secondLines;
		cv::Vec4f referenceLine;

		Algorithm::compareWithLine(lines, firstLines, secondLines, referenceLine);
		Assert::IsTrue(firstLines.empty());
		Assert::IsTrue(secondLines.empty());

		lines.push_back(cv::Vec4i(0, 0, 0, 0));
		referenceLine = cv::Vec4f(1, 1, 2, 2);
		Algorithm::compareWithLine(lines, firstLines, secondLines, referenceLine);
		Assert::IsTrue(firstLines.empty());
		Assert::IsTrue(secondLines.size() == 1);

		secondLines.clear();
		referenceLine = cv::Vec4f(0, 0, 0, 0);
		Algorithm::compareWithLine(lines, firstLines, secondLines, referenceLine);
		Assert::IsTrue(firstLines.empty());
		Assert::IsTrue(secondLines.empty());

		referenceLine = cv::Vec4f(-1, -1, -1, -1);
		Algorithm::compareWithLine(lines, firstLines, secondLines, referenceLine);
		Assert::IsTrue(firstLines.empty());
		Assert::IsTrue(secondLines.empty());
	}

	TEST_METHOD(compareWithLine_ValidInput)
	{
		std::vector<cv::Vec4i> lines;
		std::vector<cv::Vec4i> firstLines;
		std::vector<cv::Vec4i> secondLines;
		cv::Vec4f referenceLine;

		lines.push_back(cv::Vec4i(0, 0, 100, 0));
		lines.push_back(cv::Vec4i(0, 100, 100, 100));
		referenceLine = cv::Vec4f(0, 50, 100, 50);

		Algorithm::compareWithLine(lines, firstLines, secondLines, referenceLine);
		Assert::IsTrue(firstLines[0] == lines[0] && secondLines[0] == lines[1]);
	}

	TEST_METHOD(initialTerminalPoints_InvalidInput)
	{
		std::vector<cv::Vec4i> lines;
		bool direction;
		cv::Vec4i segment;

		segment = Algorithm::initialTerminalPoints(lines, direction);
		Assert::IsTrue(segment == cv::Vec4i(0, 0, 0, 0));
	}

	TEST_METHOD(initialTerminalPoints_ValidInput)
	{
		std::vector<cv::Vec4i> lines;
		bool direction;
		cv::Vec4i segment;

		lines.push_back(cv::Vec4i(12, 5, 100, 10));
		lines.push_back(cv::Vec4i(1, 3, 90, 8));
		direction = true;
		segment = Algorithm::initialTerminalPoints(lines, direction);
		Assert::IsTrue(segment == cv::Vec4i(1, 3, 100, 10));
	}

	TEST_METHOD(lineSorting_InvalidInput)
	{
		std::vector<cv::Vec4i> sortedLines;
		std::vector<cv::Vec4i> lines;
		cv::Size size;

#ifdef _DEBUG
		Algorithm::lineSorting(sortedLines, lines, size, cv::Mat());
#else
		Algorithm::lineSorting(sortedLines, lines, size);
#endif
		Assert::IsTrue(sortedLines.empty());

		lines.push_back(cv::Vec4i(0, 10, 0, 0));
#ifdef _DEBUG
		Algorithm::lineSorting(sortedLines, lines, size, cv::Mat());
#else
		Algorithm::lineSorting(sortedLines, lines, size);
#endif	
		Assert::IsTrue(sortedLines.empty());

		lines.clear();
		lines.push_back(cv::Vec4i(10, 0, 0, 0));
#ifdef _DEBUG
		Algorithm::lineSorting(sortedLines, lines, size, cv::Mat());
#else
		Algorithm::lineSorting(sortedLines, lines, size);
#endif	
		Assert::IsTrue(sortedLines.empty());

		lines.push_back(cv::Vec4i(0, 10, 0, 0));
#ifdef _DEBUG
		Algorithm::lineSorting(sortedLines, lines, size, cv::Mat());
#else
		Algorithm::lineSorting(sortedLines, lines, size);
#endif		
		Assert::IsTrue(sortedLines.empty());

		size = cv::Size(-1, -1);
#ifdef _DEBUG
		Algorithm::lineSorting(sortedLines, lines, size, cv::Mat());
#else
		Algorithm::lineSorting(sortedLines, lines, size);
#endif
		Assert::IsTrue(sortedLines.empty());
	}

	TEST_METHOD(lineSorting_ValidInput)
	{
		std::vector<cv::Vec4i> sortedLines;
		std::vector<cv::Vec4i> lines;
		cv::Size size;
		cv::Mat src;

		lines.push_back(cv::Vec4i(0, 0, 100, 0));
		lines.push_back(cv::Vec4i(0, 100, 100, 100));
		lines.push_back(cv::Vec4i(0, 0, 0, 100));
		lines.push_back(cv::Vec4i(100, 0, 100, 100));
		size = cv::Size(101, 101);
		src = cv::Mat::zeros(size, CV_8UC1);

#ifdef _DEBUG
		Algorithm::lineSorting(sortedLines, lines, size, src);
#else
		Algorithm::lineSorting(sortedLines, lines, size);
#endif

		Assert::IsTrue(sortedLines[0] == lines[2] && sortedLines[1] == lines[0] && sortedLines[2] == lines[3] && sortedLines[3] == lines[1]);
	}

	TEST_METHOD(intersection_InvalidInput)
	{
		cv::Vec4f line1, line2;

		Algorithm::intersection(line1, line2);
		Assert::IsTrue(line1 == cv::Vec4f(0, 0, 0, 0));

		line2 = cv::Vec4f(0, 0, 0, 10);
		Algorithm::intersection(line1, line2);
		Assert::IsTrue(line1 == cv::Vec4f(0, 0, 0, 0));

		line2 = cv::Vec4f(10, 0, 10, 10);
		Algorithm::intersection(line1, line2);
		Assert::IsTrue(line1 == cv::Vec4f(0, 0, 0, 0));

		line2 = line1;
		Algorithm::intersection(line1, line2);
		Assert::IsTrue(line1 == cv::Vec4f(0, 0, 0, 0));
	}

	TEST_METHOD(intersection_ValidInput)
	{
		cv::Vec4i line1, line2;
		cv::Point2f result;

		line1 = cv::Vec4i(0, 0, 100, 100);
		line2 = cv::Vec4i(0, 100, 100, 0);

		result = Algorithm::intersection(line1, line2);
		Assert::IsTrue(result == cv::Point2f(50, 50));
	}

	TEST_METHOD(cornersCoordinates_InvalidInput)
	{
		cv::Mat src;
		std::vector<cv::Point2f> quadrilateralCoordinates;
		std::vector<cv::Point> largestContour;

		Algorithm::cornersCoordinates(src, quadrilateralCoordinates, largestContour);
		Assert::IsTrue(quadrilateralCoordinates.empty());

		src = cv::Mat::zeros(100, 100, CV_8UC1);
		Algorithm::cornersCoordinates(src, quadrilateralCoordinates, largestContour);
		Assert::IsTrue(quadrilateralCoordinates.empty());

		largestContour = std::vector<cv::Point>(4, cv::Point(0, 0));
		Algorithm::cornersCoordinates(src, quadrilateralCoordinates, largestContour);
		Assert::IsTrue(quadrilateralCoordinates.empty());

		src = cv::Mat::zeros(100, 100, CV_16FC1);
		Algorithm::cornersCoordinates(src, quadrilateralCoordinates, largestContour);
		Assert::IsTrue(quadrilateralCoordinates.empty());
	}

	TEST_METHOD(cornersCoordinates_ValidInput)
	{
		cv::Mat src;
		std::vector<cv::Point2f> quadrilateralCoordinates;
		std::vector<cv::Point> largestContour;

		src = cv::Mat::zeros(100, 100, CV_8UC1);
		cv::rectangle(src, cv::Point(10, 10), cv::Point(80, 80), cv::Scalar(255), cv::FILLED);
		Algorithm::roiContour(src, src, largestContour);

		Algorithm::cornersCoordinates(src, quadrilateralCoordinates, largestContour);
		Assert::IsTrue(quadrilateralCoordinates[0] == cv::Point2f(10, 10));
		Assert::IsTrue(quadrilateralCoordinates[1] == cv::Point2f(80, 10));
		Assert::IsTrue(quadrilateralCoordinates[2] == cv::Point2f(80, 80));
		Assert::IsTrue(quadrilateralCoordinates[3] == cv::Point2f(10, 80));
	}

	TEST_METHOD(resizeToPoints_InvalidInput)
	{
		cv::Mat src, dst;
		std::vector<cv::Point2f> points;
		float percent;

		Algorithm::resizeToPoints(src, dst, points, percent);
		Assert::IsTrue(dst.empty());

		src = cv::Mat::zeros(100, 100, CV_8UC1);
		percent = -1;
		Algorithm::resizeToPoints(src, dst, points, percent);
		Assert::IsTrue(dst.empty());

		points = std::vector<cv::Point2f>(4, cv::Point2f(0, 0));
		Algorithm::resizeToPoints(src, dst, points, percent);
		Assert::IsTrue(dst.empty());

		src = cv::Mat::zeros(100, 100, CV_16FC1);
		Algorithm::resizeToPoints(src, dst, points);
		Assert::IsTrue(dst.empty());
	}

	TEST_METHOD(resizeToPoints_ValidType)
	{
		cv::Mat src;
		cv::Mat dst;
		std::vector<cv::Point2f> points;

		src = cv::Mat::zeros(100, 100, CV_8UC1);
		points.push_back(cv::Point2f(0, -50));
		points.push_back(cv::Point2f(150, 0));
		points.push_back(cv::Point2f(125, 100));
		points.push_back(cv::Point2f(0, 100));

		Algorithm::resizeToPoints(src, dst, points);
		Assert::IsTrue(dst.size() == cv::Size(151, 151));
	}

	TEST_METHOD(geometricalTransformation_InvalidInput)
	{
		cv::Mat src, dst;
		std::vector<cv::Point2f> quadrilateralCoordinates;
		float percent;

		Algorithm::geometricalTransformation(src, dst, quadrilateralCoordinates, percent);
		Assert::IsTrue(dst.empty());

		src = cv::Mat::zeros(100, 100, CV_8UC1);
		quadrilateralCoordinates = std::vector<cv::Point2f>(4, cv::Point2f(0, 0));
		percent = -1;
		Algorithm::geometricalTransformation(src, dst, quadrilateralCoordinates, percent);
		Assert::IsTrue(dst.empty());

		src = cv::Mat::zeros(100, 100, CV_16FC1);
		Algorithm::geometricalTransformation(src, dst, quadrilateralCoordinates);
		Assert::IsTrue(dst.empty());
	}

	TEST_METHOD(geometricalTransformation_ValidInput)
	{
		cv::Mat src, dst, aux;
		std::vector<cv::Point2f> quadrilateralCoordinates;
		std::vector<cv::Point> intCoordinates;

		src = cv::Mat::zeros(100, 100, CV_8UC1);

		quadrilateralCoordinates.push_back(cv::Point(20, 40));
		quadrilateralCoordinates.push_back(cv::Point(80, 40));
		quadrilateralCoordinates.push_back(cv::Point(70, 70));
		quadrilateralCoordinates.push_back(cv::Point(10, 70));
		for (const auto& point : quadrilateralCoordinates)
			intCoordinates.push_back(cv::Point(static_cast<int>(point.x), static_cast<int>(point.y)));
		std::vector<std::vector<cv::Point>> points{ intCoordinates };
		cv::fillPoly(src, points, cv::Scalar(255));
		cv::line(src, intCoordinates[0], intCoordinates[2], cv::Scalar(0), 1, cv::LINE_AA);
		cv::line(src, intCoordinates[1], intCoordinates[3], cv::Scalar(0), 1, cv::LINE_AA);
		aux = cv::Mat::zeros(30, 129, CV_8UC1);
		aux = 255 - aux;
		cv::line(aux, cv::Point(0, 0), cv::Point(aux.cols - 1, aux.rows - 1), cv::Scalar(0), 1, cv::LINE_AA);
		cv::line(aux, cv::Point(aux.cols - 1, 0), cv::Point(0, aux.rows - 1), cv::Scalar(0), 1, cv::LINE_AA);

		Algorithm::geometricalTransformation(src, dst, quadrilateralCoordinates);
		float error = RMS(dst, aux);
		Assert::IsTrue(error < 0.1);
	}

	TEST_METHOD(insideContour_InvalidInput)
	{
		cv::Mat src, dst;

		Algorithm::insideContour(src, dst);
		Assert::IsTrue(dst.empty());

		src = cv::Mat::zeros(100, 100, CV_16FC3);
		Algorithm::insideContour(src, dst);
		Assert::IsTrue(dst.empty());
	}

	TEST_METHOD(insideContour_ValidInput)
	{
		cv::Mat src, dst;

		src = cv::Mat::zeros(100, 100, CV_8UC1);

		src = 255 - src;
		cv::rectangle(src, cv::Point(10, 10), cv::Point(90, 90), cv::Scalar(0), cv::FILLED);

		Algorithm::insideContour(src, dst);
		src = 255 - src;
		float error = RMS(src, dst);
		Assert::IsTrue(error < 0.01);
	}

	TEST_METHOD(getContourHeight_InvalidInput)
	{
		std::vector<cv::Point> contour;
		int height;

		height = Algorithm::getContourHeight(contour);
		Assert::AreEqual(0, height);

		contour.push_back(cv::Point(0, 0));
		height = Algorithm::getContourHeight(contour);
		Assert::AreEqual(0, height);
	}

	TEST_METHOD(getContourHeight_ValidInput)
	{
		std::vector<cv::Point> contour;
		int height;

		contour.push_back(cv::Point(0, 50));
		contour.push_back(cv::Point(0, 75));
		contour.push_back(cv::Point(0, 100));
		height = Algorithm::getContourHeight(contour);
		Assert::AreEqual(height, 50);
	}

	TEST_METHOD(compareAreas_InvalidInput)
	{
		std::vector<cv::Point> a, b;
		bool result;

		result = Algorithm::compareAreas(a, b);
		Assert::AreEqual(result, false);
	}

	TEST_METHOD(compareAreas_ValidInput)
	{
		std::vector<cv::Point> a, b;
		bool result;

		a.push_back(cv::Point(0, 0));
		a.push_back(cv::Point(0, 75));
		a.push_back(cv::Point(0, 100));

		b.push_back(cv::Point(0, 0));
		b.push_back(cv::Point(0, 25));
		b.push_back(cv::Point(0, 50));

		result = Algorithm::compareAreas(a, b);
		Assert::AreEqual(result, true);
	}

	TEST_METHOD(medianHeight_InvalidInput)
	{
		std::vector<std::vector<cv::Point>> contours;
		int height;

		height = Algorithm::medianHeight(contours);
		Assert::AreEqual(0, height);

		contours.push_back(std::vector<cv::Point>(1, cv::Point(0, 0)));
		height = Algorithm::medianHeight(contours);
		Assert::AreEqual(0, height);
	}

	TEST_METHOD(medianHeight_ValidInput)
	{
		std::vector<std::vector<cv::Point>> contours;
		int height;
		std::vector<cv::Point> contour;

		contour.push_back(cv::Point(0, 50));
		contour.push_back(cv::Point(0, 100));
		contours.push_back(contour);
		contour.clear();
		contour.push_back(cv::Point(0, 0));
		contour.push_back(cv::Point(0, 70));
		contours.push_back(contour);
		contour.clear();
		contour.push_back(cv::Point(0, 0));
		contour.push_back(cv::Point(0, 20));
		contours.push_back(contour);
		contour.clear();

		height = Algorithm::medianHeight(contours);
		Assert::AreEqual(height, 50);
	}

	TEST_METHOD(denoise_InvalidInput)
	{
		cv::Mat src, dst;
		float percent;

		Algorithm::denoise(src, dst, percent);
		Assert::IsTrue(dst.empty());

		src = cv::Mat::zeros(100, 100, CV_8UC1);
		percent = -1;
		Algorithm::denoise(src, dst, percent);
		Assert::IsTrue(dst.empty());

		src = cv::Mat::zeros(100, 100, CV_16FC1);
		Algorithm::denoise(src, dst);
		Assert::IsTrue(dst.empty());
	}

	TEST_METHOD(denoise_ValidInput)
	{
		cv::Mat src, dst, aux;
		float percent = 0.15;

		src = cv::Mat::zeros(100, 100, CV_8UC1);

		cv::rectangle(src, cv::Point(10, 10), cv::Point(30, 40), cv::Scalar(255), cv::FILLED);
		cv::rectangle(src, cv::Point(40, 10), cv::Point(60, 42), cv::Scalar(255), cv::FILLED);
		cv::rectangle(src, cv::Point(70, 10), cv::Point(90, 38), cv::Scalar(255), cv::FILLED);
		cv::rectangle(src, cv::Point(10, 50), cv::Point(30, 80), cv::Scalar(255), cv::FILLED);
		cv::rectangle(src, cv::Point(40, 50), cv::Point(60, 78), cv::Scalar(255), cv::FILLED);
		cv::rectangle(src, cv::Point(70, 50), cv::Point(90, 79), cv::Scalar(255), cv::FILLED);
		cv::rectangle(src, cv::Point(50, 90), cv::Point(55, 95), cv::Scalar(255), cv::FILLED);
		aux = cv::Mat::zeros(100, 100, CV_8UC1);
		cv::rectangle(aux, cv::Point(10, 10), cv::Point(30, 40), cv::Scalar(255), cv::FILLED);
		cv::rectangle(aux, cv::Point(40, 10), cv::Point(60, 42), cv::Scalar(255), cv::FILLED);
		cv::rectangle(aux, cv::Point(70, 10), cv::Point(90, 38), cv::Scalar(255), cv::FILLED);
		cv::rectangle(aux, cv::Point(10, 50), cv::Point(30, 80), cv::Scalar(255), cv::FILLED);
		cv::rectangle(aux, cv::Point(40, 50), cv::Point(60, 78), cv::Scalar(255), cv::FILLED);
		cv::rectangle(aux, cv::Point(70, 50), cv::Point(90, 79), cv::Scalar(255), cv::FILLED);

		Algorithm::denoise(src, dst, percent);
		float error = RMS(dst, aux);
		Assert::IsTrue(error < 0.01);
	}

	TEST_METHOD(charsBBoxes_InvalidInput)
	{
		cv::Mat src;
		std::vector<cv::Rect> chars;

		Algorithm::charsBBoxes(src, chars);
		Assert::IsTrue(chars.empty());

		src = cv::Mat::zeros(100, 100, CV_8UC1);
		Algorithm::charsBBoxes(src, chars);
		Assert::IsTrue(chars.empty());

		src = cv::Mat::zeros(100, 100, CV_16FC1);
		Algorithm::charsBBoxes(src, chars);
		Assert::IsTrue(chars.empty());
	}

	TEST_METHOD(charsBBoxes_ValidInput)
	{
		cv::Mat src;
		std::vector<cv::Rect> chars;

		src = cv::Mat::zeros(100, 100, CV_8UC1);
		cv::rectangle(src, cv::Point(10, 10), cv::Point(30, 40), cv::Scalar(255), cv::FILLED);
		cv::rectangle(src, cv::Point(40, 10), cv::Point(60, 42), cv::Scalar(255), cv::FILLED);
		cv::rectangle(src, cv::Point(70, 10), cv::Point(90, 38), cv::Scalar(255), cv::FILLED);
		cv::rectangle(src, cv::Point(10, 50), cv::Point(30, 80), cv::Scalar(255), cv::FILLED);
		cv::rectangle(src, cv::Point(40, 50), cv::Point(60, 78), cv::Scalar(255), cv::FILLED);
		cv::rectangle(src, cv::Point(70, 50), cv::Point(90, 79), cv::Scalar(255), cv::FILLED);

		Algorithm::charsBBoxes(src, chars);
		Assert::IsTrue(chars.size() == 6);
	}

	TEST_METHOD(firstIndexes_InvalidInput)
	{
		std::vector<cv::Rect> chars;
		std::array<int, 3> indexes = { 0, 0, 0 };

		Algorithm::firstIndexes(chars, indexes);
		Assert::IsTrue(!indexes[0] && !indexes[1] && !indexes[2]);

		chars.push_back(cv::Rect(0, 0, 0, 0));
		Algorithm::firstIndexes(chars, indexes);
		Assert::IsTrue(!indexes[0] && !indexes[1] && !indexes[2]);
	}

	TEST_METHOD(firstIndexes_ValidInput)
	{
		std::vector<cv::Rect> chars;
		std::array<int, 3> indexes;

		chars.push_back(cv::Rect(0, 0, 10, 10));
		chars.push_back(cv::Rect(30, 0, 10, 10));
		chars.push_back(cv::Rect(50, 0, 10, 10));
		chars.push_back(cv::Rect(70, 0, 10, 10));
		chars.push_back(cv::Rect(90, 0, 10, 10));
		chars.push_back(cv::Rect(110, 0, 10, 10));

		Algorithm::firstIndexes(chars, indexes);
		Assert::IsTrue(indexes[0] == 0 && indexes[1] == 1 && indexes[2] == 3);
	}

	TEST_METHOD(paddingChars_InvalidInput)
	{
		std::vector<cv::Rect> src;
		std::vector<cv::Rect> dst;
		float percent;

		Algorithm::paddingChars(src, dst, percent);
		Assert::IsTrue(dst.empty());

		src.push_back(cv::Rect(0, 0, 0, 0));
		percent = -1;
		Algorithm::paddingChars(src, dst, percent);
		Assert::IsTrue(dst.empty());
	}

	TEST_METHOD(paddingChars_ValidInput)
	{
		std::vector<cv::Rect> src, dst;
		float percent = 0.1;

		src.push_back(cv::Rect(10, 5, 30, 30));
		src.push_back(cv::Rect(50, 5, 30, 30));

		Algorithm::paddingChars(src, dst, percent);
		Assert::IsTrue(dst[0] == cv::Rect(7, 2, 36, 36) && dst[1] == cv::Rect(47, 2, 36, 36));
	}

	TEST_METHOD(charsSpacing_InvalidInput)
	{
		cv::Mat src, dst;
		std::vector<cv::Rect> chars, paddedChars;

		Algorithm::charsSpacing(src, dst, chars, paddedChars);
		Assert::IsTrue(dst.empty());

		src = cv::Mat::zeros(100, 100, CV_8UC1);
		chars.clear();
		paddedChars.push_back(cv::Rect(0, 0, 0, 0));
		Algorithm::charsSpacing(src, dst, chars, paddedChars);
		Assert::IsTrue(dst.empty());

		chars.push_back(cv::Rect(0, 0, 0, 0));
		paddedChars.clear();
		Algorithm::charsSpacing(src, dst, chars, paddedChars);
		Assert::IsTrue(dst.empty());

		src = cv::Mat::zeros(100, 100, CV_16FC1);
		paddedChars.push_back(cv::Rect(0, 0, 0, 0));
		Algorithm::charsSpacing(src, dst, chars, paddedChars);
		Assert::IsTrue(dst.empty());
	}

	TEST_METHOD(charsSpacing_ValidInput)
	{
		cv::Mat src, dst, aux;
		std::vector<cv::Rect> chars, paddedChars;

		src = cv::Mat::zeros(100, 100, CV_8UC1);

		cv::rectangle(src, cv::Point(0, 10), cv::Point(20, 30), cv::Scalar(255), cv::FILLED);
		cv::rectangle(src, cv::Point(30, 10), cv::Point(50, 30), cv::Scalar(255), cv::FILLED);
		cv::rectangle(src, cv::Point(60, 10), cv::Point(80, 30), cv::Scalar(255), cv::FILLED);
		aux = cv::Mat::zeros(42, 124, CV_8UC1);
		cv::rectangle(aux, cv::Point(10, 10), cv::Point(30, 30), cv::Scalar(255), cv::FILLED);
		cv::rectangle(aux, cv::Point(51, 10), cv::Point(71, 30), cv::Scalar(255), cv::FILLED);
		cv::rectangle(aux, cv::Point(92, 10), cv::Point(112, 30), cv::Scalar(255), cv::FILLED);

		Algorithm::charsBBoxes(src, chars);
		Algorithm::paddingChars(chars, paddedChars, 0.5);
		Algorithm::charsSpacing(src, dst, chars, paddedChars);
		float error = RMS(dst, aux);
		Assert::IsTrue(error < 0.01);
	}
	TEST_METHOD(wordsSeparation_InvalidInput)
	{
		std::vector<cv::Rect> chars;
		std::array<std::vector<cv::Rect>, 3> words;
		std::array<int, 3> indexes;

#ifdef _DEBUG
		Algorithm::wordsSeparation(chars, words, indexes, cv::Mat());
#else
		Algorithm::wordsSeparation(chars, words, indexes);
#endif
		Assert::IsTrue(words[0].empty() && words[1].empty() && words[2].empty());

		chars.push_back(cv::Rect(0, 0, 0, 0));
#ifdef _DEBUG
		Algorithm::wordsSeparation(chars, words, indexes, cv::Mat());
#else
		Algorithm::wordsSeparation(chars, words, indexes);
#endif
		Assert::IsTrue(words[0].empty() && words[1].empty() && words[2].empty());
	}

	TEST_METHOD(wordsSeparation_ValidInput)
	{
		std::vector<cv::Rect> chars;
		std::array<std::vector<cv::Rect>, 3> words;
		std::array<int, 3> indexes;

		indexes[0] = 0;
		indexes[1] = 1;
		indexes[2] = 3;
		chars.push_back(cv::Rect(0, 0, 10, 10));
		chars.push_back(cv::Rect(20, 0, 10, 10));
		chars.push_back(cv::Rect(40, 0, 10, 10));
		chars.push_back(cv::Rect(60, 0, 10, 10));
		chars.push_back(cv::Rect(80, 0, 10, 10));
		chars.push_back(cv::Rect(100, 0, 10, 10));

#ifdef _DEBUG
		Algorithm::wordsSeparation(chars, words, indexes, cv::Mat());
#else
		Algorithm::wordsSeparation(chars, words, indexes);
#endif
		Assert::IsTrue(words[0].size() == 1 && words[1].size() == 2 && words[2].size() == 3);
	}

	TEST_METHOD(verifyOutputText_InvalidInput)
	{
		tesseract::TessBaseAPI tess;
		tess.Init(NULL, "DIN1451Mittelschrift", tesseract::OEM_LSTM_ONLY);
		float confidence = 0;

		Algorithm::verifyOutputText(tess, confidence);
		Assert::AreEqual(0.0f, confidence);

		cv::Mat image = cv::Mat::zeros(100, 100, CV_8UC1);
		tess.SetImage(image.data, image.cols, image.rows, 1, image.cols);
		Algorithm::verifyOutputText(tess, confidence);
		Assert::AreEqual(0.0f, confidence);
	}

	TEST_METHOD(verifyOutputText_ValidInput)
	{
		tesseract::TessBaseAPI tess;
		float confidence = 0;
		cv::Mat aux;
		bool result;

		tess.Init(NULL, "DIN1451Mittelschrift", tesseract::OEM_LSTM_ONLY);
		aux = cv::Mat::zeros(100, 100, CV_8UC1);

		cv::Size textSize = cv::getTextSize("A", cv::FONT_HERSHEY_SIMPLEX, 3, 2, 0);
		cv::putText(aux, "A", cv::Point((aux.cols - textSize.width) / 2, (aux.rows + textSize.height) / 2), cv::FONT_HERSHEY_SIMPLEX, 3, cv::Scalar(255), 2, cv::LINE_AA);
		tess.SetImage(aux.data, aux.cols, aux.rows, 1, aux.step);

		result = Algorithm::verifyOutputText(tess, confidence);
		Assert::IsTrue(result && confidence > 0.95);
	}

	TEST_METHOD(resizeCharTemplate_InvalidInput)
	{
		cv::Mat src, dst;
		cv::Size size;

		Algorithm::resizeCharTemplate(src, dst, size);
		Assert::IsTrue(dst.empty());

		src = cv::Mat::zeros(100, 100, CV_8UC1);
		size = cv::Size(-1, -1);
		Algorithm::resizeCharTemplate(src, dst, size);
		Assert::IsTrue(dst.empty());

		size = cv::Size(200, 200);
		Algorithm::resizeCharTemplate(src, dst, size);
		Assert::IsTrue(dst.empty());

		size = cv::Size(100, 100);
		src = cv::Mat::zeros(100, 100, CV_16FC1);
		Algorithm::resizeCharTemplate(src, dst, size);
		Assert::IsTrue(dst.empty());
	}

	TEST_METHOD(resizeCharTemplate_ValidInput)
	{
		cv::Mat src, dst, aux;
		cv::Size size;

		src = cv::Mat::zeros(100, 100, CV_8UC1);
		size = cv::Size(200, 200);

		cv::rectangle(src, cv::Point(10, 10), cv::Point(90, 90), cv::Scalar(255), cv::FILLED);
		aux = cv::Mat::zeros(cv::Size(162, 162), CV_8UC1);
		aux = 255 - aux;
		cv::copyMakeBorder(aux, aux, 1, 1, 1, 1, cv::BORDER_CONSTANT, cv::Scalar(0));

		Algorithm::resizeCharTemplate(src, dst, size);
		float error = RMS(dst, aux);
		Assert::IsTrue(error < 0.01);
	}

	TEST_METHOD(padding_InvalidInput)
	{
		int firstSize, secondSize, firstPadding = 0, secondPadding = 0;

		firstSize = 0;
		secondSize = 100;
		Algorithm::padding(firstSize, secondSize, firstPadding, secondPadding);
		Assert::IsTrue(!firstPadding && !secondPadding);

		firstSize = 100;
		secondSize = 100;
		Algorithm::padding(firstSize, secondSize, firstPadding, secondPadding);
		Assert::IsTrue(!firstPadding && !secondPadding);

		firstSize = -1;
		secondSize = -1;
		Algorithm::padding(firstSize, secondSize, firstPadding, secondPadding);
		Assert::IsTrue(!firstPadding && !secondPadding);
	}

	TEST_METHOD(padding_ValidInput)
	{
		int firstSize, secondSize, firstPadding, secondPadding;

		firstSize = 200;
		secondSize = 100;
		Algorithm::padding(firstSize, secondSize, firstPadding, secondPadding);
		Assert::IsTrue(firstPadding == 50 && secondPadding == 50);
	}

	TEST_METHOD(matching_InvalidInput)
	{
		cv::Mat src;
		float dice = 0, percent;

		Algorithm::matching(src, dice, percent);
		Assert::IsTrue(!dice);

		src = cv::Mat::zeros(100, 100, CV_8UC1);
		percent = -1;
		Algorithm::matching(src, dice, percent);
		Assert::IsTrue(!dice);

		src = cv::Mat::zeros(100, 100, CV_16FC1);
		Algorithm::matching(src, dice);
		Assert::IsTrue(!dice);
	}

	TEST_METHOD(matching_ValidInput)
	{
		cv::Mat src;
		float dice;

		src = cv::imread(absolutePath("I.jpg"), cv::IMREAD_GRAYSCALE);
		cv::threshold(src, src, 55, 255, cv::THRESH_BINARY);
		Algorithm::matching(src, dice);
		Assert::IsTrue(dice > 0.9);
	}

	TEST_METHOD(applyTesseract_InvalidInput)
	{
		cv::Mat src;
		std::string text;
		std::vector<cv::Rect> chars, paddedChars;
		bool charType;
		float confidence = 0;

		Algorithm::applyTesseract(src, text, chars, paddedChars, charType, confidence);
		Assert::IsTrue(text.empty() && !confidence);

		paddedChars.push_back(cv::Rect(0, 0, 10, 10));
		Algorithm::applyTesseract(src, text, chars, paddedChars, charType, confidence);
		Assert::IsTrue(text.empty() && !confidence);

		chars.push_back(cv::Rect(0, 0, 1, 1));
		Algorithm::applyTesseract(src, text, chars, paddedChars, charType, confidence);
		Assert::IsTrue(text.empty() && !confidence);

		src = cv::Mat::zeros(100, 100, CV_8UC1);
		paddedChars.push_back(cv::Rect(0, 0, 1, 1));
		chars.push_back(cv::Rect(0, 0, 10, 10));
		Algorithm::applyTesseract(src, text, chars, paddedChars, charType, confidence);
		Assert::IsTrue(text.empty() && !confidence);

		src = cv::Mat::zeros(100, 100, CV_16FC1);
		Algorithm::applyTesseract(src, text, chars, paddedChars, charType, confidence);
		Assert::IsTrue(text.empty() && !confidence);
	}

	TEST_METHOD(applyTesseract_ValidInput)
	{
		cv::Mat src;
		std::string text;
		std::vector<cv::Rect> chars, paddedChars;
		bool charType;
		float confidence;

		src = cv::Mat::zeros(100, 100, CV_8UC1);

		cv::Size textSize = cv::getTextSize("A", cv::FONT_HERSHEY_SIMPLEX, 3, 2, 0);
		cv::Rect bbox(((src.cols - textSize.width) / 2), ((src.rows - textSize.height) / 2) - 1, textSize.width - 1, textSize.height + 4);
		cv::putText(src, "A", cv::Point((src.cols - textSize.width) / 2, (src.rows + textSize.height) / 2), cv::FONT_HERSHEY_SIMPLEX, 3, cv::Scalar(255), 2, cv::LINE_AA);

		chars.push_back(cv::Rect((src.cols - textSize.width) / 2, ((src.rows - textSize.height) / 2) - 1, textSize.width - 1, textSize.height + 4));
		paddedChars.push_back(cv::Rect(0, 0, src.cols - 1, src.rows - 1));
		charType = false;
		confidence = 0;
		Algorithm::applyTesseract(src, text, chars, paddedChars, charType, confidence);
		Assert::IsTrue(text == "A\n" && confidence > 0.95);
	}

	TEST_METHOD(readText_InvalidInput)
	{
		cv::Mat src;
		std::string text;
		float confidence = 0;
		std::array<std::vector<cv::Rect>, 3> words;
		std::array<std::vector<cv::Rect>, 3> paddedWords;

		Algorithm::readText(src, text, confidence, words, paddedWords);
		Assert::IsTrue(text.empty() && !confidence);

		src = cv::Mat::zeros(100, 100, CV_8UC1);
		Algorithm::readText(src, text, confidence, words, paddedWords);
		Assert::IsTrue(text.empty() && !confidence);

		src = cv::Mat::zeros(100, 100, CV_16FC1);
		Algorithm::readText(src, text, confidence, words, paddedWords);
		Assert::IsTrue(text.empty() && !confidence);
	}

	TEST_METHOD(readText_ValidInput)
	{
		cv::Mat src;
		std::string text;
		float confidence = 0;
		std::array<std::vector<cv::Rect>, 3> words;
		std::array<std::vector<cv::Rect>, 3> paddedWords;
		std::vector<cv::Rect> chars, paddedChars;
		std::array<int, 3> indexes;

		src = cv::imread(absolutePath("connected_component.jpg"), cv::IMREAD_GRAYSCALE);

		cv::threshold(src, src, 55, 255, cv::THRESH_BINARY);
		Algorithm::charsBBoxes(src, chars);
		indexes = { 0, 2, 4 };
		Algorithm::paddingChars(chars, paddedChars, 0.6);

#ifdef _DEBUG
		Algorithm::wordsSeparation(chars, words, indexes, src);
		Algorithm::wordsSeparation(paddedChars, paddedWords, indexes, src);
#else
		Algorithm::wordsSeparation(chars, words, indexes);
		Algorithm::wordsSeparation(paddedChars, paddedWords, indexes);
#endif

		Algorithm::readText(src, text, confidence, words, paddedWords);
		Assert::IsTrue(text == "CT36NLA" && confidence > 0.95);
	}

	TEST_METHOD(drawBBoxes_InvalidInput)
	{
		cv::Mat dst;
		cv::Rect roi;
		std::string time, text;
		float confidence;

		Algorithm::drawBBoxes(dst, roi, time, text, confidence);
		Assert::IsTrue(dst.empty());

		dst = cv::Mat::zeros(100, 100, CV_8UC3);
		roi = cv::Rect(0, 0, 0, 0);
		time = "";
		text = "";
		confidence = -1;
		Algorithm::drawBBoxes(dst, roi, time, text, confidence);
		Assert::IsTrue(!dst.empty());

		dst = cv::Mat::zeros(100, 100, CV_16FC1);
		Algorithm::drawBBoxes(dst, roi, time, text, confidence);
		Assert::IsTrue(!dst.empty());
	}

	TEST_METHOD(drawBBoxes_ValidInput)
	{
		cv::Mat dst, aux;
		cv::Rect roi;
		std::string time, text;
		float confidence;

		dst = cv::Mat::zeros(1000, 3000, CV_8UC3);
		roi = cv::Rect(10, 10, 80, 80);
		text = "A";
		confidence = 0.95;
		Algorithm::drawBBoxes(dst, roi, time, text, confidence);

		aux = cv::Mat::zeros(1000, 3000, CV_8UC3);
		auto now = std::chrono::system_clock::now();
		auto toTimeT = std::chrono::system_clock::to_time_t(now);
		std::stringstream ss;
		ss << std::put_time(std::localtime(&toTimeT), "%d-%m-%Y %X");
		time = ss.str();
		std::ostringstream stream;
		stream << std::fixed << std::setprecision(2) << confidence;
		std::string displayText = time + " / " + text + " / " + "Score: " + stream.str();
		cv::putText(aux, displayText, cv::Point(10, aux.rows - 30), cv::FONT_HERSHEY_SIMPLEX, 4, cv::Scalar(0, 0, 0), 18, cv::LINE_AA);
		cv::putText(aux, displayText, cv::Point(10, aux.rows - 30), cv::FONT_HERSHEY_SIMPLEX, 4, cv::Scalar(255, 255, 255), 9, cv::LINE_AA);
		cv::rectangle(aux, roi, cv::Scalar(0, 255, 0), 5);

		float error = RMS(dst, aux);
		Assert::IsTrue(error < 0.01);
	}

	TEST_METHOD(textFromImage_InvalidInput)
	{
		std::string src, dst;
		std::string text;
		size_t plate;

		text = textFromImage(src, dst);
		plate = text.find('\n');
		Assert::IsTrue(text.substr(0, plate) == "N/A");
	}

	TEST_METHOD(textFromImage_ValidInput)
	{
		std::string src, dst;
		std::string text;

		src = absolutePath("10_d1.jpg");
		text = textFromImage(src, dst);
		size_t plate = text.find('\n');
		Assert::IsTrue(text.substr(0, plate) == "CT36NLA");
	}
};