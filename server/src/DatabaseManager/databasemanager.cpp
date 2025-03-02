#include "databasemanager.h"

#include <sys/stat.h>
#include <ctime>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <algorithm>

int getSeconds(const std::string& firstDateTime, const std::string& secondFirstTime)
{
	int totalSeconds = 0;
	std::tm firstTm = {};
	std::tm secondTm = {};

	std::istringstream firstStream(firstDateTime);
	firstStream >> std::get_time(&firstTm, "%Y-%m-%d %H:%M:%S");

	std::istringstream secondStream(secondFirstTime);
	secondStream >> std::get_time(&secondTm, "%Y-%m-%d %H:%M:%S");

	std::time_t firstSeconds = std::mktime(&firstTm);
	std::time_t secondSeconds = std::mktime(&secondTm);

	if (firstSeconds != -1 && secondSeconds != -1)
		return std::abs(std::difftime(firstSeconds, secondSeconds));

	return 0;
}

std::string timeParked(const int& seconds)
{
	std::tm dateTime = {};
	dateTime.tm_hour = seconds / 3600;
	dateTime.tm_min = (seconds % 3600) / 60;
	dateTime.tm_sec = seconds % 60;

	std::ostringstream ss;
	ss << std::setw(2) << std::setfill('0') << dateTime.tm_hour << ":"
		<< std::setw(2) << std::setfill('0') << dateTime.tm_min << ":"
		<< std::setw(2) << std::setfill('0') << dateTime.tm_sec;

	return ss.str();
}

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
			license_plate TEXT NOT NULL,
			date_time TIMESTAMP NOT NULL,
			ticket TIMESTAMP NOT NULL,
			total_amount REAL NOT NULL,
			is_paid BOOLEAN NOT NULL
		);

		CREATE TABLE IF NOT EXISTS accounts (
			id SERIAL PRIMARY KEY,
			email TEXT UNIQUE NOT NULL,
			name TEXT NOT NULL,
			last_name TEXT NOT NULL,
			password TEXT NOT NULL,
			phone TEXT UNIQUE NOT NULL
		);

		CREATE TABLE IF NOT EXISTS subscriptions (
			id SERIAL PRIMARY KEY,
			account_id INTEGER NOT NULL,
			subscription_name TEXT NOT NULL,
			FOREIGN KEY (account_id) REFERENCES accounts(id) ON DELETE CASCADE
		);

		CREATE TABLE IF NOT EXISTS subscription_license_vehicles (
			id SERIAL PRIMARY KEY,
			subscription_id INTEGER NOT NULL,
			license_plate TEXT NOT NULL,
			FOREIGN KEY (subscription_id) REFERENCES subscriptions(id) ON DELETE CASCADE
		);

		CREATE TABLE IF NOT EXISTS subscription_payments (
			id SERIAL PRIMARY KEY,
			subscription_id INTEGER NOT NULL,
			date DATE NOT NULL,
			FOREIGN KEY (subscription_id) REFERENCES subscriptions(id) ON DELETE CASCADE
		);

		CREATE TABLE IF NOT EXISTS newsletter (
			id SERIAL PRIMARY KEY,
			email TEXT UNIQUE NOT NULL
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
	const char* sql = "SELECT id, license_plate, TO_CHAR(date_time, 'DD-MM-YYYY HH24:MI:SS'), TO_CHAR(ticket, 'DD-MM-YYYY HH24:MI:SS'), total_amount, is_paid FROM vehicles;";

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
		int id = std::stoi(PQgetvalue(result, i, 0));
		std::string licensePlate = PQgetvalue(result, i, 1);
		std::string dateTime = PQgetvalue(result, i, 2);
		std::string ticket = PQgetvalue(result, i, 3);
		float totalAmount = std::stof(PQgetvalue(result, i, 4));
		bool isPaid = (std::string(PQgetvalue(result, i, 5)) == "t");

		std::ostringstream stream;
		stream << std::fixed << std::setprecision(2) << totalAmount;
		std::string totalAmountStr = stream.str();
		std::string vehicleInfo = std::to_string(id) + ", " + licensePlate + ", " + dateTime + ", " + ticket + ", " + totalAmountStr + ", " + (isPaid ? "1" : "0");

		vehicles.push_back(vehicleInfo);
	}

	PQclear(result);
	return vehicles;
}

std::string DatabaseManager::getLastVehicleActivity(const std::string& vehicleLicensePlate)
{
	std::string activity = ", , , ";

	const char* sql = "SELECT TO_CHAR(date_time, 'DD-MM-YYYY HH24:MI:SS'), "
		"TO_CHAR(ticket, 'DD-MM-YYYY HH24:MI:SS'), total_amount "
		"FROM vehicles WHERE license_plate = $1 "
		"ORDER BY date_time DESC LIMIT 1;";

	const char* paramValues[1] = { vehicleLicensePlate.c_str() };
	PGresult* result = PQexecParams(conn, sql, 1, nullptr, paramValues, nullptr, nullptr, 0);

	if (PQresultStatus(result) != PGRES_TUPLES_OK)
	{
		LOG_MESSAGE(CRITICAL) << "Failed to fetch vehicle activity from the database." << std::endl;
		PQclear(result);
		return activity;
	}

	if (PQntuples(result) > 0)
	{
		std::string dateTime = PQgetvalue(result, 0, 0);
		std::string ticket = PQgetvalue(result, 0, 1);
		float totalAmount = std::stof(PQgetvalue(result, 0, 2));

		if (dateTime != ticket)
			activity = ticket + ", " + dateTime + ", " + timeParked(getSeconds(ticket, dateTime)) + ", " + std::to_string(totalAmount) + " RON";
		else
			activity = dateTime + ", " + "" + ", " + "" + ", " + "";
	}

	PQclear(result);
	return activity;
}

std::string DatabaseManager::getTotalTimeParked(const std::string& vehicleLicensePlate)
{
	int totalSeconds = 0;
	const char* sql = "SELECT license_plate, date_time, ticket FROM vehicles;";

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
		std::string dateTime = PQgetvalue(result, i, 1);
		std::string ticket = PQgetvalue(result, i, 2);

		if (licensePlate == vehicleLicensePlate && dateTime != ticket)
			totalSeconds += getSeconds(ticket, dateTime);
	}

	PQclear(result);

	return timeParked(totalSeconds);
}

int DatabaseManager::getPayment(const std::string& vehicleLicensePlate)
{
	int payment = 0;
	const char* sql = "SELECT SUM(total_amount) FROM vehicles "
		"WHERE license_plate = $1 AND date_time != ticket;";

	const char* paramValues[1] = { vehicleLicensePlate.c_str() };
	PGresult* result = PQexecParams(conn, sql, 1, nullptr, paramValues, nullptr, nullptr, 0);

	if (PQresultStatus(result) != PGRES_TUPLES_OK)
	{
		LOG_MESSAGE(CRITICAL) << "Failed to fetch payment data from the database." << std::endl;
		PQclear(result);
		return payment;
	}

	if (PQntuples(result) > 0 && PQgetlength(result, 0, 0) > 0)
		payment = std::stoi(PQgetvalue(result, 0, 0));

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

	const char* sql = R"(
        SELECT s.subscription_name, sp.date, slv.license_plate
        FROM subscriptions s
        LEFT JOIN subscription_payments sp ON s.id = sp.subscription_id
        LEFT JOIN subscription_license_vehicles slv ON s.id = slv.subscription_id
        WHERE s.account_id = (SELECT id FROM accounts WHERE email = $1);
    )";

	const char* paramValues[] = { email.c_str() };
	PGresult* result = PQexecParams(conn, sql, 1, nullptr, paramValues, nullptr, nullptr, 0);

	if (PQresultStatus(result) != PGRES_TUPLES_OK)
	{
		LOG_MESSAGE(CRITICAL) << "Failed to fetch subscriptions for email: " << email << std::endl;
		PQclear(result);
		return subscriptions;
	}

	int numRows = PQntuples(result);

	for (int i = 0; i < numRows; ++i)
	{
		std::string subscriptionName = PQgetvalue(result, i, 0);
		std::string paymentDate = PQgetvalue(result, i, 1) ? PQgetvalue(result, i, 1) : "";
		std::string licensePlate = PQgetvalue(result, i, 2) ? PQgetvalue(result, i, 2) : "";

		if (subscriptions.find(subscriptionName) == subscriptions.end())
			subscriptions[subscriptionName] = { paymentDate, licensePlate };
		else
		{
			if (!paymentDate.empty() && subscriptions[subscriptionName].first.find(paymentDate) == std::string::npos)
				subscriptions[subscriptionName].first += ", " + paymentDate;
			if (!licensePlate.empty() && subscriptions[subscriptionName].second.find(licensePlate) == std::string::npos)
				subscriptions[subscriptionName].second += ", " + licensePlate;
		}
	}

	PQclear(result);
	return subscriptions;
}

std::vector<std::string> DatabaseManager::getVehicleHistory(const std::string& vehicleLicensePlate)
{
	std::vector<std::string> history;
	const char* sql = "SELECT license_plate, date_time, ticket, total_amount FROM vehicles;";

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
		int totalAmount = (PQgetlength(result, i, 3) == 0) ? 0 : std::stoi(PQgetvalue(result, i, 3));

		if (vehicleLicensePlate == licensePlate)
		{
			if (history.empty())
				if (dateTime != ticket)
					history.push_back(ticket + ", " + dateTime + ", " + timeParked(getSeconds(ticket, dateTime)) + ", " + std::to_string(totalAmount) + " RON");
				else
					history.push_back(ticket + ", " + "" + ", " + "" + ", " + "");
			else
				if (dateTime != ticket)
					history.push_back(ticket + ", " + dateTime + ", " + timeParked(getSeconds(ticket, dateTime)) + ", " + std::to_string(totalAmount) + " RON");
		}
	}

	PQclear(result);

	return history;
}

bool DatabaseManager::getIsPaid(const std::string& vehicleLicensePlate)
{
	const char* sqlCheckSubscription = R"(
        SELECT 1 FROM subscription_license_vehicles WHERE license_plate = $1;
    )";

	const char* paramsSubscription[] = { vehicleLicensePlate.c_str() };
	PGresult* checkSubscriptionResult = PQexecParams(conn, sqlCheckSubscription, 1, nullptr, paramsSubscription, nullptr, nullptr, 0);

	if (PQresultStatus(checkSubscriptionResult) == PGRES_TUPLES_OK && PQntuples(checkSubscriptionResult) > 0)
	{
		PQclear(checkSubscriptionResult);
		return true;
	}

	PQclear(checkSubscriptionResult);

	const char* sqlCheckPayment = R"(
        SELECT is_paid 
        FROM vehicles 
        WHERE license_plate = $1 
        ORDER BY date_time DESC 
        LIMIT 1;
    )";

	const char* params[] = { vehicleLicensePlate.c_str() };
	PGresult* checkPaymentResult = PQexecParams(conn, sqlCheckPayment, 1, nullptr, params, nullptr, nullptr, 0);

	if (PQresultStatus(checkPaymentResult) != PGRES_TUPLES_OK)
	{
		LOG_MESSAGE(CRITICAL) << "Failed to check payment status for vehicle: " << vehicleLicensePlate << std::endl;
		PQclear(checkPaymentResult);
		return false;
	}

	int numRows = PQntuples(checkPaymentResult);
	if (numRows == 0)
	{
		PQclear(checkPaymentResult);
		return false;
	}

	std::string isPaid = PQgetvalue(checkPaymentResult, 0, 0);
	PQclear(checkPaymentResult);

	return isPaid == "t";
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

	const char* sqlCheckSubscription = R"(
        SELECT 1 FROM subscription_license_vehicles WHERE license_plate = $1;
    )";

	const char* paramsSubscription[] = { licensePlate.c_str() };
	PGresult* checkSubscriptionResult = PQexecParams(conn, sqlCheckSubscription, 1, nullptr, paramsSubscription, nullptr, nullptr, 0);

	if (PQresultStatus(checkSubscriptionResult) == PGRES_TUPLES_OK && PQntuples(checkSubscriptionResult) > 0)
	{
		PQclear(checkSubscriptionResult);
		return true;
	}

	PQclear(checkSubscriptionResult);

	const char* sqlUpdate;
	if (isTicket)
		sqlUpdate = "UPDATE vehicles SET is_paid = 't' WHERE ticket = $1;";
	else
		sqlUpdate = "UPDATE vehicles SET is_paid = 't' WHERE license_plate = $1;";

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

	const char* sql = "UPDATE accounts SET email = $1 WHERE email = $2";

	const char* params[] = { newEmail.c_str(), email.c_str() };

	PGresult* result = PQexecParams(conn, sql, 2, nullptr, params, nullptr, nullptr, 0);

	if (PQresultStatus(result) != PGRES_COMMAND_OK)
	{
		LOG_MESSAGE(CRITICAL) << "Failed to update email for email: " << email << std::endl;
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

void DatabaseManager::addVehicle(const std::string& licensePlate, const std::string& dateTime, const std::string& ticket, float totalAmount)
{
	const char* sqlInsert = R"(
        INSERT INTO vehicles (license_plate, date_time, ticket, total_amount, is_paid)
        VALUES ($1, $2, $3, $4, $5);
    )";

	std::ostringstream stream;
	stream << std::fixed << std::setprecision(2) << totalAmount;
	std::string totalAmountStr = stream.str();

	std::string isPaidStr;
	if (dateTime == ticket)
		isPaidStr = "f";
	else
		isPaidStr = "t";

	const char* insertParams[] = {
		licensePlate.c_str(),
		dateTime.c_str(),
		ticket.c_str(),
		totalAmountStr.c_str(),
		isPaidStr.c_str()
	};

	PGresult* insertResult = PQexecParams(conn, sqlInsert, 5, nullptr, insertParams, nullptr, nullptr, 0);

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
	const char* sqlInsertSubscription = R"(
        INSERT INTO subscriptions (account_id, subscription_name)
        VALUES ((SELECT id FROM accounts WHERE email = $1), $2)
        RETURNING id;
    )";

	const char* paramsSubscription[] = { email.c_str(), name.c_str() };
	PGresult* result = PQexecParams(conn, sqlInsertSubscription, 2, nullptr, paramsSubscription, nullptr, nullptr, 0);

	if (PQresultStatus(result) != PGRES_TUPLES_OK)
	{
		LOG_MESSAGE(CRITICAL) << "Error inserting subscription for email: " << email << " and subscription: " << name << std::endl;
		PQclear(result);
		return;
	}

	std::string subscriptionId = PQgetvalue(result, 0, 0);
	PQclear(result);

	const char* sqlInsertPayment = R"(
    INSERT INTO subscription_payments (subscription_id, date)
    VALUES ($1, $2)
    RETURNING id;
)";

	time_t now = time(0);
	tm* ltm = localtime(&now);
	char buffer[11];
	snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d", 1900 + ltm->tm_year, 1 + ltm->tm_mon, ltm->tm_mday);
	std::string currentDate = std::string(buffer);

	const char* paramsPayment[] = { subscriptionId.c_str(), currentDate.c_str() };
	result = PQexecParams(conn, sqlInsertPayment, 2, nullptr, paramsPayment, nullptr, nullptr, 0);

	if (PQresultStatus(result) != PGRES_TUPLES_OK)
	{
		LOG_MESSAGE(CRITICAL) << "Error inserting payment for subscription for email: " << email << std::endl;
		PQclear(result);
		return;
	}

	int paymentId = std::stoi(PQgetvalue(result, 0, 0));
	PQclear(result);
}

void DatabaseManager::addLicensePlate(const std::string& email, const std::string& name, const std::string& licensePlate)
{
	const char* getAccountIdSQL = R"(
        SELECT id 
        FROM accounts 
        WHERE email = $1
        LIMIT 1;
    )";

	const char* params[] = { email.c_str() };
	PGresult* result = PQexecParams(conn, getAccountIdSQL, 1, nullptr, params, nullptr, nullptr, 0);

	if (PQresultStatus(result) != PGRES_TUPLES_OK || PQntuples(result) == 0)
	{
		LOG_MESSAGE(CRITICAL) << "Error fetching account ID for email: " << email << std::endl;
		PQclear(result);
		return;
	}

	std::string accountId = PQgetvalue(result, 0, 0);
	PQclear(result);

	const char* getSubscriptionIdSQL = R"(
        SELECT id 
        FROM subscriptions 
        WHERE account_id = $1 AND subscription_name = $2
        LIMIT 1;
    )";


	const char* paramsSubscription[] = { accountId.c_str(), name.c_str() };
	result = PQexecParams(conn, getSubscriptionIdSQL, 2, nullptr, paramsSubscription, nullptr, nullptr, 0);

	if (PQresultStatus(result) != PGRES_TUPLES_OK || PQntuples(result) == 0)
	{
		std::string errorMessage = PQerrorMessage(conn);
		LOG_MESSAGE(CRITICAL) << "Error fetching subscription ID for email: " << email << " and subscription name: " << name << std::endl;
		PQclear(result);
		return;
	}

	std::string subscriptionId = PQgetvalue(result, 0, 0);
	PQclear(result);

	const char* insertLicensePlateSQL = R"(
        INSERT INTO subscription_license_vehicles (subscription_id, license_plate) 
        VALUES ($1, $2);
    )";

	const char* paramsInsert[] = { subscriptionId.c_str(), licensePlate.c_str() };
	result = PQexecParams(conn, insertLicensePlateSQL, 2, nullptr, paramsInsert, nullptr, nullptr, 0);

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
	const char* sqlGetAccountId = R"(
        SELECT id FROM accounts WHERE email = $1;
    )";
	const char* paramsGetAccountId[] = { email.c_str() };
	PGresult* result = PQexecParams(conn, sqlGetAccountId, 1, nullptr, paramsGetAccountId, nullptr, nullptr, 0);

	if (PQresultStatus(result) != PGRES_TUPLES_OK || PQntuples(result) == 0)
	{
		LOG_MESSAGE(CRITICAL) << "Error retrieving account ID for email: " << email << std::endl;
		PQclear(result);
		return;
	}

	std::string accountId = PQgetvalue(result, 0, 0);
	PQclear(result);

	const char* sqlGetSubscriptionId = R"(
        SELECT id FROM subscriptions WHERE account_id = $1 AND subscription_name = $2;
    )";
	const char* paramsGetSubscriptionId[] = { accountId.c_str(), name.c_str() };
	result = PQexecParams(conn, sqlGetSubscriptionId, 2, nullptr, paramsGetSubscriptionId, nullptr, nullptr, 0);

	if (PQresultStatus(result) != PGRES_TUPLES_OK || PQntuples(result) == 0)
	{
		LOG_MESSAGE(CRITICAL) << "Error retrieving subscription ID for subscription: " << name << " and email: " << email << std::endl;
		PQclear(result);
		return;
	}

	std::string subscriptionId = PQgetvalue(result, 0, 0);
	PQclear(result);

	const char* sqlDeleteSubscription = R"(
        DELETE FROM subscriptions WHERE id = $1;
    )";
	const char* paramsDeleteSubscription[] = { subscriptionId.c_str() };
	result = PQexecParams(conn, sqlDeleteSubscription, 1, nullptr, paramsDeleteSubscription, nullptr, nullptr, 0);

	if (PQresultStatus(result) != PGRES_COMMAND_OK)
	{
		LOG_MESSAGE(CRITICAL) << "Error deleting subscription ID: " << subscriptionId << " for email: " << email << std::endl;
		PQclear(result);
		return;
	}

	PQclear(result);
}

void DatabaseManager::deleteLicensePlate(const std::string& email, const std::string& name, const std::string& licensePlate)
{
	const char* selectIdSQL = R"(
        SELECT subscription_id FROM subscription_license_vehicles 
        WHERE license_plate = $1;
    )";
	const char* paramsSelect[] = { licensePlate.c_str() };
	PGresult* result = PQexecParams(conn, selectIdSQL, 1, nullptr, paramsSelect, nullptr, nullptr, 0);

	std::string subscriptionId = "-1";
	if (PQresultStatus(result) == PGRES_TUPLES_OK && PQntuples(result) > 0)
		subscriptionId = PQgetvalue(result, 0, 0);
	else
	{
		std::string errorMessage = PQerrorMessage(conn);
		LOG_MESSAGE(CRITICAL) << "License plate not found: " << licensePlate << std::endl;
		PQclear(result);
		return;
	}

	PQclear(result);

	if (subscriptionId != "-1")
	{
		const char* deleteLinkSQL = R"(
            DELETE FROM subscription_license_vehicles
            WHERE subscription_id = $1 AND license_plate = $2;
        )";
		const char* paramsDeleteLink[] = { subscriptionId.c_str(), licensePlate.c_str() };
		result = PQexecParams(conn, deleteLinkSQL, 2, nullptr, paramsDeleteLink, nullptr, nullptr, 0);

		if (PQresultStatus(result) != PGRES_COMMAND_OK)
		{
			std::string errorMessage = PQerrorMessage(conn);
			LOG_MESSAGE(CRITICAL) << "Error deleting license plate link for plate: " << licensePlate << " and subscription: " << name << std::endl;
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
