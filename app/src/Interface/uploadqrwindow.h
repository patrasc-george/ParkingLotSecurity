#include <QDialog>
#include <QSize>

class UploadQRWindow : public QDialog
{
	Q_OBJECT

public:
	UploadQRWindow(const QSize& buttonSize, QWidget* parent = nullptr);

private slots:
	void upload();

signals:
	void getQRPath(const QString& path);
};
