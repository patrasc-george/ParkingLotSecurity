#pragma once

#include "vehiclemanager.h"
#include "subscriptionmanager.h"

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

	void uploadImage();

	void showImage(QListWidgetItem* item);

	void setName(const QString& name);

	void setNumberParkingLots(const QString& numberParkingLots);

	void setFee(const QString& fee);

	void search(QString text);

	void updateOccupancyStatistics(const std::vector<std::pair<std::string, std::string>>& occupancyDateTimes);

	void showSubscriptions();

	void showStatistics();

	void setLanguage(const int& choise);

private:
	void setupUI();

	void isDevelopment();

	void setupButtons();

	void setupLayouts();

	void centerWindow();

	void updateStatistics(const std::string& dateTime, const bool& statistics);

	void uploadDataBase();

	bool verifyCapacity();

	bool checkResult(const int& result);

	void processLastVehicle(const QString& QRPath = "");

	void clearPreviousItems();

	void createNewPixmapItem();

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
	QPushButton* subscriptionsButton;
	QPushButton* statisticsButton;
	QLineEdit* nameEdit;
	QLineEdit* parkingLotsEdit;
	QLineEdit* occupiedParkingLotsEdit;
	QLineEdit* feeEdit;
	QLineEdit* historyLogEdit;
	QComboBox* chooseLanguage;
	QImage image;
	VehicleManager& vehicleManager;
	SubscriptionManager* subscriptionManager;
	std::string dataBasePath;
	std::string translationsPath;
	bool pressedButton;
	int numberParkingLots = 100;
	int numberOccupiedParkingLots = 0;
	int fee = 1;
};
