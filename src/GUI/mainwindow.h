#pragma once

#include "vehicle.h"

#include <fstream>
#include <QMainWindow>
#include <QListWidget>
#include <QPushButton>

class QGraphicsView;
class QGraphicsScene;
class QGraphicsPixmapItem;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget* parent = nullptr);
	~MainWindow();

private slots:
	void uploadImage();
	void displayImage(QListWidgetItem* item);

private:
	void setupUI();
	void uploadDataBase();
	void processUploadedVehicle();
	void uploadVehicles();
	void getVehicle();
	void clearPreviousItems();
	void createNewPixmapItem();
	void setGraphicsViewProperties();
	std::string timeParked();
	void processLastVehicle();
	void processImagesFromDirectory(const QString& directoryPath);

private:
	QGraphicsView* graphicsView;
	QGraphicsScene* scene;
	QGraphicsPixmapItem* pixmapItem;
	QListWidget* entriesListWidget;
	QListWidget* exitsListWidget;
	QPushButton* enterButton;
	QPushButton* exitButton;
	QString imagePath;
	QImage image;
	Vehicle vehicle;
	std::string displayText;
	std::vector<Vehicle> vehicles;
	std::ofstream writeFile;
};
