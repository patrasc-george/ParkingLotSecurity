#pragma once

#ifdef IMAGEPROCESSINGUTILS_EXPORTS
#define IMAGEPROCESSINGUTILS_API __declspec(dllexport)
#else
#define IMAGEPROCESSINGUTILS_API __declspec(dllimport)
#endif

#include <iostream>
#include <opencv2/opencv.hpp>

class algorithm
{
	static void getKernel(const cv::Mat& src, cv::Size& kernel, const float& percent);
	static void BGR2HSV(const cv::Mat& src, cv::Mat& dst);
	static void HSV2Binary(const cv::Mat& src, cv::Mat& dst, const uchar& threshold);
	static void getConnectedComponents(const cv::Mat& src, cv::Mat& stats, std::vector<std::pair<int, int>>& areas, const int& newSize);
	static void getRoi(const cv::Mat& stats, const int& label, cv::Rect& roi);
	static bool sizeBBox(const cv::Mat& src, const cv::Rect& roi, const float& min, const float& max);
	static bool heightBBox(const cv::Rect& roi, const float& min, const float& max);
	static void getLargestContour(const std::vector<std::vector<cv::Point>>& contours, std::vector<cv::Point>& largestContour);
	static int insideContour(const cv::Mat& src, cv::Mat& dst, cv::Mat& regionContour, std::vector<cv::Point>& largestContour);
	static int getContourHeight(const std::vector<cv::Point>& contour);
	static int medianHeight(const std::vector<std::vector<cv::Point>>& contours);
	static bool denoise(const cv::Mat& src, cv::Mat& dst, const float& percent);
	static void compareWithCentroid(const std::vector<cv::Vec4i>& lines, std::vector<cv::Vec4i>& firstLines, std::vector<cv::Vec4i>& secondLines, const cv::Vec2i& centroid, const bool& direction);
	static cv::Vec4i initialTerminalPoints(const std::vector<cv::Vec4i>& lines, const bool& direction);
	static void lineSorting(const cv::Mat& src, const std::vector<cv::Vec4i>& lines, std::vector<cv::Vec4i>& sortedLines);
	static cv::Point2f intersection(const cv::Vec4i& line1, const cv::Vec4i& line2);
	static void geometricTransformation(const cv::Mat& src, cv::Mat& dst, const cv::Mat& regionContour, const std::vector<cv::Point>& largestContour);
	static bool readText(const cv::Mat& src, std::string& text);
	static void drawBBoxes(cv::Mat& dst, std::vector<cv::Rect> roiConnectedComponents);
	friend std::string IMAGEPROCESSINGUTILS_API textFromImage(const cv::Mat& src, cv::Mat& dst);
};

std::string IMAGEPROCESSINGUTILS_API textFromImage(const cv::Mat& src, cv::Mat& dst);