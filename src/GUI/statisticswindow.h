#pragma once

#include <vector>
#include <QWidget>
#include <QTableWidget>
#include <QPushButton>

class StatisticsWindow : public QWidget
{
	Q_OBJECT

public:
	StatisticsWindow(const std::vector<std::vector<int>>& enterStatistics, const std::vector<std::vector<int>>& exitStatistics, QWidget* parent = nullptr);

private:
	void normalize(const std::vector<std::vector<int>>& data, std::vector<std::vector<double>>& normalizedData);
	void setupTable(QTableWidget* table);
	void applyColor(QTableWidget* table, const std::vector<std::vector<double>>& normalizedData);

private:
	QTableWidget* enterTable;
	QTableWidget* exitTable;
	std::vector<std::vector<double>> normalizedEnterStatistics;
	std::vector<std::vector<double>> normalizedExitStatistics;
};
