#pragma once

#include <QMainWindow>
#include <QPushButton>

class QGraphicsView;
class QGraphicsScene;
class QGraphicsPixmapItem;

class MainWindow : public QMainWindow
{
	Q_OBJECT

private:
	QPushButton* uploadImageButton;
	QGraphicsView* graphicsView;
	QGraphicsScene* scene;
	QGraphicsPixmapItem* pixmapItem;
	QWidget* centralWidget;
	QImage image;

public:
	MainWindow(QWidget* parent = nullptr);
	~MainWindow();

private slots:
	void uploadImage();

private:
	void setupUI();
	void drawBBox();
	void clearPreviousItems();
	void createNewPixmapItem(const QImage& image);
	void setGraphicsViewProperties();
};
