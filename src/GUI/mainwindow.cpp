#include "mainwindow.h"
#include "ImageProcessingUtils.h"

#include <QFileDialog>
#include <QVBoxLayout>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QLabel>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
	setupUI();
}

void MainWindow::setupUI()
{
	graphicsView = new QGraphicsView(this);
	scene = new QGraphicsScene(this);
	graphicsView->setScene(scene);

	pixmapItem = new QGraphicsPixmapItem();
	scene->addItem(pixmapItem);

	isDevelopment();

	enterButton = new QPushButton(this);
	std::string enterPath = databasePath + "enter.png";
	QPixmap enterPixmap(enterPath.c_str());
	enterButton->setIcon(QIcon(enterPixmap));
	enterButton->setIconSize(enterPixmap.size() / 4);
	enterButton->setFixedSize(enterPixmap.size() / 4);

	exitButton = new QPushButton(this);
	std::string exitPath = databasePath + "exit.png";
	QPixmap exitPixmap(exitPath.c_str());
	exitButton->setIcon(QIcon(exitPixmap));
	exitButton->setIconSize(exitPixmap.size() / 4);
	exitButton->setFixedSize(exitPixmap.size() / 4);

	QHBoxLayout* buttonLayout = new QHBoxLayout();
	buttonLayout->addWidget(enterButton, 0, Qt::AlignCenter);
	buttonLayout->addWidget(exitButton, 0, Qt::AlignCenter);

	setFixedSize(1300, 700);

	entriesListWidget = new QListWidget(this);
	exitsListWidget = new QListWidget(this);

	QLabel* entriesLabel = new QLabel("Entries", this);
	QLabel* exitsLabel = new QLabel("Exits", this);

	entriesLabel->setAlignment(Qt::AlignCenter);
	exitsLabel->setAlignment(Qt::AlignCenter);

	QVBoxLayout* leftLayout = new QVBoxLayout();
	leftLayout->addWidget(graphicsView);

	QVBoxLayout* entriesLayout = new QVBoxLayout();
	entriesLayout->addWidget(entriesLabel);
	entriesLayout->addWidget(entriesListWidget);

	QVBoxLayout* exitsLayout = new QVBoxLayout();
	exitsLayout->addWidget(exitsLabel);
	exitsLayout->addWidget(exitsListWidget);

	QHBoxLayout* listsLayout = new QHBoxLayout();
	listsLayout->addLayout(entriesLayout);
	listsLayout->addLayout(exitsLayout);

	QVBoxLayout* rightLayout = new QVBoxLayout();
	rightLayout->addLayout(listsLayout);
	rightLayout->addLayout(buttonLayout);

	QHBoxLayout* mainLayout = new QHBoxLayout();
	mainLayout->addLayout(leftLayout, 2);
	mainLayout->addLayout(rightLayout, 1);

	QWidget* centralWidget = new QWidget(this);
	centralWidget->setLayout(mainLayout);

	setCentralWidget(centralWidget);

	writeFile = std::ofstream(databasePath + "vehicles.txt", std::ios::app);

	connect(enterButton, &QPushButton::clicked, this, &MainWindow::uploadImage);
	connect(exitButton, &QPushButton::clicked, this, &MainWindow::uploadImage);
	connect(entriesListWidget, &QListWidget::itemClicked, this, &MainWindow::displayImage);
	connect(exitsListWidget, &QListWidget::itemClicked, this, &MainWindow::displayImage);

	uploadDataBase();
	uploadVehicles();
}


void MainWindow::isDevelopment()
{
	if (std::filesystem::exists("../../../database"))
		databasePath = "../../../database/";
	else
		databasePath = "database/";
}

void MainWindow::uploadDataBase()
{
	std::ifstream readFile(databasePath + "vehicles.txt");

	std::vector<std::string> vehicleData;
	std::string line;
	while (std::getline(readFile, line))
	{
		if (line.empty())
		{
			vehicle = Vehicle(std::stoi(vehicleData[0]), vehicleData[1], std::stoi(vehicleData[2]), vehicleData[3], vehicleData[4]);

			if (vehicleData.size() == 6)
				vehicle.setTimeParked(vehicleData[5]);

			vehicles.push_back(vehicle);
			vehicleData.clear();
		}
		else
			vehicleData.push_back(line);
	}

	readFile.close();
}

void MainWindow::processUploadedVehicle()
{
	displayText = vehicle.getLicensePlate() + "\n" + vehicle.getTime();

	if (!vehicle.getTimeParked().empty())
		displayText += '\n' + vehicle.getTimeParked();

	QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(displayText));
	item->setData(Qt::UserRole, vehicle.getId());

	if (vehicle.getTimeParked().empty())
		entriesListWidget->addItem(item);
	else
		exitsListWidget->addItem(item);
}

void MainWindow::uploadVehicles()
{
	for (const auto& uploadedVehicle : vehicles)
	{
		vehicle = uploadedVehicle;
		processUploadedVehicle();
	}
}

void MainWindow::getVehicle()
{
	cv::Mat cvImage(image.height(), image.width(), CV_8UC4, const_cast<uchar*>(image.bits()), image.bytesPerLine());
	std::string text = textFromImage(cvImage, cvImage);
	image = QImage(cvImage.data, cvImage.cols, cvImage.rows, static_cast<int>(cvImage.step), QImage::Format_RGB888).copy();

	int firstPosition = imagePath.toStdString().rfind("/");
	int lastPosition = imagePath.toStdString().find("_", firstPosition);

	int ticket = 0;
	if (firstPosition != std::string::npos && lastPosition != std::string::npos)
		ticket = imagePath.mid(firstPosition + 1, lastPosition - firstPosition - 1).toInt();

	imagePath = QString::fromStdString(databasePath) + QString::number(vehicles.size()) + ".jpg";
	size_t plate = text.find('\n');
	size_t time = text.find('\n', plate + 1);

	vehicle = Vehicle(vehicles.size(), imagePath.toStdString(), ticket, text.substr(0, plate), text.substr(plate + 1, time - plate - 1));
	vehicles.push_back(vehicle);
}

void MainWindow::clearPreviousItems()
{
	QList<QGraphicsItem*> items = scene->items();

	for (QGraphicsItem* item : items)
	{
		scene->removeItem(item);
		delete item;
	}
}

void MainWindow::createNewPixmapItem()
{
	pixmapItem = new QGraphicsPixmapItem(QPixmap::fromImage(image));
	scene->addItem(pixmapItem);
}

void MainWindow::setGraphicsViewProperties()
{
	graphicsView->setAlignment(Qt::AlignCenter);

	QRectF imageRect = pixmapItem->boundingRect();
	graphicsView->setSceneRect(imageRect);
	graphicsView->fitInView(imageRect, Qt::KeepAspectRatio);
}

std::string MainWindow::timeParked()
{
	std::vector<Vehicle>::reverse_iterator it;

	if (vehicle.getLicensePlate() != "necunoscut")
	{
		it = std::find_if(vehicles.rbegin() + 1, vehicles.rend(), [&](const Vehicle& auxVehicle) {
			return vehicle.getLicensePlate() == auxVehicle.getLicensePlate();
			});
	}
	else if (vehicle.getTicket())
	{
		it = std::find_if(vehicles.rbegin() + 1, vehicles.rend(), [&](const Vehicle& auxVehicle) {
			return vehicle.getTicket() == auxVehicle.getTicket();
			});
	}
	else
		return "00:00:00";

	std::ostringstream timeStream;
	if (it != vehicles.rend())
	{
		std::tm tmIn = {}, tmOut = {};
		std::istringstream inStr(it->getTime());
		std::istringstream outStr(vehicle.getTime());
		inStr >> std::get_time(&tmIn, "%d-%m-%Y %H:%M:%S");
		outStr >> std::get_time(&tmOut, "%d-%m-%Y %H:%M:%S");
		auto inTime = std::mktime(&tmIn);
		auto outTime = std::mktime(&tmOut);

		if (inTime != -1 && outTime != -1)
		{
			auto duration = std::difftime(outTime, inTime);

			int hours = static_cast<int>(duration) / 3600;
			int minutes = (static_cast<int>(duration) % 3600) / 60;
			int seconds = static_cast<int>(duration) % 60;

			timeStream << std::setw(2) << std::setfill('0') << hours << ":"
				<< std::setw(2) << std::setfill('0') << minutes << ":"
				<< std::setw(2) << std::setfill('0') << seconds;
		}
	}

	return timeStream.str();
}

void MainWindow::processLastVehicle()
{
	QPushButton* button = qobject_cast<QPushButton*>(sender());

	displayText = vehicle.getLicensePlate() + "\n" + vehicle.getTime();

	if (button == exitButton)
	{
		std::string time = timeParked();
		displayText += '\n' + time;
		vehicle.setTimeParked(time);
	}

	QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(displayText));
	item->setData(Qt::UserRole, vehicle.getId());

	if (button == exitButton)
		exitsListWidget->addItem(item);
	else
		entriesListWidget->addItem(item);

	image.save(imagePath);
	writeFile << vehicle;
}

void MainWindow::uploadImage()
{
	imagePath = QFileDialog::getOpenFileName(this, "Upload Image", "", "Images (*.png *.jpg *.bmp *.gif)");

	if (imagePath.isEmpty())
		return;

	image.load(imagePath);

	getVehicle();
	clearPreviousItems();
	createNewPixmapItem();
	setGraphicsViewProperties();
	processLastVehicle();
}

void MainWindow::displayImage(QListWidgetItem* item)
{
	int imageId = item->data(Qt::UserRole).toInt();
	imagePath = vehicles[imageId].getPath().c_str();
	image.load(imagePath);

	clearPreviousItems();
	createNewPixmapItem();
	setGraphicsViewProperties();
}

MainWindow::~MainWindow()
{
	writeFile.close();
}