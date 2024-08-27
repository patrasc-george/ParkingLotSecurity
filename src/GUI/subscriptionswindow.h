#include <QWidget>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>

class SubscriptionsWindow : public QWidget
{
	Q_OBJECT

public:
	SubscriptionsWindow(const std::unordered_map<std::string, std::vector<std::string>>& subscriptions, const QSize& buttonSize, QWidget* parent = nullptr);

public slots:
	void addSubscription();

	void addVehicle();

	void showVehicles(QListWidgetItem* item);

	void closeWindow();

protected:
	void closeEvent(QCloseEvent* event) override;

signals:
	void getSubscriptions(const std::unordered_map<std::string, std::vector<std::string>>& subscriptions);

private:
	QListWidget* subscriptionsListWidget;
	QListWidget* vehiclesListWidget;
	QLineEdit* subscriptionsEdit;
	QLineEdit* vehiclesEdit;
	QPushButton* addSubscriptionButton;
	QPushButton* addVehicleButton;
	std::unordered_map<std::string, std::vector<std::string>> subscriptions;
};