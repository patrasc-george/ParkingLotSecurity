#include "vehiclemanager.h"

VehicleManager& VehicleManager::getInstance()
{
	static VehicleManager instance;
	return instance;
}

void VehicleManager::uploadDataBase(std::vector<std::string>& entranceDateTimes, std::vector<std::string>& exitDateTimes)
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
			curentVehicle = Vehicle(std::stoi(vehicleData[0]), vehicleData[1], vehicleData[2], vehicleData[3]);

			curentVehicle.setTicket(vehicleData[4]);

			if (vehicleData.size() == 6)
			{
				if (vehicleData[5] == "true")
					curentVehicle.setIsPaid();

				entranceDateTimes.push_back(vehicleData[4]);
			}
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

void VehicleManager::uploadSubscriptions()
{
	std::ifstream readFile(dataBasePath + "subscriptions.txt");

	std::vector<std::string> subscritionData;
	std::string line;
	while (std::getline(readFile, line))
		if (line.empty())
		{
			subscriptions[subscritionData[0]] = std::vector<std::string>();

			for (int i = 1; i < subscritionData.size(); i++)
				subscriptions[subscritionData[0]].push_back(subscritionData[i]);

			subscritionData.clear();
		}
		else
			subscritionData.push_back(line);
}

void VehicleManager::setNumberOccupiedParkingLots(int& numberOccupiedParkingLots)
{
	for (const auto& vehicle : vehiclesStatus)
		if (vehicle.second)
			numberOccupiedParkingLots++;
}

void VehicleManager::getVehicle(const std::string& imagePath, std::string& savePath)
{
	savePath = dataBasePath + "vehicles/" + std::to_string(vehicles.size()) + ".jpg";

	std::string text = textFromImage(imagePath, savePath);

	size_t plate = text.find('\n');
	size_t dateTime = text.find('\n', plate + 1);

	curentVehicle = Vehicle(vehicles.size(), savePath, text.substr(0, plate), text.substr(plate + 1, dateTime - plate - 1));
}

Vehicle* VehicleManager::findVehicle(const std::string& licensePlate, const std::string& ticket, const bool& isEntered, const bool& direction, const int& index)
{
	int startIndex = (index == -1) ? vehicles.size() : index;

	auto compareTickets = [&ticket](const Vehicle& vehicle) {
		return !ticket.empty() && ticket == vehicle.getTicket();
		};

	auto compareLicensePlates = [&licensePlate](const Vehicle& vehicle) {
		return licensePlate != "N/A" && licensePlate == vehicle.getLicensePlate();
		};

	int start = direction ? startIndex - 1 : startIndex + 1;
	int end = direction ? -1 : vehicles.size();
	int step = direction ? -1 : 1;

	for (int i = start; i != end; i += step)
	{
		bool isValid = (isEntered && vehicles[i].getTimeParked().empty()) || (!isEntered && !vehicles[i].getTimeParked().empty());

		if (isValid && (compareTickets(vehicles[i]) || compareLicensePlates(vehicles[i])))
			return &vehicles[i];
	}

	return nullptr;
}

std::string VehicleManager::timeParked()
{
	Vehicle* auxVehicle = findVehicle(curentVehicle.getLicensePlate(), curentVehicle.getTicket(), true);

	if (auxVehicle == nullptr)
	{
		if (curentVehicle.getTicket().empty())
			return "undetected";
		else
		{
			curentVehicle.setTicket("00-00-0000 00:00:00");
			return "00:00:00";
		}
	}

	if (!auxVehicle->getIsPaid())
		return "unpaid";

	if (auxVehicle->getLicensePlate() != "N/A")
		curentVehicle.setLicensePlate(auxVehicle->getLicensePlate());
	else
		auxVehicle->setLicensePlate(curentVehicle.getLicensePlate());
	curentVehicle.setTicket(auxVehicle->getTicket());

	std::ostringstream timeStream;
	std::tm tmIn = {}, tmOut = {};
	std::istringstream inStr(auxVehicle->getDateTime());
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

	curentVehicle.setTicket(auxVehicle->getTicket());
	return timeStream.str();
}

int VehicleManager::calculateTotalAmount(const std::string& time, const int& fee)
{
	int index = time.rfind(":");

	int hours = std::stoi(time.substr(0, index));

	return hours * fee + fee;
}

int VehicleManager::processLastVehicle(int& id, std::string& dateTime, std::string& displayText, const int& fee, const bool& pressedButton, const std::string& QRPath)
{
	id = curentVehicle.getId();
	dateTime = curentVehicle.getDateTime();
	displayText = curentVehicle.getLicensePlate() + "\n" + curentVehicle.getDateTime();

	if (pressedButton)
	{
		std::string time;

		if (QRPath.empty())
			time = timeParked();
		else
		{
			std::string ticket = qr.decodeQR(QRPath);
			curentVehicle.setTicket(ticket);

			time = timeParked();

			int rest = displayText.find('\n');
			displayText = curentVehicle.getLicensePlate() + displayText.substr(rest);
		}

		if (time == "unpaid")
			return 1;

		if (time == "undetected")
			return 2;

		int totalAmount = calculateTotalAmount(time, fee);

		displayText += '\n' + time + '\n' + std::to_string(totalAmount) + " RON";

		curentVehicle.setTimeParked(time);
		curentVehicle.setTotalAmount(totalAmount);

		qr.generateQR(curentVehicle.getTicket(), name, curentVehicle.getLicensePlate(), dataBasePath, curentVehicle.getDateTime(), time, totalAmount);
	}
	else
	{
		curentVehicle.setTicket(dateTime);

		qr.generateQR(dateTime, name, curentVehicle.getLicensePlate(), dataBasePath);
	}

	vehicles.push_back(curentVehicle);
	writeFile << curentVehicle;

	return 0;
}

void VehicleManager::search(std::string text, std::unordered_map<int, std::string>& historyLogList)
{
	if (text.empty())
		return;

	std::transform(text.begin(), text.end(), text.begin(), [](unsigned char c) { return std::toupper(c); });

	for (const auto& vehicle : vehicles)
		if (vehicle.getLicensePlate().find(text) != std::string::npos)
		{
			std::string displayText = vehicle.getLicensePlate() + "\n" + vehicle.getDateTime();
			if (!vehicle.getTimeParked().empty())
				displayText += '\n' + vehicle.getTimeParked() + '\n' + std::to_string(vehicle.getTotalAmount()) + " RON";

			historyLogList[vehicle.getId()] = displayText;
		}
}

void VehicleManager::calculateOccupancyStatistics(std::vector<std::pair<std::string, std::string>>& occupancyDateTimes)
{
	occupancyStatistics = std::vector<std::vector<int>>(7, std::vector<int>(24, 0));

	for (int i = 0; i < vehicles.size(); i++)
		if (vehicles[i].getTimeParked().empty())
		{
			Vehicle* auxVehicle = findVehicle(vehicles[i].getLicensePlate(), vehicles[i].getTicket(), false, false, i);

			if (auxVehicle != nullptr)
				occupancyDateTimes.push_back(std::make_pair(vehicles[i].getDateTime(), auxVehicle->getDateTime()));
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

bool VehicleManager::pay(const std::string& vehicle, std::string& licensePlate, std::string& dateTime, const bool& isTicket)
{
	curentVehicle = Vehicle();

	Vehicle* auxVehicle;

	if (isTicket)
	{
		std::string ticket = qr.decodeQR(vehicle);
		auxVehicle = findVehicle("N/A", ticket, true);
	}
	else
		auxVehicle = findVehicle(vehicle, "", true);

	if (auxVehicle == nullptr)
		return false;

	auxVehicle->setIsPaid();
	licensePlate = auxVehicle->getLicensePlate();
	dateTime = auxVehicle->getDateTime();

	std::ifstream readFile(dataBasePath + "vehicles.txt");
	std::vector<std::string> lines;
	std::string line;
	while (std::getline(readFile, line))
		lines.push_back(line);

	auto it = std::find(lines.begin(), lines.end(), std::to_string(auxVehicle->getId()));
	int index = std::distance(lines.begin(), it) + 5;
	lines[index] = "true";

	writeFile.close();
	writeFile.open(dataBasePath + "vehicles.txt", std::ios::out | std::ios::trunc);
	for (const auto& line : lines)
		writeFile << line << std::endl;

	return true;
}

std::string VehicleManager::getDataBasePath() const
{
	return dataBasePath;
}

void VehicleManager::setDataBasePath(const std::string& dataBasePath)
{
	this->dataBasePath = dataBasePath;
}

std::string VehicleManager::getImagePath(const int& id) const
{
	return vehicles[id].getPath();
}

void VehicleManager::setName(const std::string& name)
{
	this->name = name;
}

std::unordered_map<std::string, std::vector<std::string>> VehicleManager::getSubscriptions() const
{
	return subscriptions;
}

void VehicleManager::setSubscriptions(const std::unordered_map<std::string, std::vector<std::string>>& subscriptions)
{
	this->subscriptions = subscriptions;

	std::ofstream subscriptionsFile(dataBasePath + "subscriptions.txt", std::ios::out | std::ios::trunc);

	for (const auto& subscription : subscriptions)
	{
		subscriptionsFile << subscription.first << std::endl;

		for (const auto& vehicle : subscription.second)
			subscriptionsFile << vehicle << std::endl;

		subscriptionsFile << std::endl;
	}
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