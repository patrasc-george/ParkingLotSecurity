#include "mainwindow.h"
#include "ImageProcessingUtils.h"

#include <QFileDialog>
#include <QVBoxLayout>
#include <QPushButton>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QDebug>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
	setupUI();
}

void MainWindow::setupUI()
{
	graphicsView = new QGraphicsView(this);
	scene = new QGraphicsScene(this);
	graphicsView->setScene(scene);

	pixmapItem = new QGraphicsPixmapItem();
	scene->addItem(pixmapItem);

	uploadImageButton = new QPushButton("Upload Image");
	connect(uploadImageButton, &QPushButton::clicked, this, &MainWindow::uploadImage);

	setFixedSize(1200, 700);

	QVBoxLayout* mainLayout = new QVBoxLayout();
	mainLayout->addWidget(graphicsView);
	mainLayout->addWidget(uploadImageButton, 0, Qt::AlignHCenter | Qt::AlignBottom);

	centralWidget = new QWidget(this);
	centralWidget->setLayout(mainLayout);

	setCentralWidget(centralWidget);
}

void MainWindow::drawBBox()
{
	cv::Mat cvImage(image.height(), image.width(), CV_8UC4, const_cast<uchar*>(image.bits()), image.bytesPerLine());
	std::string text = textFromImage(cvImage, cvImage);
	image = QImage(cvImage.data, cvImage.cols, cvImage.rows, static_cast<int>(cvImage.step), QImage::Format_RGB888).copy();
	std::cout << text << std::endl;
}

void MainWindow::clearPreviousItems()
{
	QList<QGraphicsItem*> items = scene->items();
	for (QGraphicsItem* item : items)
	{
		scene->removeItem(item);
		delete item;
	}
}

void MainWindow::createNewPixmapItem(const QImage& image)
{
	pixmapItem = new QGraphicsPixmapItem(QPixmap::fromImage(image));
	scene->addItem(pixmapItem);
}

void MainWindow::setGraphicsViewProperties()
{
	graphicsView->setAlignment(Qt::AlignCenter);

	QRectF imageRect = pixmapItem->boundingRect();
	graphicsView->setSceneRect(imageRect);
	graphicsView->fitInView(imageRect, Qt::KeepAspectRatio);
}

void MainWindow::uploadImage()
{
	QString imagePath = QFileDialog::getOpenFileName(this, "Upload Image", "", "Images (*.png *.jpg *.bmp *.gif)");

	if (!imagePath.isEmpty())
	{
		image.load(imagePath);
		if (image.isNull())
			qDebug() << "Failed to load image\n";
		else
		{
			drawBBox();
			clearPreviousItems();
			createNewPixmapItem(image);
			setGraphicsViewProperties();
		}
	}
}

MainWindow::~MainWindow()
{
	delete pixmapItem;
	delete uploadImageButton;
	delete graphicsView;
	delete centralWidget;
	delete scene;
}
