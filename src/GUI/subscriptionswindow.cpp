#include "subscriptionswindow.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCloseEvent>

SubscriptionsWindow::SubscriptionsWindow(const std::unordered_map<std::string, std::vector<std::string>>& subscriptions, const QSize& buttonSize, QWidget* parent) :
	subscriptions(subscriptions),
	QWidget(parent)
{
	setWindowTitle(tr("Subscriptions"));
	setFixedSize(500, 600);

	setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);

	subscriptionsListWidget = new QListWidget(this);
	vehiclesListWidget = new QListWidget(this);

	QLabel* subscriptionsLabel = new QLabel(tr("Subscriptions"), this);
	QLabel* vehiclesLabel = new QLabel(tr("Vehicles"), this);

	subscriptionsLabel->setAlignment(Qt::AlignCenter);
	vehiclesLabel->setAlignment(Qt::AlignCenter);

	subscriptionsEdit = new QLineEdit(this);
	vehiclesEdit = new QLineEdit(this);

	subscriptionsEdit->setFixedHeight(buttonSize.height());
	vehiclesEdit->setFixedHeight(buttonSize.height());

	addSubscriptionButton = new QPushButton(tr("Add"), this);
	addVehicleButton = new QPushButton(tr("Add"), this);
	QPushButton* closeButton = new QPushButton(tr("Close"), this);

	addSubscriptionButton->setFixedSize(buttonSize);
	addVehicleButton->setFixedSize(buttonSize);
	closeButton->setFixedSize(buttonSize);

	QHBoxLayout* subscriptionEditLayout = new QHBoxLayout();
	subscriptionEditLayout->addWidget(subscriptionsEdit);
	subscriptionEditLayout->addWidget(addSubscriptionButton);

	QHBoxLayout* vehicleEditLayout = new QHBoxLayout();
	vehicleEditLayout->addWidget(vehiclesEdit);
	vehicleEditLayout->addWidget(addVehicleButton);

	QVBoxLayout* subscriptionsLayout = new QVBoxLayout();
	subscriptionsLayout->addWidget(subscriptionsLabel);
	subscriptionsLayout->addWidget(subscriptionsListWidget);
	subscriptionsLayout->addLayout(subscriptionEditLayout);

	QVBoxLayout* vehiclesLayout = new QVBoxLayout();
	vehiclesLayout->addWidget(vehiclesLabel);
	vehiclesLayout->addWidget(vehiclesListWidget);
	vehiclesLayout->addLayout(vehicleEditLayout);

	QHBoxLayout* listsLayout = new QHBoxLayout();
	listsLayout->addLayout(subscriptionsLayout);
	listsLayout->addLayout(vehiclesLayout);

	QVBoxLayout* mainLayout = new QVBoxLayout();
	mainLayout->addLayout(listsLayout);
	mainLayout->addWidget(closeButton, 0, Qt::AlignCenter);

	setLayout(mainLayout);

	connect(addSubscriptionButton, &QPushButton::clicked, this, &SubscriptionsWindow::addSubscription);
	connect(addVehicleButton, &QPushButton::clicked, this, &SubscriptionsWindow::addVehicle);
	connect(subscriptionsListWidget, &QListWidget::itemClicked, this, &SubscriptionsWindow::showVehicles);
	connect(closeButton, &QPushButton::clicked, this, &SubscriptionsWindow::closeWindow);

	for (const auto& subscription : subscriptions)
	{
		QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(subscription.first));
		item->setData(Qt::UserRole, QString::fromStdString(subscription.first));
		subscriptionsListWidget->addItem(item);
	}
}

void SubscriptionsWindow::addSubscription()
{

	std::string subscription = subscriptionsEdit->text().toStdString();

	if (subscriptions.find(subscription) == subscriptions.end())
	{
		subscriptions[subscription] = std::vector<std::string>();

		QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(subscription));
		item->setData(Qt::UserRole, QString::fromStdString(subscription));
		subscriptionsListWidget->addItem(item);
	}

	subscriptionsEdit->clear();
}

void SubscriptionsWindow::addVehicle()
{
	std::string vehicle = vehiclesEdit->text().toStdString();
	QListWidgetItem* selectedItem = subscriptionsListWidget->currentItem();

	if (selectedItem)
	{
		std::string subscription = selectedItem->data(Qt::UserRole).toString().toStdString();
		auto it = subscriptions.find(subscription);

		std::transform(vehicle.begin(), vehicle.end(), vehicle.begin(), [](unsigned char c) {
			return std::toupper(c); });

		std::vector<std::string>& vehicles = it->second;
		if (std::find(vehicles.begin(), vehicles.end(), vehicle) == vehicles.end())
		{
			subscriptions[subscription].push_back(vehicle);

			QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(vehicle));
			vehiclesListWidget->addItem(item);
		}
	}

	vehiclesEdit->clear();
}

void SubscriptionsWindow::showVehicles(QListWidgetItem* item)
{
	std::string subscription = item->data(Qt::UserRole).toString().toStdString();

	vehiclesListWidget->clear();
	for (const auto& vehicle : subscriptions[subscription])
	{
		QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(vehicle));
		vehiclesListWidget->addItem(item);
	}
}

void SubscriptionsWindow::closeWindow()
{
	emit getSubscriptions(subscriptions);
	this->close();
}

void SubscriptionsWindow::closeEvent(QCloseEvent* event)
{
	emit getSubscriptions(subscriptions);
	event->accept();
}
