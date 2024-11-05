#pragma once

#ifdef DATABASEMANAGER_EXPORTS
#define DATABASEMANAGER_API __declspec(dllexport)
#else
#define DATABASEMANAGER_API __declspec(dllimport)
#endif

#include <sqlcipher/sqlite3.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

class DATABASEMANAGER_API DatabaseManager
{
private:
	DatabaseManager(const std::string& path) : path(path), db(nullptr) {};

	~DatabaseManager();

public:
	static DatabaseManager& getInstance(const std::string& path = "");

	bool initializeDatabase(const std::string& password);

	std::string getPath() const;

	std::vector<std::string> getVehicles() const;

	std::vector<std::string> getAccounts() const;

	std::unordered_map<std::string, std::pair<std::string, std::string>> getSubscriptions(const std::string& email) const;

	void setIsPaid(const int& id);

	void setName(const std::string& email, const std::string& newName);

	void setEmail(const std::string& email, const std::string& newEmail);

	void setPassword(const std::string& email, const std::string& newPassword);

	void setPhone(const std::string& email, const std::string& newPhone);

	void addVehicle(const int& id, const std::string& imagePath, const std::string& licensePlate, const std::string& dateTime, const std::string& ticket, const std::string& timeParked, const int& totalAmount, const std::string& isPaid);

	void addAccount(const std::string& name, const std::string& email, const std::string& password, const std::string& phone);

	void addSubscription(const std::string& email, const std::string& name);

	void addLicensePlate(const std::string& email, const std::string& name, const std::string& licensePlate);

	void deleteSubscription(const std::string& email, const std::string& name);

	void deleteLicensePlate(const std::string& email, const std::string& name, const std::string& licensePlate);

private:
	sqlite3* db;
	std::string path;
};
