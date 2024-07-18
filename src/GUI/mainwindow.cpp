#include "mainwindow.h"
#include "ImageProcessingUtils.h"

#include <QFileDialog>
#include <QVBoxLayout>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QLabel>
#include <QScreen>
#include <QGuiApplication>
#include <QMessageBox>
#include <QTranslator>

QTranslator translator;

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

	setupButtons();
	setupLayouts();
	centerWindow();

	connect(enterButton, &QPushButton::clicked, this, &MainWindow::uploadImage);
	connect(exitButton, &QPushButton::clicked, this, &MainWindow::uploadImage);
	connect(entriesListWidget, &QListWidget::itemClicked, this, &MainWindow::displayImage);
	connect(exitsListWidget, &QListWidget::itemClicked, this, &MainWindow::displayImage);
	connect(historyLogListWidget, &QListWidget::itemClicked, this, &MainWindow::displayImage);
	connect(parkingLotsEdit, &QLineEdit::textChanged, this, &MainWindow::setNumberParkingLots);
	connect(feeEdit, &QLineEdit::textChanged, this, &MainWindow::setFee);
	connect(historyLogEdit, &QLineEdit::textChanged, this, &MainWindow::search);
	connect(statisticsButton, &QPushButton::clicked, this, &MainWindow::showStatistics);
	connect(chooseLanguage, &QComboBox::currentIndexChanged, this, &MainWindow::setLanguage);

	writeFile = std::ofstream(databasePath + "vehicles.txt", std::ios::app);
	uploadDataBase();
	uploadVehicles();
	setNumberOccupiedParkingLots();
}

void MainWindow::isDevelopment()
{
	if (std::filesystem::exists("../../../database"))
	{
		databasePath = "../../../database/";
		translationsPath = "../../../translations/";
	}
	else
	{
		databasePath = "database/";
		translationsPath = "translations/";
	}
}

void MainWindow::setupButtons()
{
	enterButton = new QPushButton(this);
	std::string enterPath = databasePath + tr("entrance.png").toStdString();
	QPixmap enterPixmap(enterPath.c_str());
	enterButton->setIcon(QIcon(enterPixmap));
	enterButton->setIconSize(enterPixmap.size() / 5);
	enterButton->setFixedSize(enterPixmap.size() / 5);

	exitButton = new QPushButton(this);
	std::string exitPath = databasePath + tr("exit.png").toStdString();
	QPixmap exitPixmap(exitPath.c_str());
	exitButton->setIcon(QIcon(exitPixmap));
	exitButton->setIconSize(exitPixmap.size() / 5);
	exitButton->setFixedSize(exitPixmap.size() / 5);

	statisticsButton = new QPushButton(this);
	std::string statisticsPath = databasePath + tr("statistics.png").toStdString();
	QPixmap statisticsPixmap(statisticsPath.c_str());
	statisticsButton->setIcon(QIcon(statisticsPixmap));
	statisticsButton->setIconSize(statisticsPixmap.size() / 5);
	statisticsButton->setFixedSize(statisticsPixmap.size() / 5);

	chooseLanguage = new QComboBox(this);
	chooseLanguage->addItem("ENG");
	chooseLanguage->addItem("RO");
	chooseLanguage->setFixedSize(60, 30);

	if (translator.language() == "eng")
		chooseLanguage->setCurrentIndex(0);
	else if (translator.language() == "ro")
		chooseLanguage->setCurrentIndex(1);
}

void MainWindow::setupLayouts()
{
	QHBoxLayout* buttonLayout = new QHBoxLayout();
	buttonLayout->addWidget(enterButton, 0, Qt::AlignCenter);
	buttonLayout->addWidget(exitButton, 0, Qt::AlignCenter);
	buttonLayout->addWidget(statisticsButton, 0, Qt::AlignCenter);

	entriesListWidget = new QListWidget(this);
	QLabel* entriesLabel = new QLabel(tr("Entries"), this);
	entriesLabel->setAlignment(Qt::AlignCenter);

	exitsListWidget = new QListWidget(this);
	QLabel* exitsLabel = new QLabel(tr("Exits"), this);
	exitsLabel->setAlignment(Qt::AlignCenter);

	historyLogListWidget = new QListWidget(this);
	QLabel* historyLogLabel = new QLabel(tr("History Log"), this);
	historyLogLabel->setAlignment(Qt::AlignCenter);
	historyLogEdit = new QLineEdit(this);

	QLabel* parkingLotsLabel = new QLabel(tr("Capacity:"), this);
	QLabel* occupiedParkingLotsLabel = new QLabel(tr("Occupancy:"), this);
	QLabel* feeLabel = new QLabel(tr("Fee:"), this);

	occupiedParkingLotsEdit = new QLineEdit(this);
	occupiedParkingLotsEdit->setReadOnly(true);

	parkingLotsEdit = new QLineEdit(this);
	parkingLotsEdit->setValidator(new QIntValidator(0, 1000000, this));
	parkingLotsEdit->setText(QString::number(numberParkingLots));

	feeEdit = new QLineEdit(this);
	feeEdit->setValidator(new QIntValidator(0, 1000000, this));
	feeEdit->setText(QString::number(fee));

	QHBoxLayout* topLayout = new QHBoxLayout();
	topLayout->addWidget(chooseLanguage);
	topLayout->addWidget(parkingLotsLabel);
	topLayout->addWidget(parkingLotsEdit);
	topLayout->addWidget(occupiedParkingLotsLabel);
	topLayout->addWidget(occupiedParkingLotsEdit);
	topLayout->addWidget(feeLabel);
	topLayout->addWidget(feeEdit);

	QVBoxLayout* leftLayout = new QVBoxLayout();
	leftLayout->addLayout(topLayout);
	leftLayout->addWidget(graphicsView);

	QVBoxLayout* entriesLayout = new QVBoxLayout();
	entriesLayout->addWidget(entriesLabel);
	entriesLayout->addWidget(entriesListWidget);

	QVBoxLayout* exitsLayout = new QVBoxLayout();
	exitsLayout->addWidget(exitsLabel);
	exitsLayout->addWidget(exitsListWidget);

	QVBoxLayout* historyLogLayout = new QVBoxLayout();
	historyLogLayout->addWidget(historyLogLabel);
	historyLogLayout->addWidget(historyLogEdit);
	historyLogLayout->addWidget(historyLogListWidget);

	QHBoxLayout* listsLayout = new QHBoxLayout();
	listsLayout->addLayout(entriesLayout);
	listsLayout->addLayout(exitsLayout);
	listsLayout->addLayout(historyLogLayout);

	QVBoxLayout* rightLayout = new QVBoxLayout();
	rightLayout->addLayout(listsLayout);
	rightLayout->addLayout(buttonLayout);

	QHBoxLayout* mainLayout = new QHBoxLayout();
	mainLayout->addLayout(leftLayout, 2);
	mainLayout->addLayout(rightLayout, 1.5);

	QWidget* centralWidget = new QWidget(this);
	centralWidget->setLayout(mainLayout);

	setCentralWidget(centralWidget);
}

void MainWindow::centerWindow()
{
	setFixedSize(1400, 700);

	QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
	int x = (screenGeometry.width() - width()) / 2;
	int y = (screenGeometry.height() - height()) / 2;

	move(x, y);
}

void MainWindow::uploadDataBase()
{
	std::ifstream readFile(databasePath + "vehicles.txt");

	enterStatistics = std::vector<std::vector<int>>(7, std::vector<int>(24, 0));
	exitStatistics = std::vector<std::vector<int>>(7, std::vector<int>(24, 0));

	std::vector<std::string> vehicleData;
	std::string line;
	while (std::getline(readFile, line))
	{
		if (line.empty())
		{
			curentVehicle = Vehicle(std::stoi(vehicleData[0]), vehicleData[1], std::stoi(vehicleData[2]), vehicleData[3], vehicleData[4]);
			QDateTime date = QDateTime::fromString(QString::fromStdString(vehicleData[4]), "dd-MM-yyyy HH:mm:ss");
			int dayOfWeek = date.date().dayOfWeek();
			int hour = date.time().hour();

			if (vehicleData.size() == 5)
				enterStatistics[dayOfWeek - 1][hour]++;
			else
			{
				curentVehicle.setTimeParked(vehicleData[5]);
				curentVehicle.setTotalAmount(std::stoi(vehicleData[6]));
				exitStatistics[dayOfWeek - 1][hour]++;
			}

			vehicles.push_back(curentVehicle);
			vehicleData.clear();
		}
		else
			vehicleData.push_back(line);
	}

	readFile.close();
}

void MainWindow::processUploadedVehicle()
{
	displayText = curentVehicle.getLicensePlate() + "\n" + curentVehicle.getTime();

	if (!curentVehicle.getTimeParked().empty())
		displayText += '\n' + curentVehicle.getTimeParked() + '\n' + std::to_string(curentVehicle.getTotalAmount()) + " RON";

	QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(displayText));
	item->setData(Qt::UserRole, curentVehicle.getId());

	if (curentVehicle.getTimeParked().empty())
	{
		entriesListWidget->addItem(item);
		vehiclesStatus[curentVehicle.getTicket()] = true;
	}
	else
	{
		exitsListWidget->addItem(item);
		vehiclesStatus[curentVehicle.getTicket()] = false;
	}
}

void MainWindow::uploadVehicles()
{
	for (const auto& vehicle : vehicles)
	{
		curentVehicle = vehicle;
		processUploadedVehicle();
	}
}

void MainWindow::setNumberOccupiedParkingLots()
{
	for (const auto& vehicle : vehiclesStatus)
	{
		if (vehicle.second)
			numberOccupiedParkingLots++;
	}

	occupiedParkingLotsEdit->setText(QString::number(numberOccupiedParkingLots));
}

bool MainWindow::verifyCapacity()
{
	if (pressedButton == enterButton && numberOccupiedParkingLots >= numberParkingLots)
	{
		QMessageBox::warning(this, tr("Parking Full"), tr("All parking lots are occupied."));
		return false;
	}

	if (pressedButton == exitButton && numberOccupiedParkingLots < 1)
	{
		QMessageBox::warning(this, tr("No Vehicles"), tr("There are no vehicles in the parking lot."));
		return false;
	}

	return true;
}

bool MainWindow::getVehicle()
{
	cv::Mat cvImage(image.height(), image.width(), CV_8UC4, const_cast<uchar*>(image.bits()), image.bytesPerLine());
	std::string text = textFromImage(cvImage, cvImage);
	image = QImage(cvImage.data, cvImage.cols, cvImage.rows, static_cast<int>(cvImage.step), QImage::Format_RGB888).copy();

	int firstPosition = imagePath.toStdString().rfind("/");
	int lastPosition = imagePath.toStdString().find("_", firstPosition);

	int ticket = 0;
	if (firstPosition != std::string::npos && lastPosition != std::string::npos)
		ticket = imagePath.mid(firstPosition + 1, lastPosition - firstPosition - 1).toInt();

	size_t plate = text.find('\n');
	size_t time = text.find('\n', plate + 1);

	imagePath = QString::fromStdString(databasePath) + QString::number(vehicles.size()) + ".jpg";

	curentVehicle = Vehicle(vehicles.size(), imagePath.toStdString(), ticket, text.substr(0, plate), text.substr(plate + 1, time - plate - 1));

	return true;
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

Vehicle MainWindow::findVehicle(const bool& direction, int index)
{
	Vehicle result = Vehicle();

	if (index == -1)
		index = vehicles.size();

	if (direction)
	{
		if (curentVehicle.getLicensePlate() != "necunoscut")
			for (int i = index - 1; i >= 0; i--)
			{
				if (curentVehicle.getLicensePlate() == vehicles[i].getLicensePlate())
					return vehicles[i];
			}

		if (curentVehicle.getTicket())
			for (int i = index - 1; i >= 0; i--)
			{
				if (curentVehicle.getTicket() == vehicles[i].getTicket())
					return vehicles[i];
			}
	}
	else
	{
		if (curentVehicle.getLicensePlate() != "necunoscut")
			for (int i = index + 1; i < vehicles.size(); i++)
			{
				if (curentVehicle.getLicensePlate() == vehicles[i].getLicensePlate())
					return vehicles[i];
			}

		if (curentVehicle.getTicket())
			for (int i = index + 1; i < vehicles.size(); i++)
			{
				if (curentVehicle.getTicket() == vehicles[i].getTicket())
					return vehicles[i];
			}
	}

	return Vehicle();
}

std::string MainWindow::timeParked()
{
	Vehicle auxVehicle = findVehicle();

	if (auxVehicle.getLicensePlate() == "")
		return "00:00:00";

	std::ostringstream timeStream;
	std::tm tmIn = {}, tmOut = {};
	std::istringstream inStr(auxVehicle.getTime());
	std::istringstream outStr(curentVehicle.getTime());
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

	return timeStream.str();
}

int MainWindow::calculateTotalAmount(const std::string& time)
{
	int hours = std::stoi(time.substr(0, 2)) + 1;

	return hours * fee;
}

void MainWindow::updateStatistics()
{
	QDateTime date = QDateTime::fromString(QString::fromStdString(curentVehicle.getTime()), "dd-MM-yyyy HH:mm:ss");
	int dayOfWeek = date.date().dayOfWeek();
	int hour = date.time().hour();

	if (curentVehicle.getTimeParked() == "")
		enterStatistics[dayOfWeek - 1][hour]++;
	else
		exitStatistics[dayOfWeek - 1][hour]++;
}

void MainWindow::processLastVehicle()
{
	displayText = curentVehicle.getLicensePlate() + "\n" + curentVehicle.getTime();

	if (pressedButton == exitButton)
	{
		std::string time = timeParked();
		int totalAmount = calculateTotalAmount(time);

		displayText += '\n' + time + '\n' + std::to_string(totalAmount) + " RON";

		curentVehicle.setTimeParked(time);
		curentVehicle.setTotalAmount(totalAmount);

		numberOccupiedParkingLots--;
	}
	else
		numberOccupiedParkingLots++;

	occupiedParkingLotsEdit->setText(QString::number(numberOccupiedParkingLots));

	QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(displayText));
	item->setData(Qt::UserRole, curentVehicle.getId());

	if (pressedButton == exitButton)
		exitsListWidget->addItem(item);
	else
		entriesListWidget->addItem(item);

	updateStatistics();
	vehicles.push_back(curentVehicle);

	image.save(imagePath);
	writeFile << curentVehicle;
}

void MainWindow::uploadImage()
{
	pressedButton = qobject_cast<QPushButton*>(sender());

	if (!verifyCapacity())
		return;

	imagePath = QFileDialog::getOpenFileName(this, "Upload Image", "", "Images (*.png *.jpg *.bmp *.gif)");

	if (imagePath.isEmpty())
		return;

	image.load(imagePath);

	if (!getVehicle())
		return;

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

void MainWindow::setNumberParkingLots(const QString& numberParkingLots)
{
	this->numberParkingLots = numberParkingLots.toInt();
}

void MainWindow::setFee(const QString& fee)
{
	this->fee = fee.toInt();
}

void MainWindow::search(const QString& text)
{
	historyLogListWidget->clear();

	std::string str = text.toStdString();

	if (str == "")
		return;

	std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::toupper(c); });

	for (const auto& vehicle : vehicles)
		if (vehicle.getLicensePlate().find(str) != std::string::npos)
		{
			displayText = vehicle.getLicensePlate() + "\n" + vehicle.getTime();
			if (vehicle.getTimeParked() != "")
				displayText += '\n' + vehicle.getTimeParked() + '\n' + std::to_string(vehicle.getTotalAmount()) + " RON";

			QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(displayText));
			item->setData(Qt::UserRole, vehicle.getId());
			historyLogListWidget->addItem(item);
		}
}

void MainWindow::increaseOccupancyStatistics(const std::string& startTime, const std::string& endTime)
{
	QDateTime start = QDateTime::fromString(QString::fromStdString(startTime), "dd-MM-yyyy HH:mm:ss");
	QDateTime end = QDateTime::fromString(QString::fromStdString(endTime), "dd-MM-yyyy HH:mm:ss");

	while (start <= end)
	{
		int day = start.date().dayOfWeek();
		int hour = start.time().hour();

		occupancyStatistics[day - 1][hour]++;

		start = start.addSecs(3600);
	}
}

void MainWindow::calculateOccupancyStatistics()
{
	occupancyStatistics = std::vector<std::vector<int>>(7, std::vector<int>(24, 0));

	for (int i = 0; i < vehicles.size(); i++)
		if (vehicles[i].getTimeParked() == "")
		{
			curentVehicle = vehicles[i];
			Vehicle auxVehicle = findVehicle(false, i);

			if (auxVehicle.getLicensePlate() != "")
				increaseOccupancyStatistics(vehicles[i].getTime(), auxVehicle.getTime());
			else
			{
				auto now = std::chrono::system_clock::now();
				auto toTimeT = std::chrono::system_clock::to_time_t(now);

				std::stringstream ss;
				ss << std::put_time(std::localtime(&toTimeT), "%d-%m-%Y %X");

				increaseOccupancyStatistics(vehicles[i].getTime(), ss.str());
			}
		}
}

void MainWindow::showStatistics()
{
	calculateOccupancyStatistics();

	StatisticsWindow* statisticsWindow = new StatisticsWindow(occupancyStatistics, enterStatistics, exitStatistics);

	statisticsWindow->show();
}

void MainWindow::setLanguage(const int& choise)
{
	switch (choise)
	{
	case 0:
		if (translator.load(QString::fromStdString(translationsPath + "gui_eng.qm")))
			qApp->installTranslator(&translator);
		break;
	case 1:
		if (translator.load(QString::fromStdString(translationsPath + "gui_ro.qm")))
			qApp->installTranslator(&translator);
		break;
	}

	this->close();
	MainWindow* refresh = new MainWindow();
	refresh->show();
}

MainWindow::~MainWindow()
{
	writeFile.close();
}
