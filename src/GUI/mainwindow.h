#pragma once

#include "vehiclemanager.h"
#include "statisticswindow.h"

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
	 * @brief Handles the uploading of an image for processing.
	 * @details Opens a file dialog for the user to select an image, then loads and processes the image to extract and display vehicle information.
	 */
	void uploadImage();

	/**
	 * @brief Processes the most recently added vehicle, updating the UI accordingly.
	 * @details Determines whether the vehicle is entering or exiting based on the pressed button.
	 * Updates the vehicle's parked time if exiting and displays the vehicle's information in the appropriate list widget.
	 */
	void processLastVehicle(const QString& QRPath = "");

	/**
	 * @brief Displays the selected vehicle's image in the graphics view.
	 * @details Based on the user's selection in the list widget, finds and displays the corresponding vehicle image in the graphics scene.
	 * @param item The list widget item that was clicked, containing the vehicle's ID as user data.
	 */
	void displayImage(QListWidgetItem* item);

	void setNumberParkingLots(const QString& numberParkingLots);

	void setFee(const QString& fee);

	void search(QString text);

	void updateOccupancyStatistics(const std::vector<std::pair<std::string, std::string>>& occupancyDateTimes);

	void showStatistics();

	void setLanguage(const int& choise);

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

	void updateStatistics(const std::string& dateTime, const bool& statistics);

	/**
	 * @brief Loads vehicle data from the database file into the application.
	 * @details Reads vehicle information from a text file and stores it in the application's memory.
	 * This method parses the file to create Vehicle objects and adds them to the vehicles vector.
	 */
	void uploadDataBase();

	bool verifyCapacity();

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
	QLineEdit* parkingLotsEdit;
	QLineEdit* occupiedParkingLotsEdit;
	QLineEdit* feeEdit;
	QLineEdit* historyLogEdit;
	QComboBox* chooseLanguage;
	QImage image;
	VehicleManager& vehicleManager;
	std::string dataBasePath;
	std::string translationsPath;
	bool pressedButton;
	int numberParkingLots = 100;
	int numberOccupiedParkingLots = 0;
	int fee = 1;
};
