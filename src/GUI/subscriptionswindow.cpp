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
	setFixedSize(800, 600);

	setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
	setWindowModality(Qt::ApplicationModal);

	accountsListWidget = new QListWidget(this);
	subscriptionsListWidget = new QListWidget(this);
	vehiclesListWidget = new QListWidget(this);

	QLabel* accountsLabel = new QLabel(tr("Accounts"), this);
	QLabel* subscriptionsLabel = new QLabel(tr("Subscriptions"), this);
	QLabel* vehiclesLabel = new QLabel(tr("Vehicles"), this);

	accountsLabel->setAlignment(Qt::AlignCenter);
	subscriptionsLabel->setAlignment(Qt::AlignCenter);
	vehiclesLabel->setAlignment(Qt::AlignCenter);

	QPushButton* closeButton = new QPushButton(tr("Close"), this);
	closeButton->setFixedSize(buttonSize);

	QVBoxLayout* accountsLayout = new QVBoxLayout();
	accountsLayout->addWidget(accountsLabel);
	accountsLayout->addWidget(accountsListWidget);

	QVBoxLayout* subscriptionsLayout = new QVBoxLayout();
	subscriptionsLayout->addWidget(subscriptionsLabel);
	subscriptionsLayout->addWidget(subscriptionsListWidget);

	QVBoxLayout* vehiclesLayout = new QVBoxLayout();
	vehiclesLayout->addWidget(vehiclesLabel);
	vehiclesLayout->addWidget(vehiclesListWidget);

	QHBoxLayout* listsLayout = new QHBoxLayout();
	listsLayout->addLayout(accountsLayout);
	listsLayout->addLayout(subscriptionsLayout);
	listsLayout->addLayout(vehiclesLayout);

	QVBoxLayout* mainLayout = new QVBoxLayout();
	mainLayout->addLayout(listsLayout);
	mainLayout->addWidget(closeButton, 0, Qt::AlignCenter);

	setLayout(mainLayout);

	connect(accountsListWidget, &QListWidget::itemClicked, this, &SubscriptionsWindow::showSubscriptions);
	connect(subscriptionsListWidget, &QListWidget::itemClicked, this, [this](QListWidgetItem* item) {
		showVehicles(accountsListWidget->currentItem(), item);
		});
	connect(closeButton, &QPushButton::clicked, this, &SubscriptionsWindow::closeWindow);

	for (const auto& account : subscriptionManager->getAccounts())
	{
		QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(account.first.getEmail()));
		item->setData(Qt::UserRole, QString::fromStdString(account.first.getEmail()));
		accountsListWidget->addItem(item);
	}
}

void SubscriptionsWindow::showSubscriptions(QListWidgetItem* item)
{
	std::string email = item->data(Qt::UserRole).toString().toStdString();
	Account* account = subscriptionManager->getAccountByEmail(email);

	subscriptionsListWidget->clear();
	vehiclesListWidget->clear();

	for (const auto& subscription : *subscriptionManager->getSubscriptions(*account))
	{
		QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(subscription.getName()));
		item->setData(Qt::UserRole, QString::fromStdString(subscription.getName()));
		subscriptionsListWidget->addItem(item);
	}
}

void SubscriptionsWindow::showVehicles(QListWidgetItem* accountsItem, QListWidgetItem* subscriptionItem)
{
	std::string email = accountsItem->data(Qt::UserRole).toString().toStdString();
	Account* account = subscriptionManager->getAccountByEmail(email);

	std::string name = subscriptionItem->data(Qt::UserRole).toString().toStdString();
	Subscription* subscription = subscriptionManager->getSubscription(*account, name);

	vehiclesListWidget->clear();

	if (subscription)
	{
		for (const auto& vehicle : subscription->getVehicles())
		{
			QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(vehicle));
			vehiclesListWidget->addItem(item);
		}
	}
}

void SubscriptionsWindow::closeWindow()
{
	this->close();
}
