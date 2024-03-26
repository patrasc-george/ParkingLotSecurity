#pragma once

#ifdef IMAGEPROCESSINGUTILS_EXPORTS
#define IMAGEPROCESSINGUTILS_API __declspec(dllexport)
#else
#define IMAGEPROCESSINGUTILS_API __declspec(dllimport)
#endif

#include <iostream>
#include <opencv2/opencv.hpp>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

/**
* @class algorithm
* @brief Provides a collection of image processing functions.
*
* This class encapsulates a series of static functions designed for various image processing tasks,
* including color space conversion, binary thresholding, connected component analysis,
* geometric transformations, and text extraction using OCR. It aims to provide a toolkit
* for preprocessing images, detecting and extracting features, and performing post-processing
* operations such as drawing bounding boxes around detected regions.
*/
class algorithm
{
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
	static void getRoi(const cv::Mat& stats, cv::Rect& roi, const int& label);

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
	 * @brief Adds padding to a rectangle and optionally enforces a square shape.
	 * @details This function calculates padding for a given rectangle based on a specified percentage of its dimensions.
	 * It ensures a minimum padding of 3 units if the calculated padding is less than that.
	 * The function can also adjust the padded rectangle to maintain a square shape based on the longest side.
	 * Additionally, it ensures the padded rectangle fits within an optional boundary size, adjusting its position and size if necessary.
	 * @param src The source rectangle to which padding will be applied.
	 * @param dst The destination rectangle with applied padding. It may be adjusted to maintain a square shape or fit within specified bounds.
	 * @param percent The percentage of the rectangle's dimensions to calculate the padding thickness. Default is 0.
	 * @param square A boolean flag indicating whether to enforce a square shape for the destination rectangle. Default is false.
	 * @param size An optional cv::Size representing boundary dimensions within which the destination rectangle must fit. Default is cv::Size(), implying no bounds.
	 */
	static void paddingRect(const cv::Rect& src, cv::Rect& dst, const float& percent, const bool& square, const cv::Size& size);

	/**
	 * @brief Converts pixels within a specific blue range to black in the source image.
	 * @details This function clones the source image to the destination image and iterates over all pixels,
	 * turning those within a specified blue color range to black. This can be used to highlight certain features in the image.
	 * @param[in] src The source image to be processed.
	 * @param[out] dst The destination image with specific blue pixels turned to black.
	 */
	static void blueToBlack(const cv::Mat& src, cv::Mat& dst);

	/**
	 * @brief Converts an image from HSV color space to BGR color space.
	 * @details This function iterates over each pixel of the source HSV image,
	 * converts it back to BGR color space using the HSV to RGB color conversion formula, and stores the result in the destination image.
	 * @param[in] src The source image in HSV color space.
	 * @param[out] dst The destination image in BGR color space.
	 */
	static void HSV2BGR(const cv::Mat& src, cv::Mat& dst);

	/**
	 * @brief Calculates the histogram of a given image.
	 * @details This function computes the histogram of the source image. It identifies the range of pixel values and the total number of pixel intensity levels.
	 * @param[in] src The source image for which the histogram is to be calculated.
	 * @param[out] hist The output histogram, represented as a cv::Mat.
	 */
	static void histogram(const cv::Mat& src, cv::Mat& hist);

	/**
	 * @brief Calculates the cumulative histogram of a given histogram.
	 * @details This function computes the cumulative histogram by summing up the pixel counts from the input histogram.
	 * The cumulative value at each bin represents the total count of pixels with intensity values up to that bin.
	 * @param[in] hist The input histogram.
	 * @param[out] cumulvativeHist The cumulative histogram, stored as a cv::Mat.
	 */
	static void cumulativeHistogram(const cv::Mat& hist, cv::Mat& cumulvativeHist);

	/**
	 * @brief Calculates a line from the minimum to maximum value of the cumulative histogram.
	 * @details This function identifies the minimum and maximum values in the cumulative histogram and constructs a line connecting these two points.
	 * @param[in] cumulvativeHist The cumulative histogram.
	 * @param[out] line The calculated line, represented as a cv::Vec4f.
	 */
	static void histogramLine(const cv::Mat& cumulvativeHist, cv::Vec4f& line);

	/**
	 * @brief Calculates the distance of a point from a line.
	 * @details This function computes the perpendicular distance of a given point from a specified line, using the line equation derived from the line's endpoints.
	 * @param[in] x The x-coordinate of the point.
	 * @param[in] y The y-coordinate of the point.
	 * @param[in] line The line, represented as a cv::Vec4f.
	 * @return The calculated distance as a float.
	 */
	static float distance(const float& x, const float& y, const cv::Vec4f& line);

	/**
	 * @brief Applies triangle thresholding to an image.
	 * @details This function calculates an optimal threshold based on the shape of the image histogram and uses it to convert the image to binary form.
	 * @param[in] src The source image to threshold.
	 * @param[out] dst The destination binary image.
	 */
	static void triangleThresholding(const cv::Mat& src, cv::Mat& dst);

	/**
	 * @brief Applies Sobel edge detection and thresholding to an image.
	 * @details This function detects edges using the Sobel operator, calculates the magnitude and direction of gradients,
	 * and applies triangle thresholding to the gradient magnitude.
	 * @param[in] src The source image.
	 * @param[out] dst The binary image after applying Sobel edge detection and thresholding.
	 * @param[out] direction The gradient direction of each pixel.
	 */
	static void binarySobel(const cv::Mat& src, cv::Mat& dst, cv::Mat& direction);

	/**
	 * @brief Applies non-maximum suppression to an edge image.
	 * @details This function thins out the edges in the input image by retaining only the maximum edge responses along the direction of the gradient,
	 * effectively suppressing all the other non-maximal values.
	 * @param[in] src The source edge image.
	 * @param[out] dst The image after applying non-maximum suppression.
	 * @param[in] direction The gradient direction of each pixel.
	 */
	static void nonMaximumSuppression(const cv::Mat& src, cv::Mat& dst, const cv::Mat& direction);

	/**
	 * @brief Calculates the morphological gradient of an image.
	 * @details This function applies a morphological gradient operation to highlight the edges and fine details in the image.
	 * It combines the results with Sobel edges and applies non-maximum suppression to refine the edge map.
	 * @param[in] src The source image.
	 * @param[out] dst The image after applying the morphological gradient and refining the edges.
	 */
	static void edgeDetection(const cv::Mat& src, cv::Mat& dst);

	/**
	 * @brief Performs a bitwise NAND operation between two images.
	 * @details This function applies a bitwise NAND operation, effectively inverting a bitwise AND operation,
	 * between the source image and an edges mask, setting pixels to zero where both input images have white pixels.
	 * @param[in,out] src The source image to be modified.
	 * @param[in] edges The edges image used for the NAND operation.
	 */
	static void bitwiseNand(cv::Mat& src, const cv::Mat& edges);

	/**
	 * @brief Finds and returns the largest contour from a vector of contours based on contour area.
	 * @details This function iterates over a given set of contours, calculates the area for each, and identifies the largest contour.
	 * @param[in] contours A vector containing multiple contours.
	 * @param[out] largestContour The contour with the largest area.
	 */
	static void getLargestContour(const std::vector<std::vector<cv::Point>>& contours, std::vector<cv::Point>& largestContour);

	static void roiContour(const cv::Mat& src, cv::Mat& dst, std::vector<cv::Point>& largestContour, const cv::Mat& edges);

	static void lineThroughPoint(cv::Vec4f& line, const double& slope, const cv::Point& point, const bool& direction);

	static void compareWithLine(const std::vector<cv::Vec4i>& lines, std::vector<cv::Vec4i>& firstLines, std::vector<cv::Vec4i>& secondLines, const cv::Vec4f& referenceLine);

	/**
	 * @brief Determines the initial and terminal points of a line segment representing the combined span of multiple lines.
	 * @details This function processes a collection of lines,
	 * identifying the furthest start and end points to represent them as a single extended line segment.
	 * @param[in] lines A vector of line segments represented as Vec4i (start and end points).
	 * @param[in] direction The index indicating whether to process the lines based on their x (0) or y (1) coordinates.
	 * @return A Vec4i containing the start and end points of the extended line segment.
	 */
	static cv::Vec4i initialTerminalPoints(std::vector<cv::Vec4i>& lines, const bool& direction);

	static bool lineSorting(std::vector<cv::Vec4i>& sortedLines, const std::vector<cv::Vec4i>& lines, const cv::Size& size, cv::Mat& debug);

	/**
	 * @brief Calculates the intersection point of two line segments.
	 * @details This function finds the point where two line segments intersect, using their linear equations derived from the segment endpoints.
	 * @param[in] line1 The first line segment represented as Vec4i.
	 * @param[in] line2 The second line segment represented as Vec4i.
	 * @return The intersection point as a Point2f.
	 */
	static cv::Point2f intersection(const cv::Vec4i& line1, const cv::Vec4i& line2);

	static bool cornersCoordinates(const cv::Mat& src, std::vector<cv::Point2f>& quadrilateralCoordinates, const std::vector<cv::Point>& largestContour, cv::Mat& debug);

	static bool resizeToPoints(const cv::Mat& src, cv::Mat& dst, std::vector<cv::Point2f>& points, const float& percent);

	static bool geometricalTransformation(const cv::Mat& src, cv::Mat& dst, const std::vector<cv::Point2f>& quadrilateralCoordinates, const float& percent);

	/**
	 * @brief Determines if the specified region within a contour is non-empty.
	 * @details Applies binary thresholding to identify and isolate regions within a specified contour, useful for filtering and analysis.
	 * @param src The source image.
	 * @param dst The destination binary image showing the region within the contour.
	 * @param regionContour The contour defining the region of interest.
	 * @return Boolean indicating whether the region within the contour is non-empty.
	 */
	static bool insideContour(const cv::Mat& src, cv::Mat& dst, const cv::Mat& regionContour);

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

	static bool denoise(const cv::Mat& src, cv::Mat& dst, const float& percent);

	/**
	 * @brief Extracts bounding boxes for characters from the source image.
	 * @details This function applies connected component analysis to the source image to find individual characters.
	 * It stores the bounding boxes of these characters in the provided vector.
	 * @param src The source binary image from which characters are to be extracted.
	 * @param chars Output vector storing the bounding boxes of detected characters.
	 */
	static void charsBBoxes(const cv::Mat& src, std::vector<cv::Rect>& chars);

	/**
	 * @brief Identifies critical indexes in the character bounding boxes vector to aid in word separation.
	 * @details Finds the first index of characters and the index where the largest spacing between characters occurs,
	 * indicating possible word boundaries.
	 * @param chars Vector of character bounding boxes.
	 * @param indexes Output array storing critical indexes for word separation.
	 */
	static bool firstIndexes(const std::vector<cv::Rect>& chars, std::array<int, 3>& firstIndexes);

	/**
	 * @brief Applies padding to character bounding boxes and adjusts positions to maintain layout.
	 * @details Enlarges character bounding boxes based on a percentage and ensures they do not overlap by adjusting their positions accordingly.
	 * @param src Vector of original character bounding boxes.
	 * @param dst Vector of padded and adjusted character bounding boxes.
	 * @param percent Percentage of the original size to calculate padding.
	 */
	static void paddingChars(const std::vector<cv::Rect>& src, std::vector<cv::Rect>& dst, const float& percent);

	/**
	 * @brief Creates a single matrix with characters spaced according to their padded bounding boxes.
	 * @details Arranges character matrices in a single line with appropriate spacing derived from their padded bounding boxes.
	 * This can be useful for visualization or OCR processing.
	 * @param src The source image from which characters were extracted.
	 * @param dst Output image with characters arranged and spaced according to `paddedChars`.
	 * @param chars Vector of character bounding boxes.
	 * @param paddedChars Vector of padded character bounding boxes used for spacing.
	 */
	static void charsSpacing(const cv::Mat& src, cv::Mat& dst, std::vector<cv::Rect>& chars, std::vector<cv::Rect>& paddedChars);

	/**
	 * @brief Separates characters into words based on spacing analysis.
	 * @details Uses the indexes identified by spacing analysis to split the character bounding boxes into separate vectors, each representing a word.
	 * @param chars Vector of all character bounding boxes.
	 * @param words Output array of vectors, each storing character bounding boxes for a separate word.
	 * @param indexes Array of indexes used to determine word boundaries.
	 */
	static void wordsSeparation(const std::vector<cv::Rect>& chars, std::array<std::vector<cv::Rect>, 3>& words, const std::array<int, 3>& indexes);

	static bool verifyOutputText(tesseract::TessBaseAPI& tess, float& confidence);

	static void resizeCharTemplate(const cv::Mat& src, cv::Mat& dst, const cv::Size& size);

	static void padding(const int& firstSize, const int& secondSize, int& firstPadding, int& secondPadding);

	static bool matching(const cv::Mat& src, float& dice, const float& percent);

	static bool applyTesseract(const cv::Mat& src, std::string& text, const std::vector<cv::Rect>& chars, std::vector<cv::Rect>& paddedChars, const bool& charType, float& confidence);

	static bool readText(const cv::Mat& src, std::string& text, float& confidence, const std::array<std::vector<cv::Rect>, 3>& words, std::array<std::vector<cv::Rect>, 3>& paddedWords);

	static void drawBBoxes(cv::Mat& dst, cv::Rect& roi, std::string& time, const std::string& text, const float& confidence);

	friend std::string IMAGEPROCESSINGUTILS_API textFromImage(const cv::Mat& src, cv::Mat& dst, int j);
};

std::string IMAGEPROCESSINGUTILS_API textFromImage(const cv::Mat& src, cv::Mat& dst, int j);