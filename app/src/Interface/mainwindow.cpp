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
	setupTicketCallback();
}

void MainWindow::setupUI()
{
	graphicsView = new QGraphicsView(this);
	scene = new QGraphicsScene(this);
	graphicsView->setScene(scene);

	pixmapItem = new QGraphicsPixmapItem();
	scene->addItem(pixmapItem);

	graphicsView->setStyleSheet("background: transparent");
	graphicsView->setFrameShape(QFrame::NoFrame);
	graphicsView->setAttribute(Qt::WA_TranslucentBackground);
	scene->setBackgroundBrush(Qt::transparent);

	ticketGraphicsView = new QGraphicsView(this);
	ticketScene = new QGraphicsScene(this);
	ticketGraphicsView->setScene(ticketScene);

	ticketPixmapItem = new QGraphicsPixmapItem();
	ticketScene->addItem(ticketPixmapItem);

	ticketGraphicsView->setStyleSheet("background: transparent");
	ticketGraphicsView->setFrameShape(QFrame::NoFrame);
	ticketGraphicsView->setAttribute(Qt::WA_TranslucentBackground);
	ticketScene->setBackgroundBrush(Qt::transparent);

	isDevelopment();

	setupButtons();
	setupLayouts();

	this->setMinimumSize(1400, 700);
	showMaximized();

	connect(enterButton, &QPushButton::clicked, this, &MainWindow::uploadImage);
	connect(exitButton, &QPushButton::clicked, this, &MainWindow::uploadImage);
	connect(entriesListWidget, &QListWidget::itemClicked, this, &MainWindow::showImage);
	connect(exitsListWidget, &QListWidget::itemClicked, this, &MainWindow::showImage);
	connect(historyLogListWidget, &QListWidget::itemClicked, this, &MainWindow::showImage);
	connect(ticketsListWidget, &QListWidget::itemClicked, this, &MainWindow::showTicketImage);
	connect(ticketsHistoryLogListWidget, &QListWidget::itemClicked, this, &MainWindow::showTicketImage);
	connect(nameEdit, &QLineEdit::textChanged, this, &MainWindow::setName);
	connect(parkingLotsEdit, &QLineEdit::textChanged, this, &MainWindow::setNumberParkingLots);
	connect(reservedEdit, &QLineEdit::textChanged, this, &MainWindow::setNumberReserved);
	connect(feeEdit, &QLineEdit::textChanged, this, &MainWindow::setFee);
	connect(historyLogEdit, &QLineEdit::textChanged, this, &MainWindow::search);
	connect(ticketsHistoryLogEdit, &QLineEdit::textChanged, this, &MainWindow::searchTickets);
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

	ticketsListWidget = new QListWidget(this);
	QLabel* ticketsLabel = new QLabel(tr("Tickets"), this);
	ticketsLabel->setAlignment(Qt::AlignCenter);

	ticketsHistoryLogListWidget = new QListWidget(this);
	QLabel* ticketsHistoryLogLabel = new QLabel(tr("History Log"), this);
	ticketsHistoryLogLabel->setAlignment(Qt::AlignCenter);
	ticketsHistoryLogEdit = new QLineEdit(this);

	QLabel* nameLabel = new QLabel(tr("Name:"), this);
	QLabel* parkingLotsLabel = new QLabel(tr("Capacity:"), this);
	QLabel* reservedLabel = new QLabel(tr("Reserved:"), this);
	QLabel* occupiedParkingLotsLabel = new QLabel(tr("Occupancy:"), this);
	QLabel* feeLabel = new QLabel(tr("Fee:"), this);

	nameEdit = new QLineEdit(this);

	parkingLotsEdit = new QLineEdit(this);
	parkingLotsEdit->setValidator(new QIntValidator(0, 1000000, this));
	parkingLotsEdit->setText(QString::number(numberParkingLots));

	reservedEdit = new QLineEdit(this);
	reservedEdit->setValidator(new QIntValidator(0, 1000000, this));
	reservedEdit->setText(QString::number(numberReserved));

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
	topLayout->addWidget(reservedLabel);
	topLayout->addWidget(reservedEdit);
	topLayout->addWidget(occupiedParkingLotsLabel);
	topLayout->addWidget(occupiedParkingLotsEdit);
	topLayout->addWidget(feeLabel);
	topLayout->addWidget(feeEdit);

	QVBoxLayout* vehiclesLeftLayout = new QVBoxLayout();
	vehiclesLeftLayout->addWidget(graphicsView);

	QVBoxLayout* entriesLayout = new QVBoxLayout();
	entriesLayout->addWidget(entriesLabel);
	entriesLayout->addWidget(entriesListWidget);

	QVBoxLayout* exitsLayout = new QVBoxLayout();
	exitsLayout->addWidget(exitsLabel);
	exitsLayout->addWidget(exitsListWidget);

	QVBoxLayout* vehiclesHistoryLogLayout = new QVBoxLayout();
	vehiclesHistoryLogLayout->addWidget(historyLogLabel);
	vehiclesHistoryLogLayout->addWidget(historyLogEdit);
	vehiclesHistoryLogLayout->addWidget(historyLogListWidget);

	QHBoxLayout* vehiclesListsLayout = new QHBoxLayout();
	vehiclesListsLayout->addLayout(entriesLayout);
	vehiclesListsLayout->addLayout(exitsLayout);
	vehiclesListsLayout->addLayout(vehiclesHistoryLogLayout);

	QVBoxLayout* vehiclesRightLayout = new QVBoxLayout();
	vehiclesRightLayout->addLayout(vehiclesListsLayout);
	vehiclesRightLayout->addLayout(buttonLayout);

	QHBoxLayout* vehiclesMainLayout = new QHBoxLayout();
	vehiclesMainLayout->addLayout(vehiclesLeftLayout, 2);
	vehiclesMainLayout->addLayout(vehiclesRightLayout, 1.5);

	QWidget* vehiclesTab = new QWidget(this);
	vehiclesTab->setLayout(vehiclesMainLayout);

	QVBoxLayout* ticketsLeftLayout = new QVBoxLayout();
	ticketsLeftLayout->addWidget(ticketGraphicsView);

	QVBoxLayout* ticketsLayout = new QVBoxLayout();
	ticketsLayout->addWidget(ticketsLabel);
	ticketsLayout->addWidget(ticketsListWidget);

	QVBoxLayout* ticketsHistoryLogLayout = new QVBoxLayout();
	ticketsHistoryLogLayout->addWidget(ticketsHistoryLogLabel);
	ticketsHistoryLogLayout->addWidget(ticketsHistoryLogEdit);
	ticketsHistoryLogLayout->addWidget(ticketsHistoryLogListWidget);

	QHBoxLayout* ticketsRightLayout = new QHBoxLayout();
	ticketsRightLayout->addLayout(ticketsLayout);
	ticketsRightLayout->addLayout(ticketsHistoryLogLayout);

	QHBoxLayout* ticketsMainLayout = new QHBoxLayout();
	ticketsMainLayout->addLayout(ticketsLeftLayout, 2);
	ticketsMainLayout->addLayout(ticketsRightLayout, 1.5);

	QWidget* ticketsTab = new QWidget(this);
	ticketsTab->setLayout(ticketsMainLayout);

	tabWidget = new QTabWidget(this);
	tabWidget->addTab(vehiclesTab, tr("Vehicles"));
	tabWidget->addTab(ticketsTab, tr("Tickets"));
	tabWidget->setCurrentIndex(0);

	QVBoxLayout* mainLayout = new QVBoxLayout();
	mainLayout->addLayout(topLayout);
	mainLayout->addWidget(tabWidget);

	QWidget* centralWidget = new QWidget(this);
	centralWidget->setLayout(mainLayout);

	setCentralWidget(centralWidget);
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

	std::map<std::string, std::string> ticketsList;
	vehicleManager.uploadTickets(ticketsList);
	for (const auto& pair : ticketsList)
	{
		QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(pair.second));
		item->setData(Qt::UserRole, QString::fromStdString(pair.first));
		ticketsListWidget->addItem(item);
	}

	vehicleManager.setNumberOccupiedParkingLots(numberOccupiedParkingLots);
	occupiedParkingLotsEdit->setText(QString::number(numberOccupiedParkingLots));
}

void MainWindow::setupTicketCallback()
{
	vehicleManager.setTicketCallback(
		[this](const std::string& id)
		{
			std::string displayText = vehicleManager.processTicket(id);
			QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(displayText));
			item->setData(Qt::UserRole, QString::fromStdString(id));
			ticketsListWidget->addItem(item);
		});
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

	clearPreviousItems(scene);
	createNewPixmapItem(scene, pixmapItem, image);
	setGraphicsViewProperties(graphicsView, pixmapItem);
}

void MainWindow::clearPreviousItems(QGraphicsScene* targetScene)
{
	QList<QGraphicsItem*> items = targetScene->items();

	for (QGraphicsItem* item : items)
	{
		targetScene->removeItem(item);
		delete item;
	}
}

void MainWindow::createNewPixmapItem(QGraphicsScene* targetScene, QGraphicsPixmapItem*& targetPixmapItem, const QImage& targetImage)
{
	targetPixmapItem = new QGraphicsPixmapItem(QPixmap::fromImage(targetImage));
	targetScene->addItem(targetPixmapItem);
}

void MainWindow::setGraphicsViewProperties(QGraphicsView* targetGraphicsView, QGraphicsPixmapItem* targetPixmapItem)
{
	targetGraphicsView->setAlignment(Qt::AlignCenter);

	QRectF imageRect = targetPixmapItem->boundingRect();
	targetGraphicsView->setSceneRect(imageRect);
	targetGraphicsView->fitInView(imageRect, Qt::KeepAspectRatio);
}

void MainWindow::uploadImage()
{
	if (qobject_cast<QPushButton*>(sender()) == exitButton)
		pressedButton = true;
	else
		pressedButton = false;

	if (!verifyCapacity())
		return;

	QString imagePath = QFileDialog::getOpenFileName(this, tr("Upload Image"), "", "Images (*.png *.jpg *.bmp *.gif)");

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

	clearPreviousItems(scene);
	createNewPixmapItem(scene, pixmapItem, image);
	setGraphicsViewProperties(graphicsView, pixmapItem);
}

void MainWindow::setName(const QString& name)
{
	vehicleManager.setName(name.toStdString());
}

void MainWindow::setNumberParkingLots(const QString& numberParkingLots)
{
	this->numberParkingLots = numberParkingLots.toInt();
}

void MainWindow::setNumberReserved(const QString& numberReserved)
{
	int freeParkingLots = this->numberParkingLots - (this->numberOccupiedParkingLots - this->numberReserved);

	this->numberOccupiedParkingLots -= this->numberReserved;
	if (numberReserved.toInt() <= freeParkingLots)
		this->numberReserved = numberReserved.toInt();
	else
		this->numberReserved = 0;
	this->numberOccupiedParkingLots += this->numberReserved;

	occupiedParkingLotsEdit->setText(QString::number(this->numberOccupiedParkingLots));
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

void MainWindow::showTicketImage(QListWidgetItem* item)
{
	QString id = item->data(Qt::UserRole).toString();
	ticketImage.load(QString::fromStdString(vehicleManager.getTicketPath(id.toStdString())));

	clearPreviousItems(ticketScene);
	createNewPixmapItem(ticketScene, ticketPixmapItem, ticketImage);
	setGraphicsViewProperties(ticketGraphicsView, ticketPixmapItem);
}

void MainWindow::searchTickets(QString text)
{
	std::unordered_map<std::string, std::string> ticketsHistoryLogList;
	vehicleManager.searchTickets(text.toStdString(), ticketsHistoryLogList);

	ticketsHistoryLogListWidget->clear();

	for (const auto& pair : ticketsHistoryLogList)
	{
		QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(pair.second));
		item->setData(Qt::UserRole, QString::fromStdString(pair.first));
		ticketsHistoryLogListWidget->addItem(item);
	}
}
