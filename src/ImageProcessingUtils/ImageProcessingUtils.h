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
	/**
	 * @brief Calculates the size of the Gaussian kernel based on the source image dimensions and a percentage.
	 * @details This function computes the dimensions of the Gaussian kernel as a percentage of the source image's dimensions.
	 * It ensures that the kernel dimensions are odd numbers greater than or equal to 3.
	 * @param[in] src The source image for which the kernel size is to be calculated.
	 * @param[out] kernel The calculated size of the kernel.
	 * @param[in] percent The percentage of the source image's dimensions to calculate the kernel size.
	 */
	static void getKernel(const cv::Mat& src, cv::Size& kernel, const float& percent);

	/**
	 * @brief Converts an image from BGR color space to HSV color space.
	 * @details This function converts each pixel of the source image from BGR color space to HSV color space manually,
	 * taking into account the maximum and minimum values of the BGR components to calculate the HSV values.
	 * @param[in] src The source image in BGR color space.
	 * @param[out] dst The destination image in HSV color space.
	 */
	static void BGR2HSV(const cv::Mat& src, cv::Mat& dst);

	/**
	 * @brief Converts an HSV image to a binary image based on a threshold.
	 * @details This function thresholds an HSV image, producing a binary image where pixels are set to white
	 * if their saturation is below the threshold and their value is above the threshold, and black otherwise.
	 * @param[in] src The source image in HSV color space.
	 * @param[out] dst The destination binary image.
	 * @param[in] threshold The threshold value used for binarization.
	 */
	static void HSV2Binary(const cv::Mat& src, cv::Mat& dst, const uchar& threshold);

	/**
	 * @brief Identifies connected components in a binary image and sorts them by area.
	 * @details This function uses the connected components analysis to label different parts of a binary image,
	 * computes the area of each component, and sorts them in descending order by area.
	 * @param[in] src The source binary image.
	 * @param[out] stats Statistics of the identified components, including the area.
	 * @param[out] areas A vector of pairs, each containing the label and area of a connected component.
	 * @param[in] newSize The maximum number of components to keep after sorting.
	 */
	static void getConnectedComponents(const cv::Mat& src, cv::Mat& stats, std::vector<std::pair<int, int>>& areas, const int& newSize);

	/**
	 * @brief Retrieves the bounding box of a connected component identified by its label.
	 * @details This function extracts the bounding box of a specific connected component
	 * from the statistics provided by the connected components analysis.
	 * @param[in] stats The statistics of connected components obtained from connected components analysis.
	 * @param[in] label The label of the connected component whose bounding box is to be retrieved.
	 * @param[out] roi The bounding box of the specified connected component.
	 */
	static void getRoi(const cv::Mat& stats, const int& label, cv::Rect& roi);

	/**
	 * @brief Checks if the size of the bounding box meets specified criteria relative to the source image size.
	 * @details This function evaluates whether the area of a bounding box as a fraction of the total image area falls within a specified range.
	 * @param[in] src The source image from which the bounding box was derived.
	 * @param[in] roi The bounding box whose size is being evaluated.
	 * @param[in] min The minimum fraction of the source image area that the bounding box area must exceed.
	 * @param[in] max The maximum fraction of the source image area that the bounding box area must not exceed.
	 * @return A boolean value indicating whether the bounding box meets the specified size criteria.
	 */
	static bool sizeBBox(const cv::Mat& src, const cv::Rect& roi, const float& min, const float& max);

	/**
	 * @brief Checks if the height of the bounding box meets specified criteria relative to its width.
	 * @details This function evaluates whether the height-to-width ratio of a bounding box falls within a specified range,
	 * useful for filtering based on expected proportions.
	 * @param[in] roi The bounding box whose proportions are being evaluated.
	 * @param[in] min The minimum allowed height-to-width ratio.
	 * @param[in] max The maximum allowed height-to-width ratio.
	 * @return A boolean value indicating whether the bounding box meets the specified proportional criteria.
	 */
	static bool heightBBox(const cv::Rect& roi, const float& min, const float& max);

	/**
	 * @brief Finds and returns the largest contour from a vector of contours based on contour area.
	 * @details This function iterates over a given set of contours, calculates the area for each, and identifies the largest contour.
	 * @param[in] contours A vector containing multiple contours.
	 * @param[out] largestContour The contour with the largest area.
	 */
	static void getLargestContour(const std::vector<std::vector<cv::Point>>& contours, std::vector<cv::Point>& largestContour);

	/**
	 * @brief Identifies and extracts the region inside the largest contour of a binary image.
	 * @details This function applies binary thresholding, finds contours, identifies the largest contour,
	 * and creates a binary mask representing the area inside it.
	 * @param[in] src The source binary image.
	 * @param[out] dst The destination image masked by the largest contour.
	 * @param[out] regionContour The binary mask of the largest contour.
	 * @param[out] largestContour The largest contour found in the source image.
	 * @return The number of non-zero pixels in the masked destination image.
	 */
	static int insideContour(const cv::Mat& src, cv::Mat& dst, cv::Mat& regionContour, std::vector<cv::Point>& largestContour);

	/**
	 * @brief Calculates the height of a given contour.
	 * @details This function computes the vertical span of a contour by finding the minimum and maximum Y coordinates among its points.
	 * @param[in] contour A contour represented as a vector of points.
	 * @return The height of the contour.
	 */
	static int getContourHeight(const std::vector<cv::Point>& contour);

	/**
	 * @brief Calculates the median height of a set of contours.
	 * @details This function determines the median height among all given contours, useful for filtering contours based on their size.
	 * @param[in] contours A vector of contours.
	 * @return The median height of the given contours.
	 */
	static int medianHeight(const std::vector<std::vector<cv::Point>>& contours);

	/**
	 * @brief Applies a denoising operation to a binary image based on contour analysis.
	 * @details This function performs contour detection, filters out contours that significantly deviate from the median height,
	 * and retains a specified number of the largest contours to denoise the image.
	 * @param[in] src The source binary image.
	 * @param[out] dst The destination image after denoising.
	 * @param[in] percent The percentage used to determine the acceptable deviation from the median height for contours.
	 * @return A boolean value indicating success of the denoising process.
	 */
	static bool denoise(const cv::Mat& src, cv::Mat& dst, const float& percent);

	/**
	 * @brief Separates lines into two groups based on their relative position to a centroid.
	 * @details This function categorizes lines based on whether they are above or
	 * below (or left/right, depending on the direction) a specified centroid, facilitating geometric analyses.
	 * @param[in] lines The lines to be categorized.
	 * @param[out] firstLines The group of lines that are on one side of the centroid.
	 * @param[out] secondLines The group of lines on the opposite side of the centroid.
	 * @param[in] centroid The point representing the centroid used for comparison.
	 * @param[in] direction The axis along which to compare the lines with the centroid (0 for x-axis, 1 for y-axis).
	 */
	static void compareWithCentroid(const std::vector<cv::Vec4i>& lines, std::vector<cv::Vec4i>& firstLines, std::vector<cv::Vec4i>& secondLines, const cv::Vec2i& centroid, const bool& direction);

	/**
	 * @brief Determines the initial and terminal points of a line segment representing the combined span of multiple lines.
	 * @details This function processes a collection of lines,
	 * identifying the furthest start and end points to represent them as a single extended line segment.
	 * @param[in] lines A vector of line segments represented as Vec4i (start and end points).
	 * @param[in] direction The index indicating whether to process the lines based on their x (0) or y (1) coordinates.
	 * @return A Vec4i containing the start and end points of the extended line segment.
	 */
	static cv::Vec4i initialTerminalPoints(std::vector<cv::Vec4i>& lines, const bool& direction);

	/**
	 * @brief Sorts line segments into vertical and horizontal groups and further categorizes them based on their position relative to the image centroid.
	 * @details This function differentiates lines based on their orientation and spatial relation to the centroid,
	 * assisting in the geometric structuring of image features.
	 * @param[in] src The source image, used for determining the centroid.
	 * @param[in] lines The unsorted line segments detected in the image.
	 * @param[out] sortedLines The result of categorizing lines as either vertical or horizontal, then sorting them based on their relation to the centroid.
	 * @return A boolean value indicating whether the sorting was successful.
	 */
	static bool lineSorting(const cv::Mat& src, const std::vector<cv::Vec4i>& lines, std::vector<cv::Vec4i>& sortedLines);

	/**
	 * @brief Calculates the intersection point of two line segments.
	 * @details This function finds the point where two line segments intersect, using their linear equations derived from the segment endpoints.
	 * @param[in] line1 The first line segment represented as Vec4i.
	 * @param[in] line2 The second line segment represented as Vec4i.
	 * @return The intersection point as a Point2f.
	 */
	static cv::Point2f intersection(const cv::Vec4i& line1, const cv::Vec4i& line2);

	/**
	 * @brief Adjusts the source image's size based on a set of points, adding padding if necessary.
	 * @details This function ensures that all points lie within the bounds of the image,
	 * potentially adjusting the image size by adding padding where needed.
	 * @param[in] src The original image.
	 * @param[out] dst The resized image with padding added as required.
	 * @param[in,out] points The points that must all be visible in the resized image. They are adjusted if padding is added.
	 */
	static void resizeToPoints(const cv::Mat& src, cv::Mat& dst, std::vector<cv::Point2f>& points);

	/**
	 * @brief Applies geometric transformations to an image based on the analysis of its largest contour.
	 * @details This function performs perspective correction and other transformations on an image segment identified by its largest contour,
	 * optimizing it for further processing.
	 * @param[in] src The source image.
	 * @param[out] dst The image after applying geometric transformations.
	 * @param[in] regionContour A binary mask of the region defined by the largest contour.
	 * @param[in] largestContour The largest contour identified in the source image.
	 * @return A boolean value indicating the success of the transformation.
	 */
	static bool geometricTransformation(const cv::Mat& src, cv::Mat& dst, const cv::Mat& regionContour, const std::vector<cv::Point>& largestContour);

	/**
	 * @brief Extracts text from an image using OCR technology.
	 * @details This function utilizes Tesseract OCR to identify and extract text from animage segment,
	 * applying preprocessing as necessary to optimize OCR accuracy.
	 * @param[in] src The source image prepared for text recognition.
	 * @param[out] text The extracted text as a string.
	 * @return A boolean value indicating whether text was successfully extracted.
	 */
	static bool readText(const cv::Mat& src, std::string& text);

	/**
	 * @brief Draws bounding boxes around specified regions on an image.
	 * @details This function visually highlights regions of interest in an image,
	 * typically used to denote areas where text or specific features were identified.
	 * @param[out] dst The image on which bounding boxes will be drawn.
	 * @param[in] roiConnectedComponents A vector of rectangles representing the regions of interest.
	 */
	static void drawBBoxes(cv::Mat& dst, std::vector<cv::Rect> roiConnectedComponents);

	friend std::string IMAGEPROCESSINGUTILS_API textFromImage(const cv::Mat& src, cv::Mat& dst);
};

/**
 * @brief Extracts text from an image using Optical Character Recognition (OCR).
 * @details This function leverages Tesseract OCR to detect and extract text from an image.
 * It preprocesses the image to improve OCR accuracy, which includes conversion to grayscale, thresholding,
 * and potentially other enhancements before passing the image to the OCR engine.
 * @param[in] src The source image from which text needs to be extracted. This should be a cv::Mat object.
 * @param[in] preprocess Flag indicating whether to preprocess the image for better OCR results. Preprocessing includes noise reduction, binarization, etc.
 * @return A std::string containing the extracted text. If no text is detected or in case of an error, the returned string will be empty.
 */
std::string IMAGEPROCESSINGUTILS_API textFromImage(const cv::Mat& src, cv::Mat& dst);