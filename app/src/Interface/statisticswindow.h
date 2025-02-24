#pragma once

#include <vector>
#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>

/**
 * @class StatisticsWindow
 * @brief Displays statistics related to parking occupancy, entries, and exits in a table format.
 *
 * Derived from `QWidget`, this class presents a window containing a table that shows parking statistics.
 * The table can display data for occupancy, entries, or exits based on the user's selection from a ComboBox.
 * The window is equipped with a button for closing the window and dynamically updates the table with normalized statistics,
 * applying a color scheme based on the data. The constructor takes occupancy, entries, and exit data as inputs,
 * and the class also handles the normalization and display of these statistics in an easy-to-understand format.
 */
class StatisticsWindow : public QWidget
{
	Q_OBJECT

public:
	/**
	 * @brief Constructor for the StatisticsWindow class.
	 * @details Initializes the StatisticsWindow with occupancy, entries, and exit statistics, and sets up the layout and widgets.
	 *          The window title, size, and flags are set, and the table is populated with data from the specified statistics.
	 *          Combobox and button actions are also connected to their respective slots for updating the table and closing the window.
	 * @param[in] occupancyStatistics A 2D vector containing the occupancy data.
	 * @param[in] entriesStatistics A 2D vector containing the entries data.
	 * @param[in] exitStatistics A 2D vector containing the exits data.
	 * @param[in] buttonSize The size of the buttons in the window.
	 * @param[in] parent A pointer to the parent widget (default is nullptr).
	 */
	StatisticsWindow(const std::vector<std::vector<int>>& occupancyStatistics, const std::vector<std::vector<int>>& entriesStatistics, const std::vector<std::vector<int>>& exitStatistics, const QSize& buttonSize, QWidget* parent = nullptr);

private slots:
	/**
	 * @brief Updates the table based on the selected statistics type (Occupancy, Entries, or Exits).
	 * @details This function loads the appropriate statistics based on the user's selection from a dropdown (ComboBox).
	 *          The table is then updated with the new data, and the corresponding color scheme is applied.
	 * @param[in] choise The index of the selected item in the ComboBox, where 0 represents Occupancy, 1 represents Entries, and 2 represents Exits.
	 * @return void
	 */
	void setTable(const int& choice);

private:
	/**
	 * @brief Normalizes the provided statistics.
	 * @details This function finds the maximum value in the provided statistics and scales all values between 0 and 1 by dividing them by the maximum value.
	 *          The normalized statistics are stored in the reference parameter `normalizedStatistics`.
	 * @param[in] statistics The input 2D vector containing the statistics to normalize.
	 * @param[out] normalizedStatistics A reference to the 2D vector where the normalized statistics will be stored.
	 * @return void
	 */
	void normalize(const std::vector<std::vector<int>>& data, std::vector<std::vector<double>>& normalizedData);

	/**
	 * @brief Sets up the table with days and hours as headers.
	 * @details This function configures the appearance of the table, including its row and column count, header visibility, column widths, and item alignment.
	 *          It also fills the first row and first column with days and hours, respectively.
	 * @param[in] table The QTableWidget that will be configured and populated with day and hour labels.
	 * @return void
	 */
	void setupTable(QTableWidget* table);

	/**
	 * @brief Applies a color scheme to the table based on the normalized statistics.
	 * @details The function colors each cell in the table according to the normalized statistics by mapping the normalized value to a color.
	 *          The values are displayed in the table cells with a color gradient, where higher values are shown in blue and lower values in white.
	 * @param[in] table The QTableWidget that will have its cells colored.
	 * @param[in] normalizedStatistics The normalized statistics used to calculate the color for each cell.
	 * @return void
	 */
	void applyColor(QTableWidget* table, const std::vector<std::vector<double>>& normalizedData);

private:
	QVBoxLayout* layout;
	QTableWidget* table;
	std::vector<std::vector<int>> occupancyStatistics;
	std::vector<std::vector<int>> entriesStatistics;
	std::vector<std::vector<int>> exitStatistics;
};
