#include "mainwindow.h"
#include "ImageProcessingUtils.h"

#include <QFileDialog>
#include <QVBoxLayout>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QLabel>

//void MainWindow::processImagesFromDirectory(const QString& directoryPath)
//{
//	QDir dir(directoryPath);
//	QStringList filters;
//	filters << "*.jpg";
//	QStringList fileList = dir.entryList(filters, QDir::Files);
//
//	// Sortăm lista folosind un comparator personalizat care compară componentele numerice.
//	std::sort(fileList.begin(), fileList.end(), [](const QString& file1, const QString& file2) {
//		QRegularExpression re("(\\d+)");
//		QRegularExpressionMatch match1 = re.match(file1);
//		QRegularExpressionMatch match2 = re.match(file2);
//		if (match1.hasMatch() && match2.hasMatch()) {
//			int num1 = match1.captured(1).toInt();
//			int num2 = match2.captured(1).toInt();
//			return num1 < num2;
//		}
//		return file1 < file2; // În caz că nu sunt numere, sortăm alfabetic.
//		});
//
//	foreach(const QString & fileName, fileList) {
//		imagePath = dir.absoluteFilePath(fileName);
//
//		QFileInfo checkFile(imagePath);
//		if (checkFile.exists() && checkFile.isFile()) {
//			if (image.load(imagePath)) {
//				getVehicle();
//				clearPreviousItems();
//				createNewPixmapItem();
//				setGraphicsViewProperties();
//				processLastVehicle();
//			}
//		}
//	}
//}

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

	enterButton = new QPushButton("ENTER", this);
	exitButton = new QPushButton("EXIT", this);

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

	QHBoxLayout* mainLayout = new QHBoxLayout();
	mainLayout->addLayout(leftLayout, 2);
	mainLayout->addLayout(rightLayout, 1);

	QHBoxLayout* buttonsLayout = new QHBoxLayout();
	buttonsLayout->addWidget(enterButton, 0, Qt::AlignRight);
	buttonsLayout->addWidget(exitButton, 0, Qt::AlignLeft);

	QVBoxLayout* outerLayout = new QVBoxLayout();
	outerLayout->addLayout(mainLayout);
	outerLayout->addLayout(buttonsLayout, 0);

	QWidget* centralWidget = new QWidget(this);
	centralWidget->setLayout(outerLayout);

	setCentralWidget(centralWidget);

	writeFile = std::ofstream("../../../database/vehicles.txt", std::ios::app);

	connect(enterButton, &QPushButton::clicked, this, &MainWindow::uploadImage);
	connect(exitButton, &QPushButton::clicked, this, &MainWindow::uploadImage);
	connect(entriesListWidget, &QListWidget::itemClicked, this, &MainWindow::displayImage);
	connect(exitsListWidget, &QListWidget::itemClicked, this, &MainWindow::displayImage);

	uploadDataBase();
	uploadVehicles();
}

void MainWindow::uploadDataBase()
{
	std::ifstream readFile("../../../database/vehicles.txt");

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
	//Algorithm::BGR2HSV(cvImage, cvImage);
	image = QImage(cvImage.data, cvImage.cols, cvImage.rows, static_cast<int>(cvImage.step), QImage::Format_RGB888).copy();

	int firstPosition = imagePath.toStdString().rfind("/");
	int lastPosition = imagePath.toStdString().find("_", firstPosition);

	int ticket = 0;
	if (firstPosition != std::string::npos && lastPosition != std::string::npos)
		ticket = imagePath.mid(firstPosition + 1, lastPosition - firstPosition - 1).toInt();

	imagePath = "../../../database/" + QString::number(vehicles.size()) + ".jpg";
	size_t plate = text.find('\n');
	size_t time = text.find('\n', plate + 1);

	vehicle = Vehicle(vehicles.size(), imagePath.toStdString(), ticket, text.substr(0, plate), text.substr(plate + 1, time - plate - 1));
	vehicles.push_back(vehicle);

	//int j = vehicle.getTicket();
	//std::ifstream file("C:\\Users\\George Patrasc\\OneDrive\\Pictures\\dataset\\annots.txt");
	//std::string line;
	//int currentLine = 1;
	//while (getline(file, line))
	//{
	//	if (currentLine == j)
	//	{
	//		std::cout << j;
	//		if (text.substr(0, plate) != line)
	//			std::cout << ": " << line << " / " << text.substr(0, plate);
	//		std::cout << std::endl;
	//		break;
	//	}
	//	currentLine++;
	//}
	//file.close();
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
		std::tm tmIn, tmOut;
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

	if (button == enterButton)
		entriesListWidget->addItem(item);
	else
		exitsListWidget->addItem(item);

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