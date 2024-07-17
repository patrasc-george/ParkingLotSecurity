#include "statisticswindow.h"

#include <QHeaderView>
#include <QBrush>
#include <QLabel>
#include <QComboBox>

StatisticsWindow::StatisticsWindow(const std::vector<std::vector<int>>& occupancyStatistics, const std::vector<std::vector<int>>& enterStatistics, const std::vector<std::vector<int>>& exitStatistics, QWidget* parent) :
	occupancyStatistics(occupancyStatistics),
	enterStatistics(enterStatistics),
	exitStatistics(exitStatistics),
	QWidget(parent)
{
	setWindowTitle("Statistics");
	setFixedSize(1000, 300);

	setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);

	layout = new QVBoxLayout(this);
	table = new QTableWidget(7, 24, this);

	QComboBox* chooseTable = new QComboBox(this);
	chooseTable->addItem("Occupancy");
	chooseTable->addItem("Entries");
	chooseTable->addItem("Exits");
	chooseTable->setFixedSize(100, 30);

	QPushButton* closeButton = new QPushButton("Close", this);
	closeButton->setFixedSize(100, 30);

	layout->addWidget(chooseTable);
	layout->addWidget(table);
	layout->addWidget(closeButton, 0, Qt::AlignCenter);

	setTable(0);

	connect(chooseTable, &QComboBox::currentIndexChanged, this, &StatisticsWindow::setTable);
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

			QTableWidgetItem* item = new QTableWidgetItem(QString::number(value, 'f', 2));
			item->setBackground(QBrush(color));
			item->setFlags(Qt::NoItemFlags);
			item->setTextAlignment(Qt::AlignLeft | Qt::AlignBottom);
			table->setItem(i + 1, j + 1, item);
		}
}

void StatisticsWindow::setTable(const int& choise)
{
	std::vector<std::vector<int>> statistics;
	switch (choise)
	{
	case 0:
		statistics = occupancyStatistics;
		break;
	case 1:
		statistics = enterStatistics;
		break;
	case 2:
		statistics = exitStatistics;
		break;
	}

	std::vector<std::vector<double>> normalizedStatistics(7, std::vector<double>(24, 0.0));
	normalize(statistics, normalizedStatistics);

	setupTable(table);
	applyColor(table, normalizedStatistics);
}
