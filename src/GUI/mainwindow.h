#pragma once

#include "vehicle.h"
#include "statisticswindow.h"

#include <fstream>
#include <QMainWindow>
#include <QListWidget>
#include <QPushButton>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QLineEdit>

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

	/**
	 * @brief Destroys the MainWindow object.
	 * @details Ensures that resources are released properly, including closing any open file streams.
	 */
	~MainWindow();

private slots:
	/**
	 * @brief Handles the uploading of an image for processing.
	 * @details Opens a file dialog for the user to select an image, then loads and processes the image to extract and display vehicle information.
	 */
	void uploadImage();

	/**
	 * @brief Displays the selected vehicle's image in the graphics view.
	 * @details Based on the user's selection in the list widget, finds and displays the corresponding vehicle image in the graphics scene.
	 * @param item The list widget item that was clicked, containing the vehicle's ID as user data.
	 */
	void displayImage(QListWidgetItem* item);

	void setNumberParkingLots(const QString& numberParkingLots);

	void setFee(const QString& fee);

	void search(const QString& text);

	void increaseOccupancyStatistics(const std::string& startTime, const std::string& entTime);

	void calculateOccupancyStatistics();

	void showStatistics();

private:
	/**
	 * @brief Sets up the user interface for the main window.
	 * @details Initializes and arranges all UI components including graphics view, buttons, and list widgets.
	 * Also, sets the main layout for the application window.
	 */
	void setupUI();

	/**
	 * @brief Sets the database path based on directory existence.
	 * @details Checks if "../../../database" exists and sets the database path accordingly; otherwise uses "database/".
	 */
	void isDevelopment();

	void setupButtons();

	void setupLayouts();

	void centerWindow();

	/**
	 * @brief Loads vehicle data from the database file into the application.
	 * @details Reads vehicle information from a text file and stores it in the application's memory.
	 * This method parses the file to create Vehicle objects and adds them to the vehicles vector.
	 */
	void uploadDataBase();

	/**
	 * @brief Processes and displays the information of an uploaded vehicle.
	 * @details Adds the uploaded vehicle's information to the appropriate list widget (entries or exits)
	 * based on whether the vehicle has a parked time recorded.
	 */
	void processUploadedVehicle();

	/**
	 * @brief Processes each uploaded vehicle stored in memory.
	 * @details Iterates through the vector of vehicles loaded from the database and calls processUploadedVehicle for each one to display their information in the UI.
	 */
	void uploadVehicles();

	void setNumberOccupiedParkingLots();

	bool verifyCapacity();

	bool getVehicle();

	/**
	 * @brief Clears all items from the graphics scene.
	 * @details Removes and deletes all graphics items from the scene to prepare for displaying a new image.
	 */
	void clearPreviousItems();

	/**
	 * @brief Creates and adds a new pixmap item to the graphics scene.
	 * @details Instantiates a QGraphicsPixmapItem with the current image and adds it to the scene for display.
	 */
	void createNewPixmapItem();

	/**
	 * @brief Adjusts the properties of the graphics view to fit the displayed image.
	 * @details Sets the alignment and scaling of the graphics view to ensure the image is correctly displayed within the UI.
	 */
	void setGraphicsViewProperties();

	Vehicle findVehicle(const bool& direction = true, int index = -1);

	/**
	 * @brief Calculates the time a vehicle has been parked.
	 * @details Compares the current vehicle's entry or exit time with previous records to calculate the duration of parking.
	 * Formats and returns this duration as a string.
	 * @return A string representing the parked duration in the format HH:MM:SS.
	 */
	std::string timeParked();

	int calculateTotalAmount(const std::string& time);

	void updateStatistics();

	/**
	 * @brief Processes the most recently added vehicle, updating the UI accordingly.
	 * @details Determines whether the vehicle is entering or exiting based on the pressed button.
	 * Updates the vehicle's parked time if exiting and displays the vehicle's information in the appropriate list widget.
	 */
	void processLastVehicle();

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
	QPushButton* pressedButton;
	QLineEdit* parkingLotsEdit;
	QLineEdit* occupiedParkingLotsEdit;
	QLineEdit* feeEdit;
	QLineEdit* historyLogEdit;
	QString imagePath;
	QImage image;
	Vehicle curentVehicle;
	std::string databasePath;
	std::string displayText;
	std::vector<Vehicle> vehicles;
	std::unordered_map<int, bool> vehiclesStatus;
	std::vector<std::vector<int>> occupancyStatistics;
	std::vector<std::vector<int>> enterStatistics;
	std::vector<std::vector<int>> exitStatistics;
	std::ofstream writeFile;
	int numberParkingLots = 100;
	int numberOccupiedParkingLots = 0;
	int fee = 1;
};
