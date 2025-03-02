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

/**
 * @class VehicleManager
 * @brief Manages the operations related to vehicles in the parking system.
 *
 * The VehicleManager class handles tasks related to uploading vehicle data, tracking vehicle entries and exits,
 * calculating parking fees, and managing occupancy statistics. It provides methods to process vehicle data,
 * calculate parking duration and fees, search for vehicles based on license plates, and manage occupancy statistics
 * for each hour of the week. It also supports the processing of vehicle images, ticket information, and QR code generation.
 * Additionally, the class allows for the management and tracking of parking lot capacity, as well as generating various reports.
 */
class VEHICLEMANAGER_API VehicleManager
{
public:
	/**
	 * @brief Constructor for the VehicleManager class.
	 * @details Initializes the WebSocket client based on the build configuration (debug or release)
	 *          and establishes the connection. Sets the database path accordingly.
	 */
	VehicleManager();

public:
	/**
	 * @brief Calculates the time a vehicle has been parked.
	 * @details This function finds the vehicle based on its license plate and ticket, and calculates the
	 *          difference between the vehicle's entry time and exit time. It returns the time the vehicle
	 *          has been parked in the format "HH:MM:SS".
	 * @return A string representing the parking duration (e.g., "02:30:45" or "undetected").
	 */
	std::string timeParked();

	/**
	 * @brief Uploads the vehicles data and updates entrance and exit times.
	 * @details This function fetches vehicle data from the WebSocket client and updates the entrance
	 *          and exit times. It also stores the vehicles' data for further processing.
	 * @param[out] entranceDateTimes A vector to store entrance date and time strings.
	 * @param[out] exitDateTimes A vector to store exit date and time strings.
	 * @return void
	 */
	void uploadDataBase(std::vector<std::string>& entranceDateTimes, std::vector<std::string>& exitDateTimes);

	/**
	 * @brief Uploads the vehicles' data to the entries and exits lists.
	 * @details This function iterates through the list of vehicles and categorizes them into either
	 *          the entries list or the exits list based on whether they are currently parked or not.
	 * @param[out] entriesList A map to store the entry vehicles' information.
	 * @param[out] exitsList A map to store the exit vehicles' information.
	 * @return void
	 */
	void uploadVehicles(std::map<int, std::string>& entriesList, std::map<int, std::string>& exitsList);

	/**
	 * @brief Sets the number of currently occupied parking lots.
	 * @details This function counts the number of vehicles that are currently occupying parking spaces
	 *          and updates the passed reference of the number of occupied parking lots.
	 * @param[out] numberOccupiedParkingLots The number of occupied parking lots.
	 * @return void
	 */
	void setNumberOccupiedParkingLots(int& numberOccupiedParkingLots);

	/**
	 * @brief Retrieves a vehicle's data based on the provided image path and saves the vehicle's image.
	 * @details This function processes the image to extract the vehicle's license plate and date-time
	 *          information. The vehicle's image is then saved to a predefined path.
	 * @param[in] imagePath The path to the image to be processed.
	 * @param[out] savePath The path where the vehicle's image will be saved.
	 * @return void
	 */
	void getVehicle(const std::string& imagePath, std::string& savePath);

	/**
	 * @brief Finds a vehicle based on its license plate, ticket, and parking status.
	 * @details This function searches for a vehicle in the list of vehicles based on various criteria
	 *          like the license plate, ticket, and whether the vehicle is currently parked or not.
	 * @param[in] licensePlate The license plate of the vehicle.
	 * @param[in] ticket The ticket information of the vehicle.
	 * @param[in] isEntered Flag to indicate if the vehicle has entered the parking area.
	 * @param[in] direction The direction of search (forward or backward).
	 * @param[in] index The index to start the search from.
	 * @return A pointer to the found vehicle, or nullptr if not found.
	 */
	Vehicle* findVehicle(const std::string& licensePlate, const std::string& ticket, const bool& isEntered, const bool& direction = true, const int& index = -1);

	/**
	 * @brief Calculates the total parking fee based on the time and fee rate.
	 * @details This function takes the parking duration (in hours) and calculates the total amount
	 *          based on the provided fee per hour. The function assumes that the parking time is
	 *          given in the "HH:MM:SS" format.
	 * @param[in] time The parking duration in the format "HH:MM:SS".
	 * @param[in] fee The fee per hour.
	 * @return The total amount to be charged, calculated as (hours * fee) + fee.
	 */
	int calculateTotalAmount(const std::string& time, const int& fee);

	/**
	 * @brief Processes the last vehicle in the parking system and calculates the parking fee.
	 * @details This function processes the details of the most recent vehicle, updating its ticket
	 *          information, calculating the parking duration and fee, and generating a QR code.
	 *          If a QR path is provided, it decodes the ticket and updates the vehicle information accordingly.
	 * @param[out] id The ID of the current vehicle.
	 * @param[out] dateTime The date and time associated with the current vehicle.
	 * @param[out] displayText A string containing the display information for the current vehicle.
	 * @param[in] fee The fee rate per hour.
	 * @param[in] pressedButton A flag indicating if the button has been pressed to process the vehicle.
	 * @param[in] QRPath The path to the QR code image, if applicable.
	 * @return An integer indicating the result of the operation:
	 *         1 - The vehicle has an unpaid status,
	 *         2 - The vehicle was not detected,
	 *         0 - The process was successful.
	 */
	int processLastVehicle(int& id, std::string& dateTime, std::string& displayText, const int& fee, const bool& pressedButton, const std::string& QRPath = "");

	/**
	 * @brief Searches for vehicles based on the provided text (typically license plate).
	 * @details This function searches for vehicles in the system whose license plates contain the
	 *          specified text. The search is case-insensitive. Matching vehicles are added to the
	 *          provided history log list with their details.
	 * @param[in] text The text to search for in the vehicle license plates.
	 * @param[out] historyLogList A map to store the results of the search, with vehicle IDs as keys
	 *                            and vehicle details (license plate, date, time parked, total amount) as values.
	 * @return void
	 */
	void search(std::string text, std::unordered_map<int, std::string>& historyLogList);

	/**
	 * @brief Calculates occupancy statistics based on vehicles' entry and exit times.
	 * @details This function iterates over all the vehicles to calculate the occupancy statistics
	 *          for each hour of the week. The occupancy is tracked by hour and day of the week.
	 *          It takes into account both the entry and exit time of the vehicles.
	 * @return void
	 */
	void calculateOccupancyStatistics();

public:
	/**
	 * @brief Returns the file path of the vehicle's image.
	 * @details This function returns the path where the image of the vehicle with the specified
	 *          ID is stored.
	 * @param[in] id The ID of the vehicle.
	 * @return The file path of the vehicle's image.
	 */
	std::string getImagePath(const int& id) const;

	/**
	 * @brief Sets the name for the vehicle manager.
	 * @details This function sets the name of the vehicle manager to the provided string.
	 * @param[in] name The name to be set for the vehicle manager.
	 * @return void
	 */
	void setName(const std::string& name);

	/**
	 * @brief Increases the occupancy statistics for the specified day and hour.
	 * @details This function increases the count of occupied parking spaces for the given day
	 *          and hour in the occupancy statistics.
	 * @param[in] day The day of the week (1-7).
	 * @param[in] hour The hour of the day (0-23).
	 * @return void
	 */
	void increaseOccupancyStatistics(const int& day, const int& hour);

	/**
	 * @brief Increases the entrance statistics for the specified day and hour.
	 * @details This function increases the count of vehicles that entered the parking lot
	 *          for the given day and hour.
	 * @param[in] day The day of the week (1-7).
	 * @param[in] hour The hour of the day (0-23).
	 * @return void
	 */
	void increaseEntranceStatistics(const int& day, const int& hour);

	/**
	 * @brief Increases the exit statistics for the specified day and hour.
	 * @details This function increases the count of vehicles that exited the parking lot
	 *          for the given day and hour.
	 * @param[in] day The day of the week (1-7).
	 * @param[in] hour The hour of the day (0-23).
	 * @return void
	 */
	void increaseExitStatistics(const int& day, const int& hour);

	/**
	 * @brief Retrieves the occupancy statistics for the week.
	 * @details This function returns the occupancy statistics, which indicate the number of
	 *          occupied parking spaces for each hour of each day in the week.
	 * @return A 2D vector containing occupancy statistics for each day and hour.
	 */
	std::vector<std::vector<int>> getOccupancyStatistics() const;

	/**
	 * @brief Retrieves the entrance statistics for the week.
	 * @details This function returns the entrance statistics, which track the number of vehicles
	 *          that entered the parking lot during each hour of each day in the week.
	 * @return A 2D vector containing entrance statistics for each day and hour.
	 */
	std::vector<std::vector<int>> getEntranceStatistics() const;

	/**
	 * @brief Retrieves the exit statistics for the week.
	 * @details This function returns the exit statistics, which track the number of vehicles
	 *          that exited the parking lot during each hour of each day in the week.
	 * @return A 2D vector containing exit statistics for each day and hour.
	 */
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
