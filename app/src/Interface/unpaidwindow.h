#include <QDialog>
#include <QSize>

class UnpaidWindow : public QDialog
{
	Q_OBJECT

public:
	UnpaidWindow(const QSize& buttonSize, QWidget* parent = nullptr);
};
