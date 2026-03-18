#pragma once

#ifdef _WIN32
#ifdef QRCODEDETECTION_EXPORTS
#define QRCODEDETECTION_API __declspec(dllexport)
#else
#define QRCODEDETECTION_API __declspec(dllimport)
#endif
#elif __linux__
#define QRCODEDETECTION_API __attribute__((visibility("default")))
#else
#define QRCODEDETECTION_API
#endif

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include <algorithm>
#include <ZXing/Result.h> 

class QRCODEDETECTION_API QRCode
{
public:
	QRCode();

private:
	static void resize(const cv::Mat& src, cv::Mat& dst, const int& max);

	static void binarySobel(const cv::Mat& src, cv::Mat& dst, cv::Mat& direction);

	static void nonMaximumSuppression(const cv::Mat& src, cv::Mat& dst, const cv::Mat& directions);

	static void edgeDetection(const cv::Mat& src, cv::Mat& dst);

	static cv::Mat countTransitions(const cv::Mat& src, const bool& isVertical = false);

	static cv::Mat drawHistogram(const cv::Mat& histogram);

	static cv::Mat densestInterval(const cv::Mat& histogram, const float& percentage = 1.0, const float& gap = 0.0);

	static void removeRowsAndColumns(const cv::Mat& src, cv::Mat& dst, const cv::Mat& verticalHistogram, const cv::Mat& horizontalHistogram, std::vector<int>& rowMap, std::vector<int>& columnMap);

	static cv::Size getKernelSize(const cv::Size& size, const float& percentage = 0);

	static void getConnectedComponents(const cv::Mat& src, cv::Mat& stats, std::vector<std::pair<int, int>>& areas, const int& newSize);

	static cv::Rect getRoi(const cv::Mat& stats, const int& label);

	static bool ratioBBox(const cv::Rect& roi, const float& max);

	static void paddingRect(const cv::Rect& src, cv::Rect& dst, const float& percentage = 0, const cv::Size& size = cv::Size());

	static std::vector<cv::Point> getLargestContour(const std::vector<std::vector<cv::Point>>& contours);

	static void lineThroughPoint(cv::Vec4f& line, const double& slope, const cv::Point& point, const bool& direction);

	static void compareWithLine(const std::vector<cv::Vec4i>& lines, std::vector<cv::Vec4i>& firstLines, std::vector<cv::Vec4i>& secondLines, const cv::Vec4f& referenceLine);

	static cv::Vec4i initialTerminalPoints(std::vector<cv::Vec4i>& lines, const bool& direction);

#ifdef _DEBUG
	static bool lineSorting(std::vector<cv::Vec4i>& sortedLines, const std::vector<cv::Vec4i>& lines, const cv::Size& size, const cv::Mat& src);
#else
	static bool lineSorting(std::vector<cv::Vec4i>& sortedLines, const std::vector<cv::Vec4i>& lines, const cv::Size& size);
#endif

	static cv::Point2f intersection(const cv::Vec4i& firstLine, const cv::Vec4i& secondLine);

	static void paddingCoordinates(std::vector<cv::Point2f>& coordinates, const float& percentage = 0, const cv::Size& size = cv::Size());

	static std::vector<cv::Point2f> rectificationCoordinates(const cv::Mat& src, const float& percentage = 0);

	static bool resizeToPoints(const cv::Mat& src, cv::Mat& dst, std::vector<cv::Point2f>& coordinates, const float& percentage = 0);

	static bool geometricalTransformation(const cv::Mat& src, cv::Mat& dst, const std::vector<cv::Point2f>& coordinates, const float& percentage = 0);

	static bool getMatrixFromImage(const cv::Mat& src, cv::Mat& dst, cv::dnn::Net aiModel);

	static bool getID(const cv::Mat& src, std::string& id, ZXing::Position* position = nullptr);

	std::vector<cv::Point2f> cvtPositionToCoordinates(const ZXing::Position& position);

	std::vector<cv::Point2f> scaleCoordinates(const std::vector<cv::Point2f>& coordinates, const cv::Rect& roi);

	std::vector<cv::Point2f> getCoordinatesFromMap(const std::vector<cv::Point2f>& coordinates, const std::vector<int>& rowMap, const std::vector<int>& columnMap);

	void drawBBox(const cv::Mat& src, cv::Mat& dst, const std::vector<cv::Point2f>& coordinates, const std::string& id);

public:
	std::string decodeQR(const std::vector<unsigned char>& src, std::vector<unsigned char>& dst);

private:
	std::string aiModelPath;
};
