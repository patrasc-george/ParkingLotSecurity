#include "uploadqrwindow.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QLabel>
#include <QPixmap>
#include <QMessageBox>

UploadQRWindow::UploadQRWindow(const QSize& buttonSize, QWidget* parent) : QDialog(parent)
{
	setWindowTitle("Upload QR");

	QLabel* iconLabel = new QLabel(this);
	QPixmap errorIcon = QMessageBox::standardIcon(QMessageBox::Warning);
	iconLabel->setPixmap(errorIcon);

	QLabel* messageLabel = new QLabel("The license plate number was not recognized. Please upload the QR code.", this);

	QPushButton* uploadButton = new QPushButton("Upload", this);
	QPushButton* cancelButton = new QPushButton("Cancel", this);

	uploadButton->setFixedSize(buttonSize);
	cancelButton->setFixedSize(buttonSize);

	QHBoxLayout* iconMessageLayout = new QHBoxLayout();
	iconMessageLayout->addWidget(iconLabel);
	iconMessageLayout->addWidget(messageLabel);
	iconMessageLayout->addStretch();

	QHBoxLayout* buttonLayout = new QHBoxLayout();
	buttonLayout->addStretch(1);
	buttonLayout->addWidget(uploadButton);
	buttonLayout->addWidget(cancelButton);
	buttonLayout->addStretch(1);

	QVBoxLayout* mainLayout = new QVBoxLayout();
	mainLayout->addLayout(iconMessageLayout);
	mainLayout->addLayout(buttonLayout);

	setLayout(mainLayout);

	connect(uploadButton, &QPushButton::clicked, this, &UploadQRWindow::upload);
	connect(cancelButton, &QPushButton::clicked, this, &UploadQRWindow::reject);
}

void UploadQRWindow::upload()
{
	QString path = QFileDialog::getOpenFileName(this, "Upload QR", "", "Image Files (*.png *.jpg *.bmp)");

	if (!path.isEmpty())
		emit getQRPath(path);

	this->close();
}
