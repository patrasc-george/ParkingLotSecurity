#include "unpaidwindow.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QLabel>
#include <QPixmap>
#include <QMessageBox>

UnpaidWindow::UnpaidWindow(const QSize& buttonSize, QWidget* parent) : QDialog(parent)
{
	setWindowTitle(tr("Unpaid"));
	setWindowModality(Qt::ApplicationModal);

	QLabel* iconLabel = new QLabel(this);
	QPixmap errorIcon = QMessageBox::standardIcon(QMessageBox::Warning);
	iconLabel->setPixmap(errorIcon);

	QLabel* messageLabel = new QLabel(tr("The parking fee was not paid."), this);

	QPushButton* closeButton = new QPushButton(tr("Close"), this);

	closeButton->setFixedSize(buttonSize);

	QHBoxLayout* iconMessageLayout = new QHBoxLayout();
	iconMessageLayout->addWidget(iconLabel);
	iconMessageLayout->addWidget(messageLabel);
	iconMessageLayout->addStretch();

	QHBoxLayout* buttonLayout = new QHBoxLayout();
	buttonLayout->addStretch(1);
	buttonLayout->addWidget(closeButton);
	buttonLayout->addStretch(1);

	QVBoxLayout* mainLayout = new QVBoxLayout();
	mainLayout->addLayout(iconMessageLayout);
	mainLayout->addLayout(buttonLayout);

	setLayout(mainLayout);

	connect(closeButton, &QPushButton::clicked, this, &UnpaidWindow::reject);
}
