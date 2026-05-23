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

	static bool isQuadrilateral(const std::vector<cv::Point>& contour);

	static bool detectQRAnchors(const cv::Mat& binary, std::vector<std::vector<cv::Point>>& anchors);

	static cv::Point2f getContourCentroid(const std::vector<cv::Point>& contour);

	static void sortAnchors(std::vector<std::vector<cv::Point>>& anchors);

	static void drawAnchor(const cv::Mat& src, cv::Mat& dst, const std::vector<std::vector<cv::Point>>& contours);

	static void lineThroughPoint(cv::Vec4f& line, const double& slope, const cv::Point& point, const bool& direction);

	static void compareWithLine(const std::vector<cv::Vec4i>& lines, std::vector<cv::Vec4i>& firstLines, std::vector<cv::Vec4i>& secondLines, const cv::Vec4f& referenceLine);

	static cv::Vec4i initialTerminalPoints(std::vector<cv::Vec4i>& lines, const bool& direction);

#ifdef _DEBUG
	static bool lineSorting(std::vector<cv::Vec4i>& sortedLines, const std::vector<cv::Vec4i>& lines, const cv::Size& size, const cv::Mat& src);
#else
	static bool lineSorting(std::vector<cv::Vec4i>& sortedLines, const std::vector<cv::Vec4i>& lines, const cv::Size& size);
#endif

	static cv::Point2f intersection(const cv::Vec4i& firstLine, const cv::Vec4i& secondLine);

	static void paddingCoordinates(std::vector<cv::Point2f>& coordinates, const float& percentage = 0);

	static std::vector<cv::Point2f> rectificationCoordinates(const std::vector<std::vector<cv::Point>>& anchors, const float& percentage);

	static bool resizeToPoints(const cv::Mat& src, cv::Mat& dst, std::vector<cv::Point2f>& coordinates, const float& percentage = 0);

	static bool geometricalTransformation(const cv::Mat& src, cv::Mat& dst, const std::vector<cv::Point2f>& coordinates, const float& percentage = 0);

	static bool getMatrixFromImage(const cv::Mat& src, cv::Mat& dst, cv::dnn::Net aiModel);

	static bool getID(const cv::Mat& src, std::string& id, ZXing::Position* position = nullptr);

	static std::vector<cv::Point2f> cvtPositionToCoordinates(const ZXing::Position& position);

	static void drawBBox(const cv::Mat& src, std::vector<unsigned char>& dst, const std::vector<cv::Point2f>& coordinates, const std::string& id);

public:
	std::string decodeQR(const std::vector<unsigned char>& src, std::vector<unsigned char>& dst);

private:
	std::string aiModelPath;
};
