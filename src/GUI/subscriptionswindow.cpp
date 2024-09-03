#include "subscriptionswindow.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCloseEvent>

SubscriptionsWindow::SubscriptionsWindow(SubscriptionManager* subscriptionManager, const QSize& buttonSize, QWidget* parent) :
	subscriptionManager(subscriptionManager),
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

	QPushButton* closeButton = new QPushButton(tr("Close"), this);
	closeButton->setFixedSize(buttonSize);

	QVBoxLayout* subscriptionsLayout = new QVBoxLayout();
	subscriptionsLayout->addWidget(subscriptionsLabel);
	subscriptionsLayout->addWidget(subscriptionsListWidget);

	QVBoxLayout* vehiclesLayout = new QVBoxLayout();
	vehiclesLayout->addWidget(vehiclesLabel);
	vehiclesLayout->addWidget(vehiclesListWidget);

	QHBoxLayout* listsLayout = new QHBoxLayout();
	listsLayout->addLayout(subscriptionsLayout);
	listsLayout->addLayout(vehiclesLayout);

	QVBoxLayout* mainLayout = new QVBoxLayout();
	mainLayout->addLayout(listsLayout);
	mainLayout->addWidget(closeButton, 0, Qt::AlignCenter);

	setLayout(mainLayout);

	connect(subscriptionsListWidget, &QListWidget::itemClicked, this, &SubscriptionsWindow::showVehicles);
	connect(closeButton, &QPushButton::clicked, this, &SubscriptionsWindow::closeWindow);

	for (const auto& subscription : subscriptionManager->getSubscriptions())
	{
		QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(subscription.getName()));
		item->setData(Qt::UserRole, QString::fromStdString(subscription.getName()));
		subscriptionsListWidget->addItem(item);
	}
}

void SubscriptionsWindow::showVehicles(QListWidgetItem* item)
{
	std::string name = item->data(Qt::UserRole).toString().toStdString();
	Subscription subscription = subscriptionManager->find(name);

	vehiclesListWidget->clear();
	for (const auto& vehicle : subscription.getVehicles())
	{
		QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(vehicle));
		vehiclesListWidget->addItem(item);
	}
}

void SubscriptionsWindow::closeWindow()
{
	this->close();
}
