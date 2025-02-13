#include "databasemanager.h"

#include <sys/stat.h>
#include <ctime>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <algorithm>

bool DatabaseManager::initializeDatabase()
{
	logger.setLogOutput(CONSOLE);

	LOG_MESSAGE(INFO) << "The program has been launched." << std::endl;

#ifdef _DEBUG
	conn = PQconnectdb(std::getenv("DATABASE_URL_DEBUG"));
#else
	conn = PQconnectdb(std::getenv("DATABASE_URL"));
#endif

	if (PQstatus(conn) != CONNECTION_OK)
	{
		LOG_MESSAGE(CRITICAL) << "Failed to connect to the database." << std::endl;
		PQfinish(conn);
		return false;
	}

	const char* sqlCreateTables = R"(
		CREATE TABLE IF NOT EXISTS vehicles (
			id SERIAL PRIMARY KEY,
			image_path TEXT NOT NULL,
			license_plate TEXT NOT NULL,
			date_time TEXT NOT NULL,
			ticket TEXT NOT NULL,
			time_parked TEXT,
			total_amount REAL,
			is_paid TEXT NOT NULL
		);
		CREATE TABLE IF NOT EXISTS accounts (
			email TEXT PRIMARY KEY,
			name TEXT NOT NULL,
			last_name TEXT NOT NULL,
			password TEXT NOT NULL,
			phone TEXT NOT NULL
		);
		CREATE TABLE IF NOT EXISTS payments (
			id SERIAL PRIMARY KEY,
			date DATE NOT NULL
		);
		CREATE TABLE IF NOT EXISTS license_plates (
			id SERIAL PRIMARY KEY,
			number TEXT NOT NULL
		);
		CREATE TABLE IF NOT EXISTS subscription_payments (
			email TEXT,
			subscription_name TEXT,
			payment_id INTEGER,
			PRIMARY KEY (subscription_name, email, payment_id),
			FOREIGN KEY (payment_id) REFERENCES payments(id) ON DELETE CASCADE
		);
		CREATE TABLE IF NOT EXISTS subscription_license_plates (
			email TEXT,
			subscription_name TEXT,
			license_plate_id INTEGER,
			PRIMARY KEY (subscription_name, email, license_plate_id),
			FOREIGN KEY (license_plate_id) REFERENCES license_plates(id) ON DELETE CASCADE
		);
		CREATE TABLE IF NOT EXISTS newsletter (
			email TEXT
		);
	)";

	PGresult* result = PQexec(conn, sqlCreateTables);

	if (PQresultStatus(result) != PGRES_COMMAND_OK)
	{
		LOG_MESSAGE(CRITICAL) << "Failed to create tables in the database." << std::endl;
		PQclear(result);
		PQfinish(conn);
		return false;
	}
	PQclear(result);

	LOG_MESSAGE(INFO) << "Database initialized successfully." << std::endl;

	return true;
}

DatabaseManager& DatabaseManager::getInstance()
{
	static DatabaseManager instance;
	return instance;
}

std::vector<std::string> DatabaseManager::getVehicles()
{
	std::vector<std::string> vehicles;
	const char* sql = "SELECT id, image_path, license_plate, date_time, ticket, time_parked, total_amount, is_paid FROM vehicles;";

	PGresult* result = PQexec(conn, sql);

	if (PQresultStatus(result) != PGRES_TUPLES_OK)
	{
		LOG_MESSAGE(CRITICAL) << "Failed to fetch vehicles from the database." << std::endl;
		PQclear(result);
		return vehicles;
	}

	int numRows = PQntuples(result);
	for (int i = 0; i < numRows; i++)
	{
		int id = std::stoi(PQgetvalue(result, i, 0)) - 1;
		std::string imagePath = PQgetvalue(result, i, 1);
		std::string licensePlate = PQgetvalue(result, i, 2);
		std::string dateTime = PQgetvalue(result, i, 3);
		std::string ticket = PQgetvalue(result, i, 4);
		std::string timeParked = (PQgetlength(result, i, 5) == 0) ? "" : PQgetvalue(result, i, 5);
		int totalAmount = (PQgetlength(result, i, 6) == 0) ? 0 : std::stoi(PQgetvalue(result, i, 6));
		std::string isPaid = PQgetvalue(result, i, 7);

		std::string vehicleInfo = std::to_string(id) + ", " + imagePath + ", " + licensePlate + ", " + dateTime + ", " + ticket + ", " + timeParked + ", " + std::to_string(totalAmount) + ", " + isPaid;

		vehicles.push_back(vehicleInfo);
	}

	PQclear(result);
	return vehicles;
}

std::string DatabaseManager::getLastVehicleActivity(const std::string& vehicleLicensePlate)
{
	std::string activity = ", , , ";
	const char* sql = "SELECT license_plate, date_time, ticket, time_parked, total_amount FROM vehicles;";

	PGresult* result = PQexec(conn, sql);

	if (PQresultStatus(result) != PGRES_TUPLES_OK)
	{
		LOG_MESSAGE(CRITICAL) << "Failed to fetch vehicle activity from the database." << std::endl;
		PQclear(result);
		return activity;
	}

	int numRows = PQntuples(result);
	for (int i = 0; i < numRows; i++)
	{
		std::string licensePlate = PQgetvalue(result, i, 0);
		std::string dateTime = PQgetvalue(result, i, 1);
		std::string ticket = PQgetvalue(result, i, 2);
		std::string timeParked = (PQgetlength(result, i, 3) == 0) ? "" : PQgetvalue(result, i, 3);
		std::string totalAmount = (PQgetlength(result, i, 4) == 0) ? "0" : PQgetvalue(result, i, 4);

		if (licensePlate == vehicleLicensePlate)
		{
			if (timeParked.empty())
				activity = dateTime + ", " + "" + ", " + "" + ", " + "";
			else
				activity = ticket + ", " + dateTime + ", " + timeParked + ", " + totalAmount + " RON";
		}
	}

	PQclear(result);

	return activity;
}

std::string DatabaseManager::getTotalTimeParked(const std::string& vehicleLicensePlate)
{
	int totalSeconds = 0;
	const char* sql = "SELECT license_plate, time_parked FROM vehicles;";

	PGresult* result = PQexec(conn, sql);

	if (PQresultStatus(result) != PGRES_TUPLES_OK)
	{
		LOG_MESSAGE(CRITICAL) << "Failed to fetch parking time from the database." << std::endl;
		PQclear(result);
		return "00:00:00";
	}

	int numRows = PQntuples(result);
	for (int i = 0; i < numRows; i++)
	{
		std::string licensePlate = PQgetvalue(result, i, 0);
		std::string timeParked = (PQgetlength(result, i, 1) == 0) ? "" : PQgetvalue(result, i, 1);

		if (licensePlate == vehicleLicensePlate)
		{
			if (!timeParked.empty())
			{
				int hours, minutes, seconds;
				std::string time = timeParked;

				std::replace(time.begin(), time.end(), ':', ' ');
				std::istringstream ss(time);
				ss >> hours >> minutes >> seconds;

				totalSeconds += hours * 3600 + minutes * 60 + seconds;
			}
		}
	}

	std::tm timeParked = {};
	timeParked.tm_hour = totalSeconds / 3600;
	timeParked.tm_min = (totalSeconds % 3600) / 60;
	timeParked.tm_sec = totalSeconds % 60;

	std::ostringstream ss;
	ss << std::setw(2) << std::setfill('0') << timeParked.tm_hour << ":"
		<< std::setw(2) << std::setfill('0') << timeParked.tm_min << ":"
		<< std::setw(2) << std::setfill('0') << timeParked.tm_sec;

	PQclear(result);

	return ss.str();
}

int DatabaseManager::getPayment(const std::string& vehicleLicensePlate)
{
	int payment = 0;
	const char* sql = "SELECT license_plate, time_parked, total_amount FROM vehicles;";

	PGresult* result = PQexec(conn, sql);

	if (PQresultStatus(result) != PGRES_TUPLES_OK)
	{
		LOG_MESSAGE(CRITICAL) << "Failed to fetch payment data from the database." << std::endl;
		PQclear(result);
		return payment;
	}

	int numRows = PQntuples(result);
	for (int i = 0; i < numRows; i++)
	{
		std::string licensePlate = PQgetvalue(result, i, 0);
		std::string timeParked = (PQgetlength(result, i, 1) == 0) ? "" : PQgetvalue(result, i, 1);
		int totalAmount = (PQgetlength(result, i, 2) == 0) ? 0 : std::stoi(PQgetvalue(result, i, 2));

		if (licensePlate == vehicleLicensePlate && !timeParked.empty())
			payment += totalAmount;
	}

	PQclear(result);

	return payment;
}

std::vector<std::string> DatabaseManager::getAccounts()
{
	std::vector<std::string> accounts;
	const char* sql = "SELECT name, last_name, email, password, phone FROM accounts;";

	PGresult* result = PQexec(conn, sql);

	if (PQresultStatus(result) != PGRES_TUPLES_OK)
	{
		LOG_MESSAGE(CRITICAL) << "Failed to fetch account data from the database." << std::endl;
		PQclear(result);
		return accounts;
	}

	int numRows = PQntuples(result);

	for (int i = 0; i < numRows; ++i)
	{
		std::string name = PQgetvalue(result, i, 0);
		std::string lastName = PQgetvalue(result, i, 1);
		std::string email = PQgetvalue(result, i, 2);
		std::string password = PQgetvalue(result, i, 3);
		std::string phone = PQgetvalue(result, i, 4);

		accounts.push_back(name + ", " + lastName + ", " + email + ", " + password + ", " + phone);
	}

	PQclear(result);

	return accounts;
}

std::unordered_set<std::string> DatabaseManager::getNewsletter()
{
	std::unordered_set<std::string> newsletter;
	const char* sql = "SELECT email FROM newsletter;";

	PGresult* result = PQexec(conn, sql);

	if (PQresultStatus(result) != PGRES_TUPLES_OK)
	{
		LOG_MESSAGE(CRITICAL) << "Failed to fetch newsletter emails from the database." << std::endl;
		PQclear(result);
		return newsletter;
	}

	int numRows = PQntuples(result);

	for (int i = 0; i < numRows; ++i)
	{
		newsletter.insert(PQgetvalue(result, i, 0));
	}

	PQclear(result);

	return newsletter;
}

std::unordered_map<std::string, std::pair<std::string, std::string>> DatabaseManager::getSubscriptions(const std::string& email)
{
	std::unordered_map<std::string, std::pair<std::string, std::string>> subscriptions;

	const char* sqlGetSubscriptions = R"(
        SELECT DISTINCT sp.subscription_name
        FROM subscription_payments sp
        WHERE sp.email = $1;
    )";

	const char* params[] = { email.c_str() };

	PGresult* resultGetSubscriptions = PQexecParams(conn, sqlGetSubscriptions, 1, nullptr, params, nullptr, nullptr, 0);

	if (PQresultStatus(resultGetSubscriptions) != PGRES_TUPLES_OK)
	{
		LOG_MESSAGE(CRITICAL) << "Failed to fetch subscriptions for email: " << email << std::endl;
		PQclear(resultGetSubscriptions);
		return subscriptions;
	}

	int numRows = PQntuples(resultGetSubscriptions);

	for (int i = 0; i < numRows; i++)
	{
		std::string subscriptionName = PQgetvalue(resultGetSubscriptions, i, 0);
		subscriptions[subscriptionName] = {};
	}

	PQclear(resultGetSubscriptions);

	const char* sqlGetDetails = R"(
		SELECT p.date, l.number
		FROM subscription_payments sp
		INNER JOIN payments p ON sp.payment_id = p.id
		INNER JOIN subscription_license_plates slp ON sp.subscription_name = slp.subscription_name AND sp.email = slp.email
		INNER JOIN license_plates l ON slp.license_plate_id = l.id
		WHERE sp.subscription_name = $1 AND sp.email = $2;
    )";

	for (auto& pair : subscriptions)
	{
		std::string payments;
		std::string licensePlates;

		const char* detailsParams[] = { pair.first.c_str(), email.c_str() };

		PGresult* resultGetDetails = PQexecParams(conn, sqlGetDetails, 2, nullptr, detailsParams, nullptr, nullptr, 0);

		if (PQresultStatus(resultGetDetails) != PGRES_TUPLES_OK)
		{
			LOG_MESSAGE(CRITICAL) << "Failed to fetch details for subscription: " << pair.first << " and email: " << email << std::endl;
			PQclear(resultGetDetails);
			continue;
		}

		int numRowsDetails = PQntuples(resultGetDetails);

		for (int i = 0; i < numRowsDetails; ++i)
		{
			const char* paymentText = PQgetvalue(resultGetDetails, i, 0);
			if (paymentText)
			{
				std::string payment(paymentText);
				if (!payments.empty())
					payments += ", ";
				payments += payment;
			}

			const char* licensePlateText = PQgetvalue(resultGetDetails, i, 1);
			if (licensePlateText)
			{
				std::string licensePlate(licensePlateText);
				if (!licensePlates.empty())
					licensePlates += ", ";
				licensePlates += licensePlate;
			}
		}

		PQclear(resultGetDetails);

		pair.second.first = payments;
		pair.second.second = licensePlates;
	}

	return subscriptions;
}

std::vector<std::string> DatabaseManager::getVehicleHistory(const std::string& vehicleLicensePlate)
{
	std::vector<std::string> history;
	const char* sql = "SELECT license_plate, date_time, ticket, time_parked, total_amount FROM vehicles;";

	PGresult* result = PQexec(conn, sql);

	if (PQresultStatus(result) != PGRES_TUPLES_OK)
	{
		LOG_MESSAGE(CRITICAL) << "Failed to fetch vehicle history from the database." << std::endl;
		PQclear(result);
		return history;
	}

	int numRows = PQntuples(result);
	for (int i = numRows - 1; i >= 0; i--)
	{
		std::string licensePlate = PQgetvalue(result, i, 0);
		std::string dateTime = PQgetvalue(result, i, 1);
		std::string ticket = PQgetvalue(result, i, 2);
		std::string timeParked = (PQgetlength(result, i, 3) == 0) ? "" : PQgetvalue(result, i, 3);
		int totalAmount = (PQgetlength(result, i, 4) == 0) ? 0 : std::stoi(PQgetvalue(result, i, 4));

		if (vehicleLicensePlate == licensePlate)
			if (history.empty())
				if (!timeParked.empty())
					history.push_back(ticket + ", " + dateTime + ", " + timeParked + ", " + std::to_string(totalAmount) + " RON");
				else
					history.push_back(ticket + ", " + "" + ", " + "" + ", " + "");
			else
				if (!timeParked.empty())
					history.push_back(ticket + ", " + dateTime + ", " + timeParked + ", " + std::to_string(totalAmount) + " RON");
	}

	PQclear(result);

	return history;
}

bool DatabaseManager::getIsPaid(const std::string& vehicleLicensePlate)
{
	const char* sqlCheckLicensePlate = "SELECT 1 FROM license_plates WHERE number = $1;";

	const char* paramsCheckLicensePlate[] = { vehicleLicensePlate.c_str() };

	PGresult* checkLicensePlateResult = PQexecParams(conn, sqlCheckLicensePlate, 1, nullptr, paramsCheckLicensePlate, nullptr, nullptr, 0);

	if (PQresultStatus(checkLicensePlateResult) == PGRES_TUPLES_OK && PQntuples(checkLicensePlateResult))
	{
		PQclear(checkLicensePlateResult);
		return true;
	}

	PQclear(checkLicensePlateResult);

	const char* sql = R"(
    SELECT is_paid 
    FROM vehicles 
    WHERE license_plate = $1 
    ORDER BY date_time DESC 
    LIMIT 1;
	)";

	const char* params[] = { vehicleLicensePlate.c_str() };

	PGresult* checkResult = PQexecParams(conn, sql, 1, nullptr, params, nullptr, nullptr, 0);

	if (PQresultStatus(checkResult) != PGRES_TUPLES_OK)
	{
		LOG_MESSAGE(CRITICAL) << "Failed to check payment status for vehicle: " << vehicleLicensePlate << std::endl;
		PQclear(checkResult);
		return false;
	}

	int numRows = PQntuples(checkResult);
	if (numRows == 0)
	{
		LOG_MESSAGE(CRITICAL) << "No payment records found for vehicle: " << vehicleLicensePlate << std::endl;
		PQclear(checkResult);
		return false;
	}

	std::string isPaid = PQgetvalue(checkResult, 0, 0);

	PQclear(checkResult);

	if (isPaid == "true")
		return true;

	return false;
}

bool DatabaseManager::setIsPaid(const std::string& vehicle, std::string& licensePlate, std::string& dateTime, const bool& isTicket)
{
	const char* sqlCheck;
	if (isTicket)
		sqlCheck = R"(
            SELECT license_plate, date_time 
            FROM vehicles 
            WHERE ticket = $1 
            ORDER BY date_time DESC 
            LIMIT 1;
        )";
	else
		sqlCheck = R"(
            SELECT license_plate, date_time 
            FROM vehicles 
            WHERE license_plate = $1 
            ORDER BY date_time DESC 
            LIMIT 1;
        )";

	const char* params[] = { vehicle.c_str() };

	PGresult* checkResult = PQexecParams(conn, sqlCheck, 1, nullptr, params, nullptr, nullptr, 0);

	if (PQresultStatus(checkResult) != PGRES_TUPLES_OK)
	{
		LOG_MESSAGE(CRITICAL) << "Failed to fetch vehicle information for: " << vehicle << std::endl;
		PQclear(checkResult);
		return false;
	}

	int numRows = PQntuples(checkResult);
	if (numRows == 0)
	{
		LOG_MESSAGE(CRITICAL) << "No information found for vehicle: " << vehicle << std::endl;
		PQclear(checkResult);
		return false;
	}

	licensePlate = PQgetvalue(checkResult, 0, 0);
	dateTime = PQgetvalue(checkResult, 0, 1);

	PQclear(checkResult);

	const char* sqlUpdate;
	if (isTicket)
		sqlUpdate = "UPDATE vehicles SET is_paid = 'true' WHERE ticket = $1;";
	else
		sqlUpdate = "UPDATE vehicles SET is_paid = 'true' WHERE license_plate = $1;";

	PGresult* result = PQexecParams(conn, sqlUpdate, 1, nullptr, params, nullptr, nullptr, 0);

	if (PQresultStatus(result) != PGRES_COMMAND_OK)
	{
		LOG_MESSAGE(CRITICAL) << "Failed to update payment status for vehicle: " << vehicle << std::endl;
		PQclear(result);
		return false;
	}

	PQclear(result);
	return true;
}

void DatabaseManager::setName(const std::string& email, const std::string& newName)
{
	if (newName.empty())
		return;

	const char* sql = "UPDATE accounts SET name = $1 WHERE email = $2";

	const char* params[] = { newName.c_str(), email.c_str() };

	PGresult* result = PQexecParams(conn, sql, 2, nullptr, params, nullptr, nullptr, 0);

	if (PQresultStatus(result) != PGRES_COMMAND_OK)
	{
		LOG_MESSAGE(CRITICAL) << "Failed to update name for email: " << email << std::endl;
		PQclear(result);
		return;
	}

	PQclear(result);
}

void DatabaseManager::setLastName(const std::string& email, const std::string& newLastName)
{
	if (newLastName.empty())
		return;

	const char* sql = "UPDATE accounts SET last_name = $1 WHERE email = $2";

	const char* params[] = { newLastName.c_str(), email.c_str() };

	PGresult* result = PQexecParams(conn, sql, 2, nullptr, params, nullptr, nullptr, 0);

	if (PQresultStatus(result) != PGRES_COMMAND_OK)
	{
		LOG_MESSAGE(CRITICAL) << "Failed to update last name for email: " << email << std::endl;
		PQclear(result);
		return;
	}

	PQclear(result);
}

void DatabaseManager::setEmail(const std::string& email, const std::string& newEmail)
{
	if (newEmail.empty())
		return;

	const char* selectAccount = "SELECT name, last_name, password, phone FROM accounts WHERE email = $1";
	const char* paramsSelect[] = { email.c_str() };

	PGresult* result = PQexecParams(conn, selectAccount, 1, nullptr, paramsSelect, nullptr, nullptr, 0);

	if (PQresultStatus(result) != PGRES_TUPLES_OK || PQntuples(result) != 1)
	{
		LOG_MESSAGE(CRITICAL) << "Failed to fetch account information for: " << email << std::endl;
		PQclear(result);
		return;
	}

	std::string name = PQgetvalue(result, 0, 0);
	std::string lastName = PQgetvalue(result, 0, 1);
	std::string password = PQgetvalue(result, 0, 2);
	std::string phone = PQgetvalue(result, 0, 3);

	PQclear(result);

	const char* insertAccount = "INSERT INTO accounts (email, name, last_name, password, phone) VALUES ($1, $2, $3, $4, $5)";
	const char* paramsInsert[] = { newEmail.c_str(), name.c_str(), lastName.c_str(), password.c_str(), phone.c_str() };

	result = PQexecParams(conn, insertAccount, 5, nullptr, paramsInsert, nullptr, nullptr, 0);

	if (PQresultStatus(result) != PGRES_COMMAND_OK)
	{
		LOG_MESSAGE(CRITICAL) << "Failed to insert new account with email: " << newEmail << std::endl;
		PQclear(result);
		return;
	}

	PQclear(result);

	const char* updatePayments = "UPDATE subscription_payments SET email = $1 WHERE email = $2";
	const char* paramsPayments[] = { newEmail.c_str(), email.c_str() };

	result = PQexecParams(conn, updatePayments, 2, nullptr, paramsPayments, nullptr, nullptr, 0);

	if (PQresultStatus(result) != PGRES_COMMAND_OK)
	{
		LOG_MESSAGE(CRITICAL) << "Failed to update subscription payments for email: " << email << std::endl;
		PQclear(result);
		return;
	}
	PQclear(result);

	const char* updateLicensePlates = "UPDATE subscription_license_plates SET email = $1 WHERE email = $2";
	const char* paramsLicensePlates[] = { newEmail.c_str(), email.c_str() };

	result = PQexecParams(conn, updateLicensePlates, 2, nullptr, paramsLicensePlates, nullptr, nullptr, 0);

	if (PQresultStatus(result) != PGRES_COMMAND_OK)
	{
		LOG_MESSAGE(CRITICAL) << "Failed to update subscription license plates for email: " << email << std::endl;
		PQclear(result);
		return;
	}
	PQclear(result);

	const char* deleteAccount = "DELETE FROM accounts WHERE email = $1";
	const char* paramsDelete[] = { email.c_str() };

	result = PQexecParams(conn, deleteAccount, 1, nullptr, paramsDelete, nullptr, nullptr, 0);

	if (PQresultStatus(result) != PGRES_COMMAND_OK)
	{
		LOG_MESSAGE(CRITICAL) << "Failed to delete old account with email: " << email << std::endl;
		PQclear(result);
		return;
	}

	PQclear(result);
}

void DatabaseManager::setPassword(const std::string& email, const std::string& newPassword)
{
	if (newPassword.empty())
		return;

	const char* sql = "UPDATE accounts SET password = $1 WHERE email = $2";

	const char* params[] = { newPassword.c_str(), email.c_str() };

	PGresult* result = PQexecParams(conn, sql, 2, nullptr, params, nullptr, nullptr, 0);

	if (PQresultStatus(result) != PGRES_COMMAND_OK)
	{
		LOG_MESSAGE(CRITICAL) << "Failed to update password for email: " << email << std::endl;
		PQclear(result);
		return;
	}

	PQclear(result);
}

void DatabaseManager::setPhone(const std::string& email, const std::string& newPhone)
{
	if (newPhone.empty())
		return;

	const char* sql = "UPDATE accounts SET phone = $1 WHERE email = $2";

	const char* params[] = { newPhone.c_str(), email.c_str() };

	PGresult* result = PQexecParams(conn, sql, 2, nullptr, params, nullptr, nullptr, 0);

	if (PQresultStatus(result) != PGRES_COMMAND_OK)
	{
		LOG_MESSAGE(CRITICAL) << "Failed to update phone number for email: " << email << std::endl;
		PQclear(result);
		return;
	}

	PQclear(result);
}

void DatabaseManager::addVehicle(const int& id, const std::string& imagePath, const std::string& licensePlate, const std::string& dateTime, const std::string& ticket, const std::string& timeParked, const std::string& totalAmount, const std::string& isPaid)
{
	const char* sqlInsert = R"(
        INSERT INTO vehicles (image_path, license_plate, date_time, ticket, time_parked, total_amount, is_paid)
        VALUES ($1, $2, $3, $4, $5, $6, $7);
    )";

	const char* insertParams[] = {
		imagePath.c_str(),
		licensePlate.c_str(),
		dateTime.c_str(),
		ticket.c_str(),
		timeParked.c_str(),
		totalAmount.c_str(),
		isPaid.c_str()
	};

	PGresult* insertResult = PQexecParams(conn, sqlInsert, 7, nullptr, insertParams, nullptr, nullptr, 0);

	if (PQresultStatus(insertResult) != PGRES_COMMAND_OK)
	{
		LOG_MESSAGE(CRITICAL) << "Error inserting vehicle with license plate: " << licensePlate << std::endl;
		PQclear(insertResult);
		throw std::runtime_error("Error inserting vehicle into database");
	}

	PQclear(insertResult);
}

void DatabaseManager::addAccount(const std::string& name, const std::string& lastName, const std::string& email, const std::string& password, const std::string& phone)
{
	const char* sql = R"(
        INSERT INTO accounts (name, last_name, email, password, phone)
        VALUES ($1, $2, $3, $4, $5);
    )";

	const char* params[] = { name.c_str(), lastName.c_str(), email.c_str(), password.c_str(), phone.c_str() };

	PGresult* result = PQexecParams(conn, sql, 5, nullptr, params, nullptr, nullptr, 0);

	if (PQresultStatus(result) != PGRES_COMMAND_OK)
	{
		LOG_MESSAGE(CRITICAL) << "Error adding account for email: " << email << std::endl;
		PQclear(result);
		return;
	}

	PQclear(result);
}

void DatabaseManager::addSubscription(const std::string& email, const std::string& name)
{
	const char* sqlInsertPayment = R"(
    INSERT INTO payments (date)
    VALUES ($1)
    RETURNING id;
	)";

	time_t now = time(0);
	tm* ltm = localtime(&now);
	char buffer[11];
	snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d", 1900 + ltm->tm_year, 1 + ltm->tm_mon, ltm->tm_mday);
	std::string currentDate = std::string(buffer);

	const char* paramsPayment[] = { currentDate.c_str() };
	PGresult* result = PQexecParams(conn, sqlInsertPayment, 1, nullptr, paramsPayment, nullptr, nullptr, 0);

	if (PQresultStatus(result) != PGRES_TUPLES_OK)
	{
		LOG_MESSAGE(CRITICAL) << "Error inserting payment for email: " << email << std::endl;
		PQclear(result);
		return;
	}

	int paymentId = std::stoi(PQgetvalue(result, 0, 0));
	PQclear(result);

	const char* sqlLinkSubscriptionPayment = R"(
    INSERT INTO subscription_payments (email, subscription_name, payment_id)
    VALUES ($1, $2, $3);
	)";

	std::string id = std::to_string(paymentId);
	const char* paramsLinkSubscriptionPayment[] = {
		email.c_str(),
		name.c_str(),
		id.c_str()
	};

	result = PQexecParams(conn, sqlLinkSubscriptionPayment, 3, nullptr, paramsLinkSubscriptionPayment, nullptr, nullptr, 0);

	if (PQresultStatus(result) != PGRES_COMMAND_OK)
	{
		LOG_MESSAGE(CRITICAL) << "Error linking subscription for email: " << email << std::endl;
		PQclear(result);
		return;
	}

	PQclear(result);
}

void DatabaseManager::addLicensePlate(const std::string& email, const std::string& name, const std::string& licensePlate)
{
	const char* insertLicensePlateSQL = R"(
        INSERT INTO license_plates (number) 
        VALUES ($1);
    )";

	const char* params[] = { licensePlate.c_str() };
	PGresult* result = PQexecParams(conn, insertLicensePlateSQL, 1, nullptr, params, nullptr, nullptr, 0);

	if (PQresultStatus(result) != PGRES_COMMAND_OK)
	{
		LOG_MESSAGE(CRITICAL) << "Error inserting license plate: " << licensePlate << " for email: " << email << std::endl;
		PQclear(result);
		return;
	}

	PQclear(result);

	const char* getLastInsertIdSQL = R"(
        SELECT LASTVAL();
    )";
	result = PQexec(conn, getLastInsertIdSQL);

	if (PQresultStatus(result) != PGRES_TUPLES_OK)
	{
		LOG_MESSAGE(CRITICAL) << "Error getting last inserted license plate ID for email: " << email << std::endl;
		PQclear(result);
		return;
	}

	int licensePlateId = std::stoi(PQgetvalue(result, 0, 0));
	PQclear(result);

	const char* insertSubscriptionLicensePlateSQL = R"(
    INSERT INTO subscription_license_plates (email, subscription_name, license_plate_id)
    VALUES ($1, $2, $3);
	)";

	std::string id = std::to_string(licensePlateId);
	const char* paramsInsert[] = { email.c_str(), name.c_str(), id.c_str() };
	result = PQexecParams(conn, insertSubscriptionLicensePlateSQL, 3, nullptr, paramsInsert, nullptr, nullptr, 0);

	if (PQresultStatus(result) != PGRES_COMMAND_OK)
	{
		LOG_MESSAGE(CRITICAL) << "Error linking license plate to subscription for email: " << email << " with license plate: " << licensePlate << std::endl;
		PQclear(result);
		return;
	}

	PQclear(result);
}

void DatabaseManager::subscribeNewsletter(const std::string& email)
{
	const char* subscribeSQL = R"(
        INSERT INTO newsletter (email) 
        VALUES ($1);
    )";

	const char* params[] = { email.c_str() };
	PGresult* result = PQexecParams(conn, subscribeSQL, 1, nullptr, params, nullptr, nullptr, 0);

	if (PQresultStatus(result) != PGRES_COMMAND_OK)
	{
		LOG_MESSAGE(CRITICAL) << "Error subscribing email to newsletter: " << email << std::endl;
		PQclear(result);
		return;
	}

	PQclear(result);
}

void DatabaseManager::unsubscribeNewsletter(const std::string& email)
{
	const char* unsubscribeSQL = R"(
        DELETE FROM newsletter 
        WHERE email = $1;
    )";

	const char* params[] = { email.c_str() };
	PGresult* result = PQexecParams(conn, unsubscribeSQL, 1, nullptr, params, nullptr, nullptr, 0);

	if (PQresultStatus(result) != PGRES_COMMAND_OK)
	{
		LOG_MESSAGE(CRITICAL) << "Error unsubscribing email from newsletter: " << email << std::endl;
		PQclear(result);
		return;
	}

	PQclear(result);
}

void DatabaseManager::deleteSubscription(const std::string& email, const std::string& name)
{
	std::vector<int> paymentIds;
	std::vector<int> licensePlateIds;

	const char* sqlGetPaymentIds = R"(
        SELECT payment_id
        FROM subscription_payments
        WHERE email = $1 AND subscription_name = $2;
    )";
	const char* paramsGetPaymentIds[] = { email.c_str(), name.c_str() };
	PGresult* result = PQexecParams(conn, sqlGetPaymentIds, 2, nullptr, paramsGetPaymentIds, nullptr, nullptr, 0);

	if (PQresultStatus(result) != PGRES_TUPLES_OK)
	{
		LOG_MESSAGE(CRITICAL) << "Error retrieving payment IDs for subscription: " << name << " and email: " << email << std::endl;
		PQclear(result);
		return;
	}

	for (int i = 0; i < PQntuples(result); ++i)
		paymentIds.push_back(std::stoi(PQgetvalue(result, i, 0)));

	PQclear(result);

	const char* sqlGetLicensePlateIds = R"(
        SELECT license_plate_id
        FROM subscription_license_plates
        WHERE email = $1 AND subscription_name = $2;
    )";
	const char* paramsGetLicensePlateIds[] = { email.c_str(), name.c_str() };
	result = PQexecParams(conn, sqlGetLicensePlateIds, 2, nullptr, paramsGetLicensePlateIds, nullptr, nullptr, 0);

	if (PQresultStatus(result) != PGRES_TUPLES_OK)
	{
		LOG_MESSAGE(CRITICAL) << "Error retrieving license plate IDs for subscription: " << name << " and email: " << email << std::endl;
		PQclear(result);
		return;
	}

	for (int i = 0; i < PQntuples(result); ++i)
		licensePlateIds.push_back(std::stoi(PQgetvalue(result, i, 0)));

	PQclear(result);

	const char* sqlDeletePayments = R"(
        DELETE FROM subscription_payments
        WHERE email = $1 AND subscription_name = $2;
    )";
	const char* paramsDeletePayments[] = { email.c_str(), name.c_str() };
	result = PQexecParams(conn, sqlDeletePayments, 2, nullptr, paramsDeletePayments, nullptr, nullptr, 0);

	if (PQresultStatus(result) != PGRES_COMMAND_OK)
	{
		LOG_MESSAGE(CRITICAL) << "Error deleting payments for subscription: " << name << " and email: " << email << std::endl;
		PQclear(result);
		return;
	}
	PQclear(result);

	const char* sqlDeleteLicensePlates = R"(
        DELETE FROM subscription_license_plates
        WHERE email = $1 AND subscription_name = $2;
    )";
	const char* paramsDeleteLicensePlates[] = { email.c_str(), name.c_str() };
	result = PQexecParams(conn, sqlDeleteLicensePlates, 2, nullptr, paramsDeleteLicensePlates, nullptr, nullptr, 0);

	if (PQresultStatus(result) != PGRES_COMMAND_OK)
	{
		LOG_MESSAGE(CRITICAL) << "Error deleting license plates for subscription: " << name << " and email: " << email << std::endl;
		PQclear(result);
		return;
	}
	PQclear(result);

	for (int id : paymentIds)
	{
		const char* sqlDeletePayment = R"(
            DELETE FROM payments
            WHERE id = $1;
        )";

		std::string strId = std::to_string(id);
		const char* paramsDeletePayment[] = { strId.c_str() };
		result = PQexecParams(conn, sqlDeletePayment, 1, nullptr, paramsDeletePayment, nullptr, nullptr, 0);

		if (PQresultStatus(result) != PGRES_COMMAND_OK)
		{
			LOG_MESSAGE(CRITICAL) << "Error deleting payment ID: " << std::to_string(id) << " for email: " << email << std::endl;
			PQclear(result);
			return;
		}
		PQclear(result);
	}

	for (int id : licensePlateIds)
	{
		const char* sqlDeleteLicensePlate = R"(
            DELETE FROM license_plates
            WHERE id = $1;
        )";

		std::string strId = std::to_string(id);
		const char* paramsDeleteLicensePlate[] = { strId.c_str() };
		result = PQexecParams(conn, sqlDeleteLicensePlate, 1, nullptr, paramsDeleteLicensePlate, nullptr, nullptr, 0);

		if (PQresultStatus(result) != PGRES_COMMAND_OK)
		{
			LOG_MESSAGE(CRITICAL) << "Error deleting license plate ID: " << std::to_string(id) << " for email: " << email << std::endl;
			PQclear(result);
			return;
		}
		PQclear(result);
	}
}

void DatabaseManager::deleteLicensePlate(const std::string& email, const std::string& name, const std::string& licensePlate)
{
	const char* selectIdSQL = R"(
        SELECT id FROM license_plates 
        WHERE number = $1;
    )";
	const char* paramsSelect[] = { licensePlate.c_str() };
	PGresult* result = PQexecParams(conn, selectIdSQL, 1, nullptr, paramsSelect, nullptr, nullptr, 0);

	int licensePlateId = -1;
	if (PQresultStatus(result) == PGRES_TUPLES_OK && PQntuples(result) > 0)
		licensePlateId = std::stoi(PQgetvalue(result, 0, 0));
	else
	{
		LOG_MESSAGE(CRITICAL) << "License plate not found: " << licensePlate << std::endl;
		PQclear(result);
		return;
	}

	PQclear(result);

	if (licensePlateId != -1)
	{
		const char* deleteLinkSQL = R"(
	        DELETE FROM subscription_license_plates
	        WHERE license_plate_id = $1;
	    )";

		std::string id = std::to_string(licensePlateId);
		const char* paramsDeleteLink[] = { id.c_str() };
		result = PQexecParams(conn, deleteLinkSQL, 1, nullptr, paramsDeleteLink, nullptr, nullptr, 0);

		if (PQresultStatus(result) != PGRES_COMMAND_OK)
		{
			LOG_MESSAGE(CRITICAL) << "Error deleting license plate link for plate: " << licensePlate << " and subscription: " << name << std::endl;
			PQclear(result);
			return;
		}

		PQclear(result);

		const char* deleteLicensePlateSQL = R"(
	        DELETE FROM license_plates
	        WHERE id = $1;
	    )";
		const char* paramsDeleteLicensePlate[] = { id.c_str() };
		result = PQexecParams(conn, deleteLicensePlateSQL, 1, nullptr, paramsDeleteLicensePlate, nullptr, nullptr, 0);

		if (PQresultStatus(result) != PGRES_COMMAND_OK)
		{
			LOG_MESSAGE(CRITICAL) << "Error deleting license plate record for plate: " << licensePlate << std::endl;
			PQclear(result);
			return;
		}

		PQclear(result);
	}
}

DatabaseManager::~DatabaseManager()
{
	if (conn)
	{
		LOG_MESSAGE(INFO) << "Closing database connection." << std::endl;
		PQfinish(conn);
	}

	LOG_MESSAGE(INFO) << "The program has been closed." << std::endl << std::endl << std::endl << std::endl;
}
