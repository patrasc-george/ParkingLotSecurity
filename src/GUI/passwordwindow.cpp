#include "passwordwindow.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <filesystem>

PasswordWindow::PasswordWindow(const QSize& buttonSize, const std::string& dataBasePath, const bool& isPasswordWrong, std::string& password, QWidget* parent)
	: QDialog(parent),
	password(password),
	passwordInput1(nullptr),
	passwordInput2(nullptr)
{
	setWindowTitle(tr("Password"));
	setWindowModality(Qt::ApplicationModal);
	setFixedWidth(400);

	QVBoxLayout* layout = new QVBoxLayout(this);
	QLabel* messageLabel;

	errorLabel = new QLabel(this);
	errorLabel->setStyleSheet("color: red;");
	errorLabel->setAlignment(Qt::AlignCenter);

	if (std::filesystem::exists(dataBasePath + "/database.db"))
	{
		messageLabel = new QLabel(tr("Database was found."), this);
		messageLabel->setAlignment(Qt::AlignCenter);

		if (isPasswordWrong)
			errorLabel->setText(tr("The password is wrong. Please try again."));

		QLabel* passwordLabel1 = new QLabel(tr("Enter password:"), this);
		passwordInput1 = new QLineEdit(this);
		passwordInput1->setEchoMode(QLineEdit::Password);
		passwordInput1->setFixedHeight(buttonSize.height());

		layout->addWidget(messageLabel);
		layout->addWidget(errorLabel);
		layout->addWidget(passwordLabel1);
		layout->addWidget(passwordInput1);
	}
	else
	{
		messageLabel = new QLabel(tr("No database was found."), this);
		messageLabel->setAlignment(Qt::AlignCenter);

		QLabel* passwordLabel1 = new QLabel(tr("Enter password:"), this);
		passwordInput1 = new QLineEdit(this);
		passwordInput1->setEchoMode(QLineEdit::Password);
		passwordInput1->setFixedHeight(buttonSize.height());

		QLabel* passwordLabel2 = new QLabel(tr("Confirm password:"), this);
		passwordInput2 = new QLineEdit(this);
		passwordInput2->setEchoMode(QLineEdit::Password);
		passwordInput2->setFixedHeight(buttonSize.height());

		layout->addWidget(messageLabel);
		layout->addWidget(errorLabel);
		layout->addWidget(passwordLabel1);
		layout->addWidget(passwordInput1);
		layout->addWidget(passwordLabel2);
		layout->addWidget(passwordInput2);
	}

	QPushButton* submitButton = new QPushButton(tr("Submit"), this);
	submitButton->setFixedSize(buttonSize);
	layout->addWidget(submitButton);
	layout->setAlignment(submitButton, Qt::AlignCenter);

	connect(submitButton, &QPushButton::clicked, this, &PasswordWindow::submit);

	setLayout(layout);
}

void PasswordWindow::submit()
{
	errorLabel->clear();

	if (passwordInput1->text().isEmpty())
	{
		errorLabel->setText(tr("Please enter a password."));
		return;
	}

	if (passwordInput2)
	{
		if (passwordInput1->text() == passwordInput2->text())
		{
			password = passwordInput1->text().toStdString();
			accept();
		}
		else
			errorLabel->setText(tr("Passwords do not match. Please try again."));
	}
	else
	{
		password = passwordInput1->text().toStdString();
		accept();
	}
}
