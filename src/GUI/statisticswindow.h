#pragma once

#include <vector>
#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>

class StatisticsWindow : public QWidget
{
	Q_OBJECT

public:
	StatisticsWindow(const std::vector<std::vector<int>>& occupancyStatistics, const std::vector<std::vector<int>>& entriesStatistics, const std::vector<std::vector<int>>& exitStatistics, QWidget* parent = nullptr);

private slots:
	void setTable(const int& choice);

private:
	void normalize(const std::vector<std::vector<int>>& data, std::vector<std::vector<double>>& normalizedData);

	void setupTable(QTableWidget* table);

	void applyColor(QTableWidget* table, const std::vector<std::vector<double>>& normalizedData);

private:
	QVBoxLayout* layout;
	QTableWidget* table;
	std::vector<std::vector<int>> occupancyStatistics;
	std::vector<std::vector<int>> entriesStatistics;
	std::vector<std::vector<int>> exitStatistics;
};
