#include "subscriptionmanager.h"

#include <QWidget>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>

class SubscriptionsWindow : public QWidget
{
	Q_OBJECT

public:
	SubscriptionsWindow(SubscriptionManager* subscriptionManager, const QSize& buttonSize, QWidget* parent = nullptr);

public slots:
	void showSubscriptions(QListWidgetItem* item);

	void showVehicles(QListWidgetItem* accountsItem, QListWidgetItem* subscriptionItem);

	void closeWindow();

private:
	QListWidget* accountsListWidget;
	QListWidget* subscriptionsListWidget;
	QListWidget* vehiclesListWidget;
	SubscriptionManager* subscriptionManager;
};
