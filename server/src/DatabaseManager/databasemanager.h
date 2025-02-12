#pragma once

#ifdef _WIN32
#ifdef DATABASEMANAGER_EXPORTS
#define DATABASEMANAGER_API __declspec(dllexport)
#else
#define DATABASEMANAGER_API __declspec(dllimport)
#endif
#elif __linux__
#define DATABASEMANAGER_API __attribute__((visibility("default")))
#else
#define DATABASEMANAGER_API
#endif

#include "logger.h"

#include <libpq-fe.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

class DATABASEMANAGER_API DatabaseManager
{
private:
	DatabaseManager() : logger(Logger::getInstance()) {};

	~DatabaseManager();

public:
	static DatabaseManager& getInstance();

	bool initializeDatabase();

	std::vector<std::string> getVehicles();

	std::string getLastVehicleActivity(const std::string& vehicleLicensePlate);

	std::string getTotalTimeParked(const std::string& vehicleLicensePlate);

	int getPayment(const std::string& vehicleLicensePlate);

	std::vector<std::string> getAccounts();

	std::unordered_set<std::string> getNewsletter();

	std::unordered_map<std::string, std::pair<std::string, std::string>> getSubscriptions(const std::string& email);

	std::vector<std::string> getVehicleHistory(const std::string& vehicleLicensePlate);

	bool getIsPaid(const std::string& vehicleLicensePlate);

	bool setIsPaid(const std::string& vehicle, std::string& licensePlate, std::string& dateTime, const bool& isTicket);

	void setName(const std::string& email, const std::string& newName);

	void setLastName(const std::string& email, const std::string& newLastName);

	void setEmail(const std::string& email, const std::string& newEmail);

	void setPassword(const std::string& email, const std::string& newPassword);

	void setPhone(const std::string& email, const std::string& newPhone);

	void addVehicle(const int& id, const std::string& imagePath, const std::string& licensePlate, const std::string& dateTime, const std::string& ticket, const std::string& timeParked, const std::string& totalAmount, const std::string& isPaid);

	void addAccount(const std::string& name, const std::string& lastName, const std::string& email, const std::string& password, const std::string& phone);

	void addSubscription(const std::string& email, const std::string& name);

	void addLicensePlate(const std::string& email, const std::string& name, const std::string& licensePlate);

	void subscribeNewsletter(const std::string& email);

	void unsubscribeNewsletter(const std::string& email);

	void deleteSubscription(const std::string& email, const std::string& name);

	void deleteLicensePlate(const std::string& email, const std::string& name, const std::string& licensePlate);

private:
	PGconn* conn;
	Logger& logger;
};
