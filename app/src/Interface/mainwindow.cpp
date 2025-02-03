#include "mainwindow.h"
#include "statisticswindow.h"
#include "uploadqrwindow.h"
#include "unpaidwindow.h"

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
	connect(entriesListWidget, &QListWidget::itemClicked, this, &MainWindow::showImage);
	connect(exitsListWidget, &QListWidget::itemClicked, this, &MainWindow::showImage);
	connect(historyLogListWidget, &QListWidget::itemClicked, this, &MainWindow::showImage);
	connect(nameEdit, &QLineEdit::textChanged, this, &MainWindow::setName);
	connect(parkingLotsEdit, &QLineEdit::textChanged, this, &MainWindow::setNumberParkingLots);
	connect(feeEdit, &QLineEdit::textChanged, this, &MainWindow::setFee);
	connect(historyLogEdit, &QLineEdit::textChanged, this, &MainWindow::search);
	connect(statisticsButton, &QPushButton::clicked, this, &MainWindow::showStatistics);
	connect(chooseLanguage, &QComboBox::currentIndexChanged, this, &MainWindow::setLanguage);

	uploadDataBase();
}

void MainWindow::isDevelopment()
{
#ifdef _DEBUG
	assetsPath = "../../../assets/";
	translationsPath = "../../../translations/";
#else
	assetsPath = "assets/";
	translationsPath = "translations/";
#endif
}

void MainWindow::setupButtons()
{
	enterButton = new QPushButton(this);
	std::string enterPath = assetsPath + tr("entrance.png").toStdString();
	QPixmap enterPixmap(enterPath.c_str());
	enterButton->setIcon(QIcon(enterPixmap));
	enterButton->setIconSize(enterPixmap.size() / 5);
	enterButton->setFixedSize(enterPixmap.size() / 5);

	exitButton = new QPushButton(this);
	std::string exitPath = assetsPath + tr("exit.png").toStdString();
	QPixmap exitPixmap(exitPath.c_str());
	exitButton->setIcon(QIcon(exitPixmap));
	exitButton->setIconSize(exitPixmap.size() / 5);
	exitButton->setFixedSize(exitPixmap.size() / 5);

	statisticsButton = new QPushButton(this);
	std::string statisticsPath = assetsPath + tr("statistics.png").toStdString();
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

	QLabel* nameLabel = new QLabel(tr("Name:"), this);
	QLabel* parkingLotsLabel = new QLabel(tr("Capacity:"), this);
	QLabel* occupiedParkingLotsLabel = new QLabel(tr("Occupancy:"), this);
	QLabel* feeLabel = new QLabel(tr("Fee:"), this);

	nameEdit = new QLineEdit(this);

	parkingLotsEdit = new QLineEdit(this);
	parkingLotsEdit->setValidator(new QIntValidator(0, 1000000, this));
	parkingLotsEdit->setText(QString::number(numberParkingLots));

	occupiedParkingLotsEdit = new QLineEdit(this);
	occupiedParkingLotsEdit->setReadOnly(true);

	feeEdit = new QLineEdit(this);
	feeEdit->setValidator(new QIntValidator(0, 1000000, this));
	feeEdit->setText(QString::number(fee));

	QHBoxLayout* topLayout = new QHBoxLayout();
	topLayout->addWidget(chooseLanguage);
	topLayout->addWidget(nameLabel);
	topLayout->addWidget(nameEdit);
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

void MainWindow::updateStatistics(const std::string& dateTime, const bool& statistics)
{
	QDateTime date = QDateTime::fromString(QString::fromStdString(dateTime), "dd-MM-yyyy HH:mm:ss");
	int dayOfWeek = date.date().dayOfWeek();
	int hour = date.time().hour();

	if (statistics)
		vehicleManager.increaseExitStatistics(dayOfWeek, hour);
	else
		vehicleManager.increaseEntranceStatistics(dayOfWeek, hour);
}

void MainWindow::uploadDataBase()
{
	std::vector<std::string> entranceDateTimes;
	std::vector<std::string> exitDateTimes;
	vehicleManager.uploadDataBase(entranceDateTimes, exitDateTimes);
	for (const auto& dateTime : entranceDateTimes)
		updateStatistics(dateTime, false);
	for (const auto& dateTime : exitDateTimes)
		updateStatistics(dateTime, true);

	std::map<int, std::string> entriesList;
	std::map<int, std::string> exitsList;
	vehicleManager.uploadVehicles(entriesList, exitsList);
	for (const auto& pair : entriesList)
	{
		QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(pair.second));
		item->setData(Qt::UserRole, pair.first);
		entriesListWidget->addItem(item);
	}
	for (const auto& pair : exitsList)
	{
		QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(pair.second));
		item->setData(Qt::UserRole, pair.first);
		exitsListWidget->addItem(item);
	}

	vehicleManager.setNumberOccupiedParkingLots(numberOccupiedParkingLots);
	occupiedParkingLotsEdit->setText(QString::number(numberOccupiedParkingLots));
}

bool MainWindow::verifyCapacity()
{
	if (!pressedButton && numberOccupiedParkingLots >= numberParkingLots)
	{
		QMessageBox::warning(this, tr("Parking Full"), tr("All parking lots are occupied."));
		return false;
	}

	if (pressedButton && numberOccupiedParkingLots < 1)
	{
		QMessageBox::warning(this, tr("No Vehicles"), tr("There are no vehicles in the parking lot."));
		return false;
	}

	return true;
}

bool MainWindow::checkResult(const int& result)
{
	switch (result)
	{
	case 0:
		return true;
	case 1:
	{
		UnpaidWindow* unpaidWindow = new UnpaidWindow(enterButton->size(), this);

		QEventLoop loop;
		connect(unpaidWindow, &QDialog::rejected, &loop, &QEventLoop::quit);
		unpaidWindow->show();
		loop.exec();

		return false;
	}
	case 2:
	{
		UploadQRWindow* uploadQRWindow = new UploadQRWindow(enterButton->size(), this);
		connect(uploadQRWindow, &UploadQRWindow::getQRPath, this, &MainWindow::processLastVehicle);

		QEventLoop loop;
		connect(uploadQRWindow, &UploadQRWindow::getQRPath, &loop, &QEventLoop::quit);
		uploadQRWindow->show();
		loop.exec();

		return false;
	}
	}

	return false;
}

void MainWindow::processLastVehicle(const QString& QRPath)
{
	int id, result;
	std::string dateTime;
	std::string displayText;

	if (!checkResult(vehicleManager.processLastVehicle(id, dateTime, displayText, fee, pressedButton, QRPath.toStdString())))
		return;

	updateStatistics(dateTime, pressedButton);

	if (pressedButton)
		numberOccupiedParkingLots--;
	else
		numberOccupiedParkingLots++;
	occupiedParkingLotsEdit->setText(QString::number(numberOccupiedParkingLots));

	QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(displayText));
	item->setData(Qt::UserRole, id);
	if (pressedButton)
		exitsListWidget->addItem(item);
	else
		entriesListWidget->addItem(item);

	clearPreviousItems();
	createNewPixmapItem();
	setGraphicsViewProperties();
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

void MainWindow::uploadImage()
{
	if (qobject_cast<QPushButton*>(sender()) == exitButton)
		pressedButton = true;
	else
		pressedButton = false;

	if (!verifyCapacity())
		return;

	QString imagePath = QFileDialog::getOpenFileName(this, "Upload Image", "", "Images (*.png *.jpg *.bmp *.gif)");

	if (imagePath.isEmpty())
		return;

	std::string savePath;
	vehicleManager.getVehicle(imagePath.toStdString(), savePath);
	image = QImage(QString::fromStdString(savePath));

	processLastVehicle();
}

void MainWindow::showImage(QListWidgetItem* item)
{
	int id = item->data(Qt::UserRole).toInt();
	image.load(QString::fromStdString(vehicleManager.getImagePath(id)));

	clearPreviousItems();
	createNewPixmapItem();
	setGraphicsViewProperties();
}

void MainWindow::setName(const QString& name)
{
	vehicleManager.setName(name.toStdString());
}

void MainWindow::setNumberParkingLots(const QString& numberParkingLots)
{
	this->numberParkingLots = numberParkingLots.toInt();
}

void MainWindow::setFee(const QString& fee)
{
	this->fee = fee.toInt();
}

void MainWindow::search(QString text)
{
	std::unordered_map<int, std::string> historyLogList;
	vehicleManager.search(text.toStdString(), historyLogList);

	historyLogListWidget->clear();

	for (const auto& pair : historyLogList)
	{
		QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(pair.second));
		item->setData(Qt::UserRole, pair.first);
		historyLogListWidget->addItem(item);
	}
}

void MainWindow::updateOccupancyStatistics(const std::vector<std::pair<std::string, std::string>>& occupancyDateTimes)
{
	for (const auto& dateTime : occupancyDateTimes)
	{
		QDateTime start = QDateTime::fromString(QString::fromStdString(dateTime.first), "dd-MM-yyyy HH:mm:ss");
		QDateTime end = QDateTime::fromString(QString::fromStdString(dateTime.second), "dd-MM-yyyy HH:mm:ss");

		while (start <= end)
		{
			int day = start.date().dayOfWeek();
			int hour = start.time().hour();

			vehicleManager.increaseOccupancyStatistics(day, hour);

			start = start.addSecs(3600);
		}
	}
}

void MainWindow::showStatistics()
{
	vehicleManager.calculateOccupancyStatistics();

	StatisticsWindow* statisticsWindow = new StatisticsWindow(vehicleManager.getOccupancyStatistics(), vehicleManager.getEntranceStatistics(), vehicleManager.getExitStatistics(), enterButton->size());

	statisticsWindow->show();
}

void MainWindow::setLanguage(const int& choise)
{
	switch (choise)
	{
	case 0:
		if (translator.load(QString::fromStdString(translationsPath + "eng.qm")))
			qApp->installTranslator(&translator);
		break;
	case 1:
		if (translator.load(QString::fromStdString(translationsPath + "ro.qm")))
			qApp->installTranslator(&translator);
		break;
	}

	this->close();
	setupUI();
	this->show();
}
