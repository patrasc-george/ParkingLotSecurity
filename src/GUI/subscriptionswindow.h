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
	void showVehicles(QListWidgetItem* item);

	void closeWindow();

private:
	QListWidget* subscriptionsListWidget;
	QListWidget* vehiclesListWidget;
	SubscriptionManager* subscriptionManager;
};
