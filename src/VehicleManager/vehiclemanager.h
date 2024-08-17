#pragma once

#ifdef VEHICLEMANAGER_EXPORTS
#define VEHICLEMANAGER_API __declspec(dllexport)
#else
#define VEHICLEMANAGER_API __declspec(dllimport)
#endif

#include "vehicle.h"
#include "ImageProcessingUtils.h"
#include "QRCodeUtils.h"

#include <fstream>
#include <vector>
#include <unordered_map>

class VEHICLEMANAGER_API VehicleManager
{
private:
	VehicleManager() = default;

public:
	~VehicleManager();

public:
	static VehicleManager& getInstance();

	void uploadDataBase(std::vector<std::string>& entranceDateTimes, std::vector<std::string>& exitDateTimes);

	void uploadVehicles(std::unordered_map<int, std::string>& entriesList, std::unordered_map<int, std::string>& exitsList);

	void setNumberOccupiedParkingLots(int& numberOccupiedParkingLots);

	void getVehicle(const std::string& imagePath, std::string& savePath);

	Vehicle findVehicle(const bool& direction = true, int index = -1);

	std::string timeParked();

	int calculateTotalAmount(const std::string& time, const int& fee);

	bool processLastVehicle(int& id, std::string& dateTime, std::string& displayText, const int& fee, const bool& pressedButton, const std::string& QRPath = "");

	void search(std::string text, std::unordered_map<int, std::string>& historyLogList);

	void calculateOccupancyStatistics(std::vector<std::pair<std::string, std::string>>& occupancyDateTimes);

	void pay(const std::string& licensePlate);

public:
	void setDataBasePath(const std::string& dataBasePath);

	std::string getImagePath(const int& id) const;

	void increaseOccupancyStatistics(const int& day, const int& hour);

	void increaseEntranceStatistics(const int& day, const int& hour);

	void increaseExitStatistics(const int& day, const int& hour);

	std::vector<std::vector<int>> getOccupancyStatistics() const;

	std::vector<std::vector<int>> getEntranceStatistics() const;

	std::vector<std::vector<int>> getExitStatistics() const;

private:
	std::string dataBasePath;
	std::ofstream writeFile;
	Vehicle curentVehicle;
	std::vector<Vehicle> vehicles;
	QRCode qr;
	std::vector<std::vector<int>> occupancyStatistics;
	std::vector<std::vector<int>> entranceStatistics;
	std::vector<std::vector<int>> exitStatistics;
	std::unordered_map<std::string, bool> vehiclesStatus;
};
