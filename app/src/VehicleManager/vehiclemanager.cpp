#include "vehiclemanager.h"

#include <iomanip>

VehicleManager::VehicleManager()
{
#ifdef _DEBUG
	client = std::make_shared<WebSocketClient>(ioContext, std::getenv("WEBSOCKET_URL_DEBUG"));
#else
	client = std::make_shared<WebSocketClient>(ioContext, std::getenv("WEBSOCKET_URL"));
#endif

#ifdef _DEBUG
	dataBasePath = "../../../database/";
#else
	dataBasePath = "database/";
#endif

	client->connect();
}

inline std::vector<std::string> split(const std::string& string, const std::string& delimiter)
{
	if (string.empty())
		return std::vector<std::string>();

	std::vector<std::string> substrings;
	std::string::size_type start = 0;
	std::string::size_type end = string.find(delimiter);

	while (end != std::string::npos)
	{
		substrings.push_back(string.substr(start, end - start));
		start = end + delimiter.length();
		end = string.find(delimiter, start);
	}

	substrings.push_back(string.substr(start));
	return substrings;
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

	if (!client->getIsPaid(curentVehicle.getLicensePlate()))
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

void VehicleManager::uploadDataBase(std::vector<std::string>& entranceDateTimes, std::vector<std::string>& exitDateTimes)
{
	entranceStatistics = std::vector<std::vector<int>>(7, std::vector<int>(24, 0));
	exitStatistics = std::vector<std::vector<int>>(7, std::vector<int>(24, 0));

	std::vector<std::string> vehiclesData = client->getVehicles();
	for (const auto& vehicleData : vehiclesData)
	{
		std::vector<std::string> data = split(vehicleData, ", ");

		int id = std::stoi(data[0]) - 1;
		std::string path = dataBasePath + "vehicles/" + std::to_string(id) + ".jpg";
		curentVehicle = Vehicle(id, path, data[1], data[2]);
		curentVehicle.setTicket(data[3]);

		if (data[2] == data[3])
		{
			if (data[5] == "1")
				curentVehicle.setIsPaid();

			entranceDateTimes.push_back(data[2]);
		}
		else
		{
			curentVehicle.setTimeParked(timeParked());
			curentVehicle.setTotalAmount(std::stoi(data[4]));

			exitDateTimes.push_back(data[2]);
		}

		vehicles.push_back(curentVehicle);
	}
}

void VehicleManager::uploadVehicles(std::map<int, std::string>& entriesList, std::map<int, std::string>& exitsList)
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
		if (vehicle.second)
			numberOccupiedParkingLots++;
}

void VehicleManager::getVehicle(const std::string& imagePath, std::string& savePath)
{
	savePath = dataBasePath + "vehicles/" + std::to_string(vehicles.size()) + ".jpg";

	std::string text = textFromImage(imagePath, savePath);

	int plate = text.find('\n');
	int dateTime = text.find('\n', plate + 1);

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
	client->addVehicle(curentVehicle.getLicensePlate(), curentVehicle.getDateTime(), curentVehicle.getTicket(), curentVehicle.getTotalAmount());
	vehicles.push_back(curentVehicle);

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

std::chrono::system_clock::time_point parseDateTime(const std::string& dateTime) {
	std::tm timeStruct = {};
	std::istringstream ss(dateTime);
	ss >> std::get_time(&timeStruct, "%d-%m-%Y %H:%M:%S");

	time_t timeT = std::mktime(&timeStruct);
	return std::chrono::system_clock::from_time_t(timeT);
}

void VehicleManager::calculateOccupancyStatistics()
{
	occupancyStatistics = std::vector<std::vector<int>>(7, std::vector<int>(24, 0));

	for (int i = 0; i < vehicles.size(); i++)
		if (vehicles[i].getTimeParked().empty())
		{
			Vehicle* auxVehicle = findVehicle(vehicles[i].getLicensePlate(), vehicles[i].getTicket(), false, false, i);

			std::string firstDateTime = vehicles[i].getDateTime();
			std::string secondDateTime;
			if (auxVehicle != nullptr)
				secondDateTime = auxVehicle->getDateTime();
			else
			{
				auto now = std::chrono::system_clock::now();
				auto toTimeT = std::chrono::system_clock::to_time_t(now);

				std::tm* timeStruct = std::localtime(&toTimeT);

				timeStruct->tm_hour += 1;
				timeStruct->tm_min = 0;
				timeStruct->tm_sec = 0;

				toTimeT = std::mktime(timeStruct);

				std::stringstream ss;
				ss << std::put_time(timeStruct, "%d-%m-%Y %X");

				secondDateTime = ss.str();
			}

			std::tm startTime = {};
			std::tm endTime = {};

			std::istringstream startStream(firstDateTime);
			std::istringstream endStream(secondDateTime);

			startStream >> std::get_time(&startTime, "%d-%m-%Y %H:%M:%S");
			endStream >> std::get_time(&endTime, "%d-%m-%Y %H:%M:%S");

			auto start = std::chrono::system_clock::from_time_t(std::mktime(&startTime));
			auto end = std::chrono::system_clock::from_time_t(std::mktime(&endTime));

			while (start <= end)
			{
				std::time_t currentTimeT = std::chrono::system_clock::to_time_t(start);
				std::tm* currentTime = std::localtime(&currentTimeT);

				int day = currentTime->tm_wday;
				int hour = currentTime->tm_hour;

				day = (day == 0) ? 7 : day;

				increaseOccupancyStatistics(day, hour);

				start += std::chrono::hours(1);
			}
		}
}

std::string VehicleManager::getImagePath(const int& id) const
{
	return vehicles[id].getPath();
}

void VehicleManager::setName(const std::string& name)
{
	this->name = name;
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
