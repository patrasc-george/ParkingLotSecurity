#pragma once

#ifdef VEHICLEMANAGER_EXPORTS
#define VEHICLEMANAGER_API __declspec(dllexport)
#else
#define VEHICLEMANAGER_API __declspec(dllimport)
#endif

#include "vehicle.h"
#include "licenseplatedetection.h"
#include "qrcodedetection.h"
#include "websocketclient.h"

#include <fstream>
#include <vector>

class VEHICLEMANAGER_API VehicleManager
{
public:
	VehicleManager();

public:
	void uploadDataBase(std::vector<std::string>& entranceDateTimes, std::vector<std::string>& exitDateTimes);

	void uploadVehicles(std::map<int, std::string>& entriesList, std::map<int, std::string>& exitsList);

	void setNumberOccupiedParkingLots(int& numberOccupiedParkingLots);

	void getVehicle(const std::string& imagePath, std::string& savePath);

	Vehicle* findVehicle(const std::string& licensePlate, const std::string& ticket, const bool& isEntered, const bool& direction = true, const int& index = -1);

	std::string timeParked();

	int calculateTotalAmount(const std::string& time, const int& fee);

	int processLastVehicle(int& id, std::string& dateTime, std::string& displayText, const int& fee, const bool& pressedButton, const std::string& QRPath = "");

	void search(std::string text, std::unordered_map<int, std::string>& historyLogList);

	void calculateOccupancyStatistics();

public:
	std::string getImagePath(const int& id) const;

	void setName(const std::string& name);

	void increaseOccupancyStatistics(const int& day, const int& hour);

	void increaseEntranceStatistics(const int& day, const int& hour);

	void increaseExitStatistics(const int& day, const int& hour);

	std::vector<std::vector<int>> getOccupancyStatistics() const;

	std::vector<std::vector<int>> getEntranceStatistics() const;

	std::vector<std::vector<int>> getExitStatistics() const;

private:
	Vehicle curentVehicle;
	std::vector<Vehicle> vehicles;
	QRCode qr;
	boost::asio::io_context ioContext;
	std::shared_ptr<WebSocketClient> client;
	std::string dataBasePath;
	std::string name;
	std::vector<std::vector<int>> occupancyStatistics;
	std::vector<std::vector<int>> entranceStatistics;
	std::vector<std::vector<int>> exitStatistics;
	std::unordered_map<std::string, bool> vehiclesStatus;
};
