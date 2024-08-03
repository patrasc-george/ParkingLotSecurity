#include "vehiclemanager.h"

void VehicleManager::uploadDataBase(const std::string& dataBasePath, std::vector<std::string>& entranceDateTimes, std::vector<std::string>& exitDateTimes)
{
	writeFile = std::ofstream(dataBasePath + "vehicles.txt", std::ios::app);
	std::ifstream readFile(dataBasePath + "vehicles.txt");

	entranceStatistics = std::vector<std::vector<int>>(7, std::vector<int>(24, 0));
	exitStatistics = std::vector<std::vector<int>>(7, std::vector<int>(24, 0));

	std::vector<std::string> vehicleData;
	std::string line;
	while (std::getline(readFile, line))
	{
		if (line.empty())
		{
			curentVehicle = Vehicle(std::stoi(vehicleData[0]), vehicleData[1], std::stoi(vehicleData[2]), vehicleData[3], vehicleData[4]);

			if (vehicleData.size() == 5)
				entranceDateTimes.push_back(vehicleData[4]);
			else
			{
				curentVehicle.setTimeParked(vehicleData[5]);
				curentVehicle.setTotalAmount(std::stoi(vehicleData[6]));
				exitDateTimes.push_back(vehicleData[4]);
			}

			vehicles.push_back(curentVehicle);
			vehicleData.clear();
		}
		else
			vehicleData.push_back(line);
	}

	readFile.close();
}

void VehicleManager::uploadVehicles(std::unordered_map<int, std::string>& entriesList, std::unordered_map<int, std::string>& exitsList)
{
	for (const auto& vehicle : vehicles)
	{
		curentVehicle = vehicle;

		std::string displayText = curentVehicle.getLicensePlate() + "\n" + curentVehicle.getDateTime();

		if (!curentVehicle.getTimeParked().empty())
			displayText += '\n' + curentVehicle.getTimeParked() + '\n' + std::to_string(curentVehicle.getTotalAmount()) + " RON";

		if (curentVehicle.getTimeParked().empty())
		{
			entriesList[curentVehicle.getId()] = displayText;
			vehiclesStatus[curentVehicle.getTicket()] = true;
		}
		else
		{
			exitsList[curentVehicle.getId()] = displayText;
			vehiclesStatus[curentVehicle.getTicket()] = false;
		}
	}
}

void VehicleManager::setNumberOccupiedParkingLots(int& numberOccupiedParkingLots)
{
	for (const auto& vehicle : vehiclesStatus)
	{
		if (vehicle.second)
			numberOccupiedParkingLots++;
	}
}

void VehicleManager::getVehicle(const std::string& imagePath, std::string& savePath, const std::string& dataBasePath)
{
	savePath = dataBasePath + "vehicles/" + std::to_string(vehicles.size()) + ".jpg";

	std::string text = textFromImage(imagePath, savePath);

	size_t plate = text.find('\n');
	size_t dateTime = text.find('\n', plate + 1);

	curentVehicle = Vehicle(vehicles.size(), savePath, 0, text.substr(0, plate), text.substr(plate + 1, dateTime - plate - 1));
}

Vehicle VehicleManager::findVehicle(const bool& direction, int index)
{
	Vehicle result = Vehicle();

	if (index == -1)
		index = vehicles.size();

	if (direction)
	{
		if (curentVehicle.getLicensePlate() != "N/A")
			for (int i = index - 1; i >= 0; i--)
			{
				if (curentVehicle.getLicensePlate() == vehicles[i].getLicensePlate())
					return vehicles[i];
			}

		if (curentVehicle.getTicket())
			for (int i = index - 1; i >= 0; i--)
			{
				if (curentVehicle.getTicket() == vehicles[i].getTicket())
					return vehicles[i];
			}
	}
	else
	{
		if (curentVehicle.getLicensePlate() != "N/A")
			for (int i = index + 1; i < vehicles.size(); i++)
			{
				if (curentVehicle.getLicensePlate() == vehicles[i].getLicensePlate())
					return vehicles[i];
			}

		if (curentVehicle.getTicket())
			for (int i = index + 1; i < vehicles.size(); i++)
			{
				if (curentVehicle.getTicket() == vehicles[i].getTicket())
					return vehicles[i];
			}
	}

	return Vehicle();
}

std::string VehicleManager::timeParked()
{
	Vehicle auxVehicle = findVehicle();

	if (auxVehicle.getLicensePlate() == "")
		return "00:00:00";

	std::ostringstream timeStream;
	std::tm tmIn = {}, tmOut = {};
	std::istringstream inStr(auxVehicle.getDateTime());
	std::istringstream outStr(curentVehicle.getDateTime());
	inStr >> std::get_time(&tmIn, "%d-%m-%Y %H:%M:%S");
	outStr >> std::get_time(&tmOut, "%d-%m-%Y %H:%M:%S");
	auto inTime = std::mktime(&tmIn);
	auto outTime = std::mktime(&tmOut);

	if (inTime != -1 && outTime != -1)
	{
		auto duration = std::difftime(outTime, inTime);

		int hours = static_cast<int>(duration) / 3600;
		int minutes = (static_cast<int>(duration) % 3600) / 60;
		int seconds = static_cast<int>(duration) % 60;

		timeStream << std::setw(2) << std::setfill('0') << hours << ":"
			<< std::setw(2) << std::setfill('0') << minutes << ":"
			<< std::setw(2) << std::setfill('0') << seconds;
	}

	return timeStream.str();
}

int VehicleManager::calculateTotalAmount(const std::string& time, const int& fee)
{
	int index = time.rfind(":");

	int hours = std::stoi(time.substr(0, index));

	return hours * fee + fee;
}

void VehicleManager::processLastVehicle(int& id, std::string& dateTime, std::string& displayText, int& numberOccupiedParkingLots, const int& fee, const bool& pressedButton)
{
	id = curentVehicle.getId();
	dateTime = curentVehicle.getDateTime();
	displayText = curentVehicle.getLicensePlate() + "\n" + curentVehicle.getDateTime();

	if (pressedButton)
	{
		std::string time = timeParked();
		int totalAmount = calculateTotalAmount(time, fee);

		displayText += '\n' + time + '\n' + std::to_string(totalAmount) + " RON";

		curentVehicle.setTimeParked(time);
		curentVehicle.setTotalAmount(totalAmount);

		numberOccupiedParkingLots--;
	}
	else
	{

		numberOccupiedParkingLots++;
	}

	vehicles.push_back(curentVehicle);
	writeFile << curentVehicle;
}

void VehicleManager::search(std::string text, std::unordered_map<int, std::string>& historyLogList)
{
	if (text == "")
		return;

	std::transform(text.begin(), text.end(), text.begin(), [](unsigned char c) { return std::toupper(c); });

	for (const auto& vehicle : vehicles)
		if (vehicle.getLicensePlate().find(text) != std::string::npos)
		{
			std::string displayText = vehicle.getLicensePlate() + "\n" + vehicle.getDateTime();
			if (vehicle.getTimeParked() != "")
				displayText += '\n' + vehicle.getTimeParked() + '\n' + std::to_string(vehicle.getTotalAmount()) + " RON";

			historyLogList[vehicle.getId()] = displayText;
		}
}

void VehicleManager::calculateOccupancyStatistics(std::vector<std::pair<std::string, std::string>>& occupancyDateTimes)
{
	occupancyStatistics = std::vector<std::vector<int>>(7, std::vector<int>(24, 0));

	for (int i = 0; i < vehicles.size(); i++)
		if (vehicles[i].getTimeParked() == "")
		{
			curentVehicle = vehicles[i];
			Vehicle auxVehicle = findVehicle(false, i);

			if (auxVehicle.getLicensePlate() != "")
				occupancyDateTimes.push_back(std::make_pair(vehicles[i].getDateTime(), auxVehicle.getDateTime()));
			else
			{
				auto now = std::chrono::system_clock::now();
				auto toTimeT = std::chrono::system_clock::to_time_t(now);

				std::stringstream ss;
				ss << std::put_time(std::localtime(&toTimeT), "%d-%m-%Y %X");

				occupancyDateTimes.push_back(std::make_pair(vehicles[i].getDateTime(), ss.str()));
			}
		}
}

std::string VehicleManager::getImagePath(const int& id) const
{
	return vehicles[id].getPath();
}

void VehicleManager::increaseOccupancyStatistics(const int& day, const int& hour)
{
	occupancyStatistics[day - 1][hour]++;
}

void VehicleManager::increaseEntranceStatistics(const int& day, const int& hour)
{
	entranceStatistics[day - 1][hour]++;
}

void VehicleManager::increaseExitStatistics(const int& day, const int& hour)
{
	exitStatistics[day - 1][hour]++;
}

std::vector<std::vector<int>> VehicleManager::getOccupancyStatistics() const
{
	return occupancyStatistics;
}

std::vector<std::vector<int>> VehicleManager::getEntranceStatistics() const
{
	return entranceStatistics;
}

std::vector<std::vector<int>> VehicleManager::getExitStatistics() const
{
	return exitStatistics;
}

VehicleManager::~VehicleManager()
{
	writeFile.close();
}