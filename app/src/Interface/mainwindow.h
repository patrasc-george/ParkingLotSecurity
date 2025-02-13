#pragma once

#include "vehiclemanager.h"

#include <fstream>
#include <QApplication>
#include <QMainWindow>
#include <QListWidget>
#include <QPushButton>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QLineEdit>
#include <QComboBox>

/**
 * @class MainWindow
 * @brief Provides the main user interface for parking lot management.
 *
 * Derived from `QMainWindow`, this class manages vehicle entry/exit, occupancy statistics,
 * and image handling for a parking lot system. It allows users to upload images, set parking facility details
 * (e.g., name, number of spaces, fee), and view historical data and occupancy statistics.
 * It also supports language localization and connects UI actions to core functionality,
 * including parking capacity management and display updates.
 */
class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	/**
	 * @brief Constructs a new MainWindow object.
	 * @details Initializes the main window with a parent widget. Sets up the UI components necessary for the application.
	 * @param parent The parent widget of the main window, defaulting to nullptr.
	 */
	MainWindow(QWidget* parent = nullptr);

private slots:
	/**
	 * @brief Handles the image upload process.
	 * @details This function opens a file dialog to upload an image, verifies the parking lot capacity,
	 *          and processes the uploaded vehicle data (either entry or exit).
	 * @return void
	 */
	void uploadImage();

	/**
	 * @brief Displays the image associated with the selected item from the list.
	 * @details This function loads and displays the image corresponding to the selected item from the list widget.
	 * @param[in] item The selected item from the list.
	 * @return void
	 */
	void showImage(QListWidgetItem* item);

	/**
	 * @brief Sets the name of the vehicle or parking facility.
	 * @details This function updates the name of the vehicle or parking facility with the provided name.
	 * @param[in] name A string representing the name to be set.
	 * @return void
	 */
	void setName(const QString& name);

	/**
	 * @brief Sets the number of parking lots available.
	 * @details This function updates the total number of parking lots based on the provided number.
	 * @param[in] numberParkingLots A string representing the total number of parking lots.
	 * @return void
	 */
	void setNumberParkingLots(const QString& numberParkingLots);

	/**
	 * @brief Sets the fee for parking.
	 * @details This function updates the parking fee based on the provided value.
	 * @param[in] fee A string representing the parking fee to be set.
	 * @return void
	 */
	void setFee(const QString& fee);

	/**
	 * @brief Searches for vehicles or records in the history log.
	 * @details This function searches the history log for entries matching the provided text and updates the
	 *          history log list widget with the search results.
	 * @param[in] text The text to search for in the history log.
	 * @return void
	 */
	void search(QString text);

	/**
	 * @brief Updates occupancy statistics for the parking lot based on provided date and time intervals.
	 * @details This function iterates over the provided date-time intervals and updates the occupancy statistics
	 *          by incrementing the relevant statistics for each hour in the range.
	 * @param[in] occupancyDateTimes A vector of pairs representing the start and end date-time strings in the format "dd-MM-yyyy HH:mm:ss".
	 * @return void
	 */
	void updateOccupancyStatistics(const std::vector<std::pair<std::string, std::string>>& occupancyDateTimes);

	/**
	 * @brief Displays the statistics of parking lot occupancy, entrances, and exits.
	 * @details This function calculates the occupancy statistics and creates a new statistics window to display the data.
	 * @return void
	 */
	void showStatistics();

	/**
	 * @brief Sets the application's language based on user choice.
	 * @details This function loads the appropriate translation file based on the user's choice (either English or Romanian)
	 *          and applies it to the application interface. The UI is then reloaded with the new language.
	 * @param[in] choise An integer representing the user's language choice (0 for English, 1 for Romanian).
	 * @return void
	 */
	void setLanguage(const int& choise);

private:
	/**
	 * @brief Sets up the user interface for the main window.
	 * @details This function initializes the various UI components such as buttons, layouts, and graphics view,
	 *          sets up connections for signals and slots, and loads data from the database.
	 * @return void
	 */
	void setupUI();

	/**
	 * @brief Configures paths based on the build environment.
	 * @details This function sets the paths for assets and translations based on whether the application
	 *          is being built in development (_DEBUG) mode or production mode.
	 * @return void
	 */
	void isDevelopment();

	/**
	 * @brief Initializes buttons for the UI with icons and size.
	 * @details This function creates and configures the buttons used in the main window, setting their icons,
	 *          sizes, and connections to relevant slots.
	 * @return void
	 */
	void setupButtons();

	/**
	 * @brief Sets up the layouts for the main window's UI components.
	 * @details This function arranges the widgets and buttons into appropriate layouts (horizontal and vertical)
	 *          to define the structure of the main window's user interface.
	 * @return void
	 */
	void setupLayouts();

	/**
	 * @brief Centers the main window on the screen.
	 * @details This function calculates the appropriate position on the screen to place the window in the center.
	 * @return void
	 */
	void centerWindow();

	/**
	 * @brief Updates the parking statistics based on the provided date and time.
	 * @details This function updates the vehicle entrance or exit statistics, categorizing them by the day of the
	 *          week and the hour of the day based on the provided timestamp.
	 * @param[in] dateTime A string representing the date and time in the format "dd-MM-yyyy HH:mm:ss".
	 * @param[in] statistics A boolean indicating whether the event is an entry (false) or exit (true).
	 * @return void
	 */
	void updateStatistics(const std::string& dateTime, const bool& statistics);

	/**
	 * @brief Uploads and initializes data from the database.
	 * @details This function retrieves vehicle and statistics data from the database, updates statistics,
	 *          and populates the entry/exit lists with the vehicle data.
	 * @return void
	 */
	void uploadDataBase();

	/**
	 * @brief Verifies the parking lot capacity before allowing vehicle entry or exit.
	 * @details This function checks whether there is space available in the parking lot for entry or whether
	 *          there are vehicles present for exit, displaying a warning message if conditions are not met.
	 * @return A boolean value indicating whether the parking lot capacity condition is satisfied (true) or not (false).
	 */
	bool verifyCapacity();

	/**
	 * @brief Checks the result of a specific operation and takes appropriate action.
	 * @details This function evaluates the provided result and performs actions based on the value:
	 *          - If the result is 0, returns true.
	 *          - If the result is 1, shows the unpaid window.
	 *          - If the result is 2, shows the upload QR window.
	 * @param[in] result An integer representing the result of a specific operation.
	 * @return A boolean indicating the success or failure of the operation.
	 */
	bool checkResult(const int& result);

	/**
	 * @brief Processes the last vehicle entry or exit based on QR code path.
	 * @details This function processes the vehicle data (such as entry or exit) based on the provided QR code path,
	 *          updates the statistics, and modifies the parking lot occupancy. It also updates the UI with new vehicle data.
	 * @param[in] QRPath A string representing the path to the QR code image.
	 * @return void
	 */
	void processLastVehicle(const QString& QRPath = "");

	/**
	 * @brief Clears all previous items from the scene.
	 * @details This function removes all the current items from the scene and deletes them to prepare for new items.
	 * @return void
	 */
	void clearPreviousItems();

	/**
	 * @brief Creates a new pixmap item and adds it to the scene.
	 * @details This function creates a new pixmap item from the loaded image and adds it to the scene to be displayed.
	 * @return void
	 */
	void createNewPixmapItem();

	/**
	 * @brief Sets the properties of the graphics view for displaying the image.
	 * @details This function sets the alignment, scene rectangle, and adjusts the view to fit the image with aspect ratio maintained.
	 * @return void
	 */
	void setGraphicsViewProperties();

private:
	QGraphicsView* graphicsView;
	QGraphicsScene* scene;
	QGraphicsPixmapItem* pixmapItem;
	QListWidget* entriesListWidget;
	QListWidget* exitsListWidget;
	QListWidget* historyLogListWidget;
	QPushButton* enterButton;
	QPushButton* exitButton;
	QPushButton* statisticsButton;
	QLineEdit* nameEdit;
	QLineEdit* parkingLotsEdit;
	QLineEdit* occupiedParkingLotsEdit;
	QLineEdit* feeEdit;
	QLineEdit* historyLogEdit;
	QComboBox* chooseLanguage;
	QImage image;
	VehicleManager vehicleManager;
	std::string assetsPath;
	std::string translationsPath;
	bool isPassword;
	bool pressedButton;
	int numberParkingLots = 100;
	int numberOccupiedParkingLots = 0;
	int fee = 1;
};
