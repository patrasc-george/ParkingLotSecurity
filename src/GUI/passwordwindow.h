#include <QLabel>
#include <QDialog>
#include <QLineEdit>

class PasswordWindow : public QDialog
{
	Q_OBJECT

public:
	PasswordWindow(const QSize& buttonSize, const std::string& dataBasePath, const bool& isPasswordWrong, std::string& password, QWidget* parent = nullptr);

private slots:
	void submit();

private:
	std::string& password;
	QLineEdit* passwordInput1;
	QLineEdit* passwordInput2;
	QLabel* errorLabel;
};
