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

	void uploadSubscriptions();

	void setNumberOccupiedParkingLots(int& numberOccupiedParkingLots);

	void getVehicle(const std::string& imagePath, std::string& savePath);

	Vehicle* findVehicle(const std::string& licensePlate, const std::string& ticket, const bool& isEntered, const bool& direction = true, const int& index = -1);

	std::string timeParked();

	int calculateTotalAmount(const std::string& time, const int& fee);

	int processLastVehicle(int& id, std::string& dateTime, std::string& displayText, const int& fee, const bool& pressedButton, const std::string& QRPath = "");

	void search(std::string text, std::unordered_map<int, std::string>& historyLogList);

	void calculateOccupancyStatistics(std::vector<std::pair<std::string, std::string>>& occupancyDateTimes);

	bool pay(const std::string& vehicle, std::string& licensePlate, std::string& dateTime, const bool& isTicket = false);

public:
	std::string getDataBasePath() const;

	void setDataBasePath(const std::string& dataBasePath);

	std::string getImagePath(const int& id) const;

	void setName(const std::string& name);

	std::unordered_map<std::string, std::vector<std::string>> getSubscriptions() const;

	void setSubscriptions(const std::unordered_map<std::string, std::vector<std::string>>& subscriptions);

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
	std::string name;
	std::unordered_map<std::string, std::vector<std::string>> subscriptions;
	std::vector<std::vector<int>> occupancyStatistics;
	std::vector<std::vector<int>> entranceStatistics;
	std::vector<std::vector<int>> exitStatistics;
	std::unordered_map<std::string, bool> vehiclesStatus;
};
