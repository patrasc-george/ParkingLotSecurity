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
	 * @brief Adds padding to a source rectangle within the specified bounds.
	 * @details Expands the source rectangle by a certain percentage of its size on all sides,
	 * ensuring the expanded rectangle stays within the given size limits.
	 * @param[in] src The source rectangle to be padded.
	 * @param[out] dst The destination rectangle with padding applied.
	 * @param[in] size The size limits within which the destination rectangle must fit.
	 * @param[in] percent The percentage of the original size by which to expand the rectangle on each side. Defaults to 0 if not specified.
	 */
	static void paddingRect(const cv::Rect& src, cv::Rect& dst, const cv::Size& size, const float& percent);

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
	 * @details This function computes the cumulative histogram by summing up the pixel counts from the input histogram. The cumulative value at each bin represents the total count of pixels with intensity values up to that bin.
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
	 * @brief Applies binary thresholding to an image.
	 * @details This function creates a binary image from the source by setting pixels to white if their intensity exceeds a given threshold, and to black otherwise.
	 * @param[in] src The source image to threshold.
	 * @param[out] dst The destination binary image.
	 * @param[in] threshold The intensity threshold.
	 */
	static void binaryThresholding(const cv::Mat& src, cv::Mat& dst, const int& threshold);

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

	/**
	 * @brief Identifies the largest contour in the source image and optionally applies morphological operations.
	 * @details This function applies a binary Otsu threshold to the source image to facilitate contour detection. 
	 * Optionally, if an edges image is provided, it performs a bitwise NAND operation with a dilated version of the edges to refine the result. 
	 * Additionally, a morphological opening can be applied to reduce noise.
	 * @param src The source image for contour extraction.
	 * @param dst The destination image where the largest contour is drawn. It is a binary image with the largest contour filled.
	 * @param largestContour Output vector of points representing the largest contour detected in the source image.
	 * @param edges Optional parameter providing an edge-detected version of the source image. This is used to refine the contour detection process if provided.
	 * @param opening Boolean flag indicating whether to perform a morphological opening on the thresholded image. Useful for removing small objects or noise.
	 */
	static void roiContour(const cv::Mat& src, cv::Mat& dst, std::vector<cv::Point>& largestContour, const cv::Mat& edges, const bool& opening);

	/**
	 * @brief Categorizes lines based on their position relative to a centroid.
	 * @details This function divides lines into two groups based on whether their median point falls above or below
	 * (or left/right) the centroid along the specified direction.
	 * @param lines The vector of lines to be categorized.
	 * @param firstLines Output vector for lines on one side of the centroid.
	 * @param secondLines Output vector for lines on the opposite side of the centroid.
	 * @param centroid The point representing the centroid for comparison.
	 * @param direction The axis (0 for x, 1 for y) along which to compare the lines' positions to the centroid.
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
	 * @brief Sorts lines into vertical or horizontal categories and further by their position relative to the image's centroid.
	 * @details This function sorts lines based on their orientation (vertical or horizontal)
	 * and spatial position (e.g., above or below the centroid) to facilitate further geometric analyses.
	 * @param sortedLines Output vector for the sorted lines.
	 * @param lines The vector of detected lines to sort.
	 * @param size The size of the image for calculating the centroid.
	 * @return Boolean indicating success of the sorting operation.
	 */
	static bool lineSorting(std::vector<cv::Vec4i>& sortedLines, const std::vector<cv::Vec4i>& lines, const cv::Size& size, cv::Mat src);

	/**
	 * @brief Calculates the intersection point of two line segments.
	 * @details This function finds the point where two line segments intersect, using their linear equations derived from the segment endpoints.
	 * @param[in] line1 The first line segment represented as Vec4i.
	 * @param[in] line2 The second line segment represented as Vec4i.
	 * @return The intersection point as a Point2f.
	 */
	static cv::Point2f intersection(const cv::Vec4i& line1, const cv::Vec4i& line2);

	/**
	 * @brief Calculates quadrilateral coordinates by finding intersections of sorted lines.
	 * @details Uses Hough line detection and sorting to calculate the intersection points of the detected lines,
	 * forming a quadrilateral that approximates a region of interest.
	 * @param src The source image for context in calculating line intersections.
	 * @param quadrilateralCoordinates Output vector for the coordinates of the quadrilateral.
	 * @param largestContour The largest contour, used for defining the region of interest.
	 * @return Boolean indicating whether the calculation was successful.
	 */
	static bool cornersCoordinates(const cv::Mat& src, std::vector<cv::Point2f>& quadrilateralCoordinates, const std::vector<cv::Point>& largestContour);

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
	 * @brief Applies a geometric transformation to extract a quadrilateral region from the source image.
	 * @details This function applies a perspective transformation to the source image
	 * to extract and rectify a quadrilateral region defined by the given coordinates.
	 * @param src The source image.
	 * @param dst The destination image after applying the geometric transformation.
	 * @param quadrilateralCoordinates The coordinates of the quadrilateral to be extracted.
	 */
	static bool geometricalTransformation(const cv::Mat& src, cv::Mat& dst, const std::vector<cv::Point2f>& quadrilateralCoordinates);

	/**
	 * @brief Adds padding to an image with specified padding thickness and value.
	 * @details This function applies a uniform padding around the source image. 
	 * The thickness of the padding is determined by a percentage of the image's dimensions. 
	 * A default padding of 1 pixel is applied if the percentage is 0. The function supports custom padding values for flexibility.
	 * @param src The source image to which padding will be applied.
	 * @param dst The destination image with applied padding. It is larger than the source image according to the specified padding.
	 * @param percent The percentage of the source image's dimensions to calculate the padding thickness. Default is 0, which applies a minimum padding of 1 pixel.
	 * @param value The color value used for padding. Default is cv::Scalar(), which applies black padding.
	 */
	static void paddingImage(const cv::Mat& src, cv::Mat& dst, const float& percent, const cv::Scalar& value);

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

	static void charsSeparation(const cv::Mat& src, std::vector<cv::Rect>& chars);

	static void wordSeparation(const std::vector<cv::Rect>& chars, std::vector<cv::Rect>& left, std::vector<cv::Rect>& middle, std::vector<cv::Rect>& right);

	static void getWord(const std::vector<cv::Rect>& chars, cv::Rect& word);

	static bool print(tesseract::TessBaseAPI& tess, const bool& levelValue);

	static bool applyTesseract(const cv::Mat& src, std::string& text, const std::vector<cv::Rect>& chars, const bool& charType);

	static bool readText(const cv::Mat& src, std::string& text, const std::vector<cv::Rect>& chars);

	/**
	 * @brief Draws bounding boxes around specified regions on an image.
	 * @details This function visually highlights regions of interest in an image,
	 * typically used to denote areas where text or specific features were identified.
	 * @param[out] dst The image on which bounding boxes will be drawn.
	 * @param[in] roiConnectedComponents A vector of rectangles representing the regions of interest.
	 */
	static void drawBBoxes(cv::Mat& src, std::vector<cv::Rect>& roiConnectedComponents);

	friend std::string IMAGEPROCESSINGUTILS_API textFromImage(const cv::Mat& src, cv::Mat& dst);
};

std::string IMAGEPROCESSINGUTILS_API textFromImage(const cv::Mat& src, cv::Mat& dst);