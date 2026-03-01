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

class QRCODEDETECTION_API QRCode
{
public:
	QRCode();

public:
	/**
 * @brief Generates a QR code containing vehicle information and saves it as an image.
 * @details This function generates a QR code based on the provided information such as
 *          vehicle ID, name, license plate, date/time, and parking duration. The QR code is
 *          rendered onto an image, and additional information (such as the vehicle's name,
 *          license plate, date/time, and total amount) is added as text. The final image is
 *          saved in the specified database path with a formatted filename.
 * @param[in] id The vehicle's unique identifier.
 * @param[in] name The vehicle owner's name.
 * @param[in] licensePlate The vehicle's license plate number.
 * @param[in] dataBasePath The base directory path where the QR code image will be saved.
 * @param[in] dateTime The date and time the vehicle entered the parking lot.
 * @param[in] timeParked The total time the vehicle was parked.
 * @param[in] totalAmount The total amount for parking, in RON.
 * @return void
 */
	void generateQR(const std::string& id, const std::string& name, const std::string& licensePlate, const std::string& dataBasePath, const std::string& assetsPath, const std::string& dateTime = "", const std::string& timeParked = "", const int& totalAmount = 0);

private:
	static void resize(const cv::Mat& src, cv::Mat& dst, const int& max);

	static void binarySobel(const cv::Mat& src, cv::Mat& dst, cv::Mat& direction);

	static void nonMaximumSuppression(const cv::Mat& src, cv::Mat& dst, const cv::Mat& directions);

	static void edgeDetection(const cv::Mat& src, cv::Mat& dst);

	static cv::Mat countTransitions(const cv::Mat& src, const bool& isVertical = false);

	static cv::Mat drawHistogram(const cv::Mat& histogram);

	static cv::Mat densestInterval(const cv::Mat& histogram, const float& percentage = 1.0, const float& gap = 0.0);

	static void removeRowsAndColumns(const cv::Mat& src, cv::Mat& dst, const cv::Mat& verticalHistogram, const cv::Mat& horizontalHistogram);

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

public:
	std::string decodeQR(const std::string& path);

private:
	std::string aiModelPath;
};