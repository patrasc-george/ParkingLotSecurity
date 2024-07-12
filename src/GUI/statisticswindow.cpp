#include "statisticswindow.h"

#include <QVBoxLayout>
#include <QHeaderView>
#include <QBrush>
#include <QLabel>

StatisticsWindow::StatisticsWindow(const std::vector<std::vector<int>>& enterStatistics, const std::vector<std::vector<int>>& exitStatistics, QWidget* parent) : QWidget(parent)
{
	setWindowTitle("Statistics");
	setFixedSize(1000, 500);

	setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);

	enterTable = new QTableWidget(7, 24, this);
	exitTable = new QTableWidget(7, 24, this);

	normalizedEnterStatistics = std::vector<std::vector<double>>(7, std::vector<double>(24, 0));
	normalizedExitStatistics = std::vector<std::vector<double>>(7, std::vector<double>(24, 0));

	normalize(enterStatistics, normalizedEnterStatistics);
	normalize(exitStatistics, normalizedExitStatistics);

	QVBoxLayout* layout = new QVBoxLayout(this);

	QLabel* enterLabel = new QLabel("Entries", this);
	QLabel* exitLabel = new QLabel("Exits", this);

	enterLabel->setAlignment(Qt::AlignCenter);
	exitLabel->setAlignment(Qt::AlignCenter);

	layout->addWidget(enterLabel);
	setupTable(enterTable);
	applyColor(enterTable, normalizedEnterStatistics);
	layout->addWidget(enterTable);

	layout->addWidget(exitLabel);
	setupTable(exitTable);
	applyColor(exitTable, normalizedExitStatistics);
	layout->addWidget(exitTable);

	QPushButton* closeButton = new QPushButton("Close", this);
	closeButton->setFixedSize(100, 30);

	layout->addWidget(closeButton);
	layout->setAlignment(closeButton, Qt::AlignCenter);

	connect(closeButton, &QPushButton::clicked, this, &StatisticsWindow::close);

	setLayout(layout);
}

void StatisticsWindow::normalize(const std::vector<std::vector<int>>& statistics, std::vector<std::vector<double>>& normalizedStatistics)
{
	int maxValue = 0;
	for (const auto& row : statistics)
		for (int value : row)
			if (value > maxValue)
				maxValue = value;

	if (maxValue == 0)
		return;

	normalizedStatistics.resize(statistics.size(), std::vector<double>(statistics[0].size(), 0));
	for (int i = 0; i < statistics.size(); i++)
		for (int j = 0; j < statistics[i].size(); j++)
			normalizedStatistics[i][j] = static_cast<double>(statistics[i][j]) / maxValue;
}

void StatisticsWindow::setupTable(QTableWidget* table)
{
	QStringList days = { "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday" };
	QStringList hours;
	for (int i = 0; i < 24; ++i)
		hours << QString::number(i);

	table->setColumnCount(25);
	table->setRowCount(8);

	table->horizontalHeader()->setVisible(false);
	table->verticalHeader()->setVisible(false);

	table->setStyleSheet("QTableWidget { border: none; background-color: transparent; gridline-color: transparent; }");
	table->setShowGrid(false);

	table->setColumnWidth(0, 70);
	table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	table->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);

	table->setSelectionMode(QAbstractItemView::NoSelection);
	table->setFocusPolicy(Qt::NoFocus);

	for (int i = 0; i < 24; i++)
	{
		QTableWidgetItem* item = new QTableWidgetItem(hours[i]);
		item->setFlags(Qt::NoItemFlags);
		item->setTextAlignment(Qt::AlignCenter);
		table->setItem(0, i + 1, item);
	}

	for (int i = 0; i < 7; i++)
	{
		QTableWidgetItem* item = new QTableWidgetItem(days[i]);
		item->setFlags(Qt::NoItemFlags);
		item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		table->setItem(i + 1, 0, item);
	}
}

void StatisticsWindow::applyColor(QTableWidget* table, const std::vector<std::vector<double>>& normalizedStatistics)
{
	for (int i = 0; i < normalizedStatistics.size(); i++)
		for (int j = 0; j < normalizedStatistics[i].size(); j++)
		{
			double value = normalizedStatistics[i][j];
			int blueValue = static_cast<int>(255 * value);
			QColor color(255 - blueValue, 255 - blueValue, 255);

			QTableWidgetItem* item = new QTableWidgetItem();
			item->setBackground(QBrush(color));
			item->setFlags(Qt::NoItemFlags);
			table->setItem(i + 1, j + 1, item);
		}
}

//void StatisticsWindow::closeWindow()
//{
//    this->close();
//}