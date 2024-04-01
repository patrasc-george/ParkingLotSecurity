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
class IMAGEPROCESSINGUTILS_API Algorithm
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
	 * @details This function computes the histogram of the source image.
	 * It identifies the range of pixel values and the total number of pixel intensity levels.
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

	/**
	 * @brief Identifies the largest contour within a region of interest (ROI) in a binary image and draws it onto a destination image.
	 * @details This function applies a binary threshold with Otsu's method to the source image to extract the foreground.
	 * If an edges image is provided, it dilates the edges and applies a bitwise NAND operation with the thresholded image.
	 * Then, it finds all external contours in the thresholded image, identifies the largest contour, and draws it onto the destination image.
	 * @param[in] src The source image on which to perform contour detection.
	 * @param[out] dst The destination image where the largest contour is drawn.
	 * @param[out] largestContour The largest contour found in the source image.
	 * @param[in] edges (Optional) An edge image that can be used to refine the ROI by excluding certain areas from the contour detection process.
	 */
	static bool roiContour(const cv::Mat& src, cv::Mat& dst, std::vector<cv::Point>& largestContour, const cv::Mat& edges, const float& percent);

	/**
	 * @brief Calculates a line passing through a given point with a specified slope and direction.
	 * @details This function creates a line defined by a starting point and an ending point,
	 * passing through a specified 'point' with a given 'slope'.
	 * The 'direction' flag determines the orientation of the line calculation: horizontal (true) or vertical (false).
	 * If the slope is very close to zero, the function adjusts the calculation to avoid division by zero errors.
	 * @param[out] line The calculated line represented as a 4-element vector (startPoint.x, startPoint.y, endPoint.x, endPoint.y).
	 * @param[in] slope The slope of the line.
	 * @param[in] point A point through which the line passes.
	 * @param[in] direction The direction of the line: true for horizontal, false for vertical.
	 */
	static void lineThroughPoint(cv::Vec4f& line, const double& slope, const cv::Point& point, const bool& direction);

	/**
	 * @brief Compares a set of lines with a reference line to categorize them based on their spatial relation to the reference.
	 * @details This function iterates through a vector of lines, each defined by two points.
	 * For each line, it calculates a midpoint and determines the line's position relative to a reference line by calculating the cross product of the vector
	 * from the midpoint to a point on the reference line with the direction vector of the reference line.
	 * Lines are then categorized into two groups based on the sign of this cross product.
	 * @param[in] lines The set of lines to be compared with the reference line.
	 * @param[out] firstLines Lines that are on one side of the reference line.
	 * @param[out] secondLines Lines that are on the opposite side of the reference line.
	 * @param[in] referenceLine The reference line used for comparison, represented as a 4-element vector (x1, y1, x2, y2).
	 */
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

	/**
	 * @brief Sorts lines into vertical and horizontal groups, and further categorizes them based on their relative position to the image center.
	 * @details This function processes a vector of lines to classify them as either vertical or horizontal based on their slope and orientation.
	 * It calculates the average slope of horizontal lines and uses this to define a reference horizontal line through the image center.
	 * Similarly, it defines a vertical reference line through the image center.
	 * Lines are then further categorized as being above or below the horizontal reference line, and to the left or right of the vertical reference line.
	 * The function ensures that there are lines in each of the categories before proceeding to calculate initial and terminal points for each group.
	 * @param[out] sortedLines A vector to store the sorted lines, ordered as left, top, right, and bottom lines based on their relation to the image center.
	 * @param[in] lines The input vector of lines to be sorted, where each line is represented as a 4-element vector (x1, y1, x2, y2).
	 * @param[in] size The size of the image or scene from which the lines are derived, used to determine the center for categorization.
	 * @return A boolean value indicating the success of the sorting operation.
	 * Returns true if lines are successfully categorized into all four groups (left, top, right, bottom); otherwise, returns false.
	 */
	static bool lineSorting(std::vector<cv::Vec4i>& sortedLines, const std::vector<cv::Vec4i>& lines, const cv::Size& size);

	/**
	 * @brief Calculates the intersection point of two line segments.
	 * @details This function finds the point where two line segments intersect, using their linear equations derived from the segment endpoints.
	 * @param[in] line1 The first line segment represented as Vec4i.
	 * @param[in] line2 The second line segment represented as Vec4i.
	 * @return The intersection point as a Point2f.
	 */
	static cv::Point2f intersection(const cv::Vec4i& line1, const cv::Vec4i& line2);

	/**
	 * @brief Detects and calculates the corner points of a quadrilateral based on the largest contour in an image.
	 * @details This function applies erosion to the source image to highlight its edges,
	 * and then attempts to fit a rotated bounding box around the largest contour.
	 * It iteratively decreases the minimum line length parameter for HoughLinesP to find lines within the edges, sorts these lines,
	 * and calculates their intersections to determine the corner points of the quadrilateral.
	 * @param[in] src The source image from which to calculate the quadrilateral's corners.
	 * @param[out] quadrilateralCoordinates The calculated coordinates of the quadrilateral's corners.
	 * @param[in] largestContour The largest contour found in the source image, used to approximate the quadrilateral's bounding box.
	 * @return A boolean value indicating the success of the corner detection. Returns true if the corners are successfully found and false otherwise.
	 */
	static bool cornersCoordinates(const cv::Mat& src, std::vector<cv::Point2f>& quadrilateralCoordinates, const std::vector<cv::Point>& largestContour);

	/**
	 * @brief Resizes the source image based on specified points, adding padding as necessary.
	 * @details This function adjusts the dimensions of the source image to ensure that specified points fit within it,
	 * optionally allowing for a percentage-based scaling.
	 * It calculates the minimum and maximum x and y coordinates among the given points and adds padding
	 * to the source image if these points extend beyond its borders.
	 * The function also adjusts the points to account for any added padding.
	 * @param[in] src The original image to be resized.
	 * @param[out] dst The destination image after resizing and padding.
	 * @param[in,out] points A vector of points that should fit within the resized image; these are adjusted if padding is added to the image.
	 * @param[in] percent (Optional) A percentage indicating how much larger the resulting image can be compared to the original, to accommodate the points.
	 * @return A boolean value indicating whether the resizing was within the specified percentage limits. Returns true if the resizing meets the criteria, false otherwise.
	 */
	static bool resizeToPoints(const cv::Mat& src, cv::Mat& dst, std::vector<cv::Point2f>& points, const float& percent);

	/**
	 * @brief Applies a geometrical transformation to the source image based on quadrilateral coordinates, adjusting its perspective.
	 * @details This function calculates the height and width of the quadrilateral defined by the provided coordinates,
	 * then uses these to establish a new perspective.
	 * It applies a perspective transformation to the source image, mapping it to a new set of coordinates that represent a flat view of the quadrilateral.
	 * The transformation is intended to normalize the perspective of the quadrilateral in the image.
	 * @param[in] src The source image to transform.
	 * @param[out] dst The destination image after the perspective transformation.
	 * @param[in] quadrilateralCoordinates The coordinates of the quadrilateral corners in the source image, used to define the transformation.
	 * @param[in] percent (Optional) A percentage that defines the minimum size of the resulting image relative to the source, as a constraint on the transformation.
	 * @return A boolean value indicating the success of the transformation.
	 * Returns true if the transformation was applied successfully and the resulting image meets the size criteria; otherwise, it returns false.
	 */
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

	/**
	 * @brief Reduces noise in the source image by selectively keeping contours based on their height relative to the median height of all contours.
	 * @details This function identifies external contours in the source image, sorts them by area, and limits the number to a maximum of 8 based on their size.
	 * It then discards any contours whose height deviates significantly from the median height of the selected contours, based on a specified percentage.
	 * The remaining contours are used to create a mask, which is then applied to the source image to isolate these contours.
	 * The function also cleans up the resulting image by ensuring no small, unwanted contours remain.
	 * The goal is to denoise the image by focusing on the main features.
	 * @param[in] src The source image to denoise.
	 * @param[out] dst The destination image after denoising.
	 * @param[in] percent (Optional) The tolerance for height deviation from the median, expressed as a percentage.
	 * Contours with a height deviating more than this percentage from the median are discarded.
	 * @return A boolean value indicating the success of the denoising process.
	 * Returns true if the resulting image has a non-zero number of pixels (indicating successful contour isolation),
	 * and false otherwise, suggesting inadequate contour selection or excessive noise removal.
	 */
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

	/**
	 * @brief Verifies the output text from Tesseract OCR for a given image area and updates the confidence level.
	 * @details This function checks if Tesseract OCR has recognized exactly two characters in the specified area.
	 * It then updates the overall confidence level based on the OCR's confidence for these symbols.
	 * @param[in,out] tess An instance of Tesseract's TessBaseAPI, already initialized and set up for OCR.
	 * @param[in,out] confidence A reference to a float that holds the cumulative confidence level.
	 * This value is updated based on the OCR's confidence for the current text.
	 * @return Returns true if exactly two characters are recognized, false otherwise.
	 */
	static bool verifyOutputText(tesseract::TessBaseAPI& tess, float& confidence);

	/**
	 * @brief Resizes a character template image to a specified size while maintaining its aspect ratio.
	 * @details This function first resizes the source character template image to match the specified height, maintaining its aspect ratio.
	 * It then applies binary thresholding to enhance the character's outline.
	 * Finally, it extracts the character's bounding box, adjusts it slightly to ensure the character is fully encapsulated,
	 * and copies the result to the destination image.
	 * @param[in] src The source character template image.
	 * @param[out] dst The destination image where the resized and processed character template will be stored.
	 * @param[in] size The desired size, specifically the height, to which the character template should be resized.
	 * The width is determined based on the aspect ratio.
	 */
	static void resizeCharTemplate(const cv::Mat& src, cv::Mat& dst, const cv::Size& size);

	/**
	 * @brief Calculates the padding needed to make two dimensions equal, distributing the padding evenly on two sides.
	 * @details This utility function is used to calculate how much padding is needed when aligning images or regions of interest to the same size.
	 * It ensures that the added padding is distributed as evenly as possible between the two sides of a dimension (height or width).
	 * @param[in] firstSize The size of the dimension (width or height) of the first area.
	 * @param[in] secondSize The size of the dimension (width or height) of the second area.
	 * @param[out] firstPadding The padding to be added on one side of the dimension.
	 * @param[out] secondPadding The padding to be added on the opposite side of the dimension.
	 * If the total required padding is odd, secondPadding will be one unit larger than firstPadding.
	 */
	static void padding(const int& firstSize, const int& secondSize, int& firstPadding, int& secondPadding);

	/**
	 * @brief Matches a source image against a character template and calculates the Dice similarity coefficient.
	 * @details This function resizes and pads the character template image to match the source image's dimensions.
	 * It then performs a bitwise AND operation between the source and the template to find the intersection.
	 * The Dice coefficient is calculated to measure the similarity between the two images, providing a basis for character recognition.
	 * @param[in] src The source image to match against the character template.
	 * @param[out] dice A reference to a float where the calculated Dice similarity coefficient will be stored.
	 * @param[in] percent (Optional) The threshold percentage for the Dice coefficient to consider the match successful.
	 * @return Returns true if the Dice similarity coefficient is greater than the specified threshold, indicating a successful match; false otherwise.
	 */
	static bool matching(const cv::Mat& src, float& dice, const float& percent);

	/**
	 * @brief Applies Tesseract OCR to recognize text in specified regions of an image, adjusting for character types and improving confidence.
	 * @details Initializes Tesseract with specific settings for character recognition,
	 * applies OCR to specified regions, and adjusts the regions iteratively to improve text recognition.
	 * It also integrates a matching function to enhance the recognition of difficult characters, updating the overall confidence accordingly.
	 * @param[in] src The source image for text recognition.
	 * @param[out] text A reference to a string where the recognized text will be appended.
	 * @param[in] chars A vector of rectangles specifying the regions within the source image to apply OCR.
	 * @param[out] paddedChars A vector of rectangles specifying the adjusted regions after padding has been applied, aiming to improve OCR accuracy.
	 * @param[in] charType A boolean indicating the type of characters to recognize (true for digits, false for letters).
	 * @param[out] confidence A reference to a float where the cumulative confidence of recognized text will be stored.
	 * @return Returns true if the text recognition process completes successfully.
	 */
	static bool applyTesseract(const cv::Mat& src, std::string& text, const std::vector<cv::Rect>& chars, std::vector<cv::Rect>& paddedChars, const bool& charType, float& confidence);

	/**
	 * @brief Attempts to read text from specific regions in an image using Tesseract OCR, for different sets of character types.
	 * @details This function sequentially applies OCR to three different regions of the source image,
	 * each time targeting a different type of characters (letters or digits).
	 * It accumulates the recognized text and the overall confidence score.
	 * Text normalization is performed by removing newline characters,
	 * and the final confidence score is averaged across the total length of the recognized text.
	 * @param[in] src The source image to read text from.
	 * @param[out] text The recognized text accumulated from all regions.
	 * @param[out] confidence The average confidence score of the recognized text.
	 * @param[in] words An array of vectors, each containing rectangles defining the regions of interest for text recognition within the source image.
	 * @param[in] paddedWords An array of vectors corresponding to `words`, adjusted to include padding around the regions of interest to improve OCR accuracy.
	 * @return Returns true if text is successfully read from all specified regions, false if any OCR application fails.
	 */
	static bool readText(const cv::Mat& src, std::string& text, float& confidence, const std::array<std::vector<cv::Rect>, 3>& words, std::array<std::vector<cv::Rect>, 3>& paddedWords);

	/**
	 * @brief Draws bounding boxes around specified regions and annotates the image with text information and a confidence score.
	 * @details This method captures the current time and appends it to the provided text and confidence score,
	 * formatting this information for display on the destination image.
	 * It adjusts the specified region of interest (ROI) according to the image dimensions
	 * before drawing the bounding box and the annotated text.
	 * @param[out] dst The destination image where the bounding box and text are to be drawn.
	 * @param[in,out] roi A reference to the region of interest rectangle, which is adjusted according to the destination image dimensions before drawing.
	 * @param[out] time The current time, formatted as a string.
	 * @param[in] text The text to be displayed along with the time and confidence score.
	 * @param[in] confidence The confidence score associated with the text, formatted as part of the display text.
	 */
	static void drawBBoxes(cv::Mat& dst, cv::Rect& roi, std::string& time, const std::string& text, const float& confidence);

	friend std::string IMAGEPROCESSINGUTILS_API textFromImage(const cv::Mat& src, cv::Mat& dst);

	friend class AlgorithmTests;
};

/**
 * @brief Extracts and returns text from an image, processing through multiple stages including cropping, filtering, and OCR.
 * @details The function processes an input image through a series of steps
 * including conversion, resizing, Gaussian blurring, HSV conversion, binary thresholding,
 * contour detection, geometrical transformations, and OCR text recognition.
 * It applies various image processing techniques to prepare regions of interest for OCR,
 * aiming to extract readable text and corresponding confidence levels.
 * The output is annotated with the recognized text, processing time, and confidence scores.
 * @param[in] src The source image from which text is to be extracted.
 * @param[out] dst The destination image, which is a copy of the source annotated with recognized text and other relevant information.
 * @return A string containing the recognized text and the time of extraction.
 */
std::string IMAGEPROCESSINGUTILS_API textFromImage(const cv::Mat& src, cv::Mat& dst);