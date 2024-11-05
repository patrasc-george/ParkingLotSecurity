#include "databasemanager.h"
#include <sys/stat.h>
#include <ctime>
#include <fstream>

bool DatabaseManager::initializeDatabase(const std::string& password)
{
	sqlite3_open((path + "/database.db").c_str(), &db);
	sqlite3_key(db, password.c_str(), password.size());

	const char* sqlCreateTables = R"(
			CREATE TABLE IF NOT EXISTS vehicles (
				id INTEGER PRIMARY KEY,
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
				password TEXT NOT NULL,
				phone TEXT NOT NULL
			);

			CREATE TABLE IF NOT EXISTS payments (
				id INTEGER PRIMARY KEY AUTOINCREMENT,
				date DATE NOT NULL
			);

			CREATE TABLE IF NOT EXISTS licensePlates (
				id INTEGER PRIMARY KEY AUTOINCREMENT,
				number TEXT NOT NULL
			);

			CREATE TABLE IF NOT EXISTS subscriptions (
				name TEXT PRIMARY KEY
			);

			CREATE TABLE IF NOT EXISTS subscriptionPayments (
				subscription_name TEXT,
				payment_id INTEGER,
				PRIMARY KEY (subscription_name, payment_id),
				FOREIGN KEY (subscription_name) REFERENCES accountsSubscriptions(subscription_name) ON DELETE CASCADE,
				FOREIGN KEY (payment_id) REFERENCES payments(id) ON DELETE CASCADE
			);

			CREATE TABLE IF NOT EXISTS subscriptionLicensePlates (
				subscription_name TEXT,
				license_plate_id INTEGER,
				PRIMARY KEY (subscription_name, license_plate_id),
				FOREIGN KEY (subscription_name) REFERENCES accountsSubscriptions(subscription_name) ON DELETE CASCADE,
				FOREIGN KEY (license_plate_id) REFERENCES licensePlates(id) ON DELETE CASCADE
			);

			CREATE TABLE IF NOT EXISTS accountsSubscriptions (
				email TEXT,
				subscription_name TEXT,
				FOREIGN KEY (email) REFERENCES accounts(email) ON DELETE CASCADE,
				PRIMARY KEY (email, subscription_name)
			);
    )";

	if (sqlite3_exec(db, sqlCreateTables, nullptr, nullptr, nullptr) == SQLITE_NOTADB)
		return false;

	return true;
}

DatabaseManager& DatabaseManager::getInstance(const std::string& path)
{
	static DatabaseManager instance(path);
	return instance;
}

std::string DatabaseManager::getPath() const
{
	return path;
}

std::vector<std::string> DatabaseManager::getVehicles() const
{
	std::vector<std::string> vehicles;
	const char* sql = "SELECT id, image_path, license_plate, date_time, ticket, time_parked, total_amount, is_paid FROM vehicles;";
	sqlite3_stmt* stmt;

	if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
		return vehicles;

	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		int id = sqlite3_column_int(stmt, 0);
		std::string imagePath = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
		std::string licensePlate = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
		std::string dateTime = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
		std::string ticket = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
		std::string timeParked = (sqlite3_column_type(stmt, 5) == SQLITE_NULL) ? "" : reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
		int totalAmount = (sqlite3_column_type(stmt, 6) == SQLITE_NULL) ? 0 : sqlite3_column_int(stmt, 6);
		std::string isPaid = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));

		std::string vehicleInfo = std::to_string(id) + ", " + imagePath + ", " + licensePlate + ", " + dateTime + ", " + ticket + ", " + timeParked + ", " + std::to_string(totalAmount) + ", " + isPaid;

		vehicles.push_back(vehicleInfo);
	}

	sqlite3_finalize(stmt);
	return vehicles;
}

std::vector<std::string> DatabaseManager::getAccounts() const
{
	std::vector<std::string> accounts;
	const char* sql = "SELECT name, email, password, phone FROM accounts;";
	sqlite3_stmt* stmt;

	if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
		return accounts;

	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
		std::string email = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
		std::string password = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
		std::string phone = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
		accounts.push_back(name + ", " + email + ", " + password + ", " + phone);
	}

	sqlite3_finalize(stmt);
	return accounts;
}

std::unordered_map<std::string, std::pair<std::string, std::string>> DatabaseManager::getSubscriptions(const std::string& email) const
{
	std::unordered_map<std::string, std::pair<std::string, std::string>> subscriptions;

	const char* sqlGetSubscriptions = R"(
        SELECT subscription_name
        FROM accountsSubscriptions
        WHERE email = ?;
    )";

	sqlite3_stmt* stmtGetSubscriptions;
	sqlite3_stmt* stmtGetDetails;

	if (sqlite3_prepare_v2(db, sqlGetSubscriptions, -1, &stmtGetSubscriptions, nullptr) != SQLITE_OK)
		return subscriptions;

	sqlite3_bind_text(stmtGetSubscriptions, 1, email.c_str(), -1, SQLITE_STATIC);

	while (sqlite3_step(stmtGetSubscriptions) == SQLITE_ROW)
	{
		std::string subscriptionName = reinterpret_cast<const char*>(sqlite3_column_text(stmtGetSubscriptions, 0));
		subscriptions[subscriptionName] = {};
	}

	sqlite3_finalize(stmtGetSubscriptions);

	const char* sqlGetDetails = R"(
        SELECT p.date, l.number
        FROM subscriptionPayments sp
        LEFT JOIN payments p ON sp.payment_id = p.id
        LEFT JOIN subscriptionLicensePlates slp ON sp.subscription_name = slp.subscription_name
        LEFT JOIN licensePlates l ON slp.license_plate_id = l.id
        WHERE sp.subscription_name = ?;
    )";

	for (auto& pair : subscriptions)
	{
		std::string payments;
		std::string licensePlates;

		if (sqlite3_prepare_v2(db, sqlGetDetails, -1, &stmtGetDetails, nullptr) != SQLITE_OK)
			continue;

		sqlite3_bind_text(stmtGetDetails, 1, pair.first.c_str(), -1, SQLITE_STATIC);

		while (sqlite3_step(stmtGetDetails) == SQLITE_ROW)
		{
			const unsigned char* paymentText = sqlite3_column_text(stmtGetDetails, 0);
			if (paymentText)
			{
				std::string payment(reinterpret_cast<const char*>(paymentText));
				if (!payments.empty())
					payments += ", ";
				payments += payment;
			}

			const unsigned char* licensePlateText = sqlite3_column_text(stmtGetDetails, 1);
			if (licensePlateText)
			{
				std::string licensePlate(reinterpret_cast<const char*>(licensePlateText));
				if (!licensePlates.empty())
					licensePlates += ", ";
				licensePlates += licensePlate;
			}
		}

		sqlite3_finalize(stmtGetDetails);

		pair.second.first = payments;
		pair.second.second = licensePlates;
	}

	return subscriptions;
}

void DatabaseManager::setIsPaid(const int& id)
{
	const char* sqlUpdate = "UPDATE vehicles SET is_paid = 'true' WHERE id = ?;";
	sqlite3_stmt* stmt;

	if (sqlite3_prepare_v2(db, sqlUpdate, -1, &stmt, nullptr) != SQLITE_OK)
		return;

	sqlite3_bind_int(stmt, 1, id);

	if (sqlite3_step(stmt) != SQLITE_DONE)
		return;

	sqlite3_finalize(stmt);
}

void DatabaseManager::setName(const std::string& email, const std::string& newName)
{
	const char* sql = "UPDATE accounts SET name = ? WHERE email = ?";

	sqlite3_stmt* stmt;
	sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

	sqlite3_bind_text(stmt, 1, newName.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 2, email.c_str(), -1, SQLITE_STATIC);

	sqlite3_step(stmt);

	sqlite3_finalize(stmt);
}

void DatabaseManager::setEmail(const std::string& email, const std::string& newEmail)
{
	const char* sql = "UPDATE accounts SET email = ? WHERE email = ?";

	sqlite3_stmt* stmt;
	sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

	sqlite3_bind_text(stmt, 1, newEmail.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 2, email.c_str(), -1, SQLITE_STATIC);

	sqlite3_step(stmt);

	sqlite3_finalize(stmt);
}

void DatabaseManager::setPassword(const std::string& email, const std::string& newPassword)
{
	const char* sql = "UPDATE accounts SET password = ? WHERE email = ?";

	sqlite3_stmt* stmt;
	sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

	sqlite3_bind_text(stmt, 1, newPassword.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 2, email.c_str(), -1, SQLITE_STATIC);

	sqlite3_step(stmt);

	sqlite3_finalize(stmt);
}

void DatabaseManager::setPhone(const std::string& email, const std::string& newPhone)
{
	const char* sql = "UPDATE accounts SET phone = ? WHERE email = ?";

	sqlite3_stmt* stmt;
	sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

	sqlite3_bind_text(stmt, 1, newPhone.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 2, email.c_str(), -1, SQLITE_STATIC);

	sqlite3_step(stmt);

	sqlite3_finalize(stmt);
}

void DatabaseManager::addVehicle(const int& id, const std::string& imagePath, const std::string& licensePlate, const std::string& dateTime, const std::string& ticket, const std::string& timeParked, const int& totalAmount, const std::string& isPaid)
{
	const char* sql = R"(
        INSERT INTO vehicles (id, image_path, license_plate, date_time, ticket, time_parked, total_amount, is_paid)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?);
    )";

	sqlite3_stmt* stmt;

	if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
		return;

	sqlite3_bind_int(stmt, 1, id);
	sqlite3_bind_text(stmt, 2, imagePath.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 3, licensePlate.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 4, dateTime.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 5, ticket.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 6, timeParked.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_double(stmt, 7, totalAmount);
	sqlite3_bind_text(stmt, 8, isPaid.c_str(), -1, SQLITE_STATIC);

	if (sqlite3_step(stmt) != SQLITE_DONE)
	{
		sqlite3_finalize(stmt);
		return;
	}

	sqlite3_finalize(stmt);
}

void DatabaseManager::addAccount(const std::string& name, const std::string& email, const std::string& password, const std::string& phone)
{
	const char* sql = R"(
        INSERT INTO accounts (name, email, password, phone)
        VALUES (?, ?, ?, ?);
    )";

	sqlite3_stmt* stmt;

	if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
	{
		sqlite3_finalize(stmt);
		return;
	}

	sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 2, email.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 3, password.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 4, phone.c_str(), -1, SQLITE_STATIC);

	if (sqlite3_step(stmt) != SQLITE_DONE)
	{
		sqlite3_finalize(stmt);
		return;
	}

	sqlite3_finalize(stmt);
}

void DatabaseManager::addSubscription(const std::string& email, const std::string& name)
{
	sqlite3_stmt* stmt;

	const char* sqlInsertPayment = R"(
        INSERT INTO payments (date)
        VALUES (?);
    )";

	if (sqlite3_prepare_v2(db, sqlInsertPayment, -1, &stmt, nullptr) != SQLITE_OK)
	{
		sqlite3_finalize(stmt);
		return;
	}

	time_t now = time(0);
	tm* ltm = localtime(&now);
	char buffer[11];
	snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d", 1900 + ltm->tm_year, 1 + ltm->tm_mon, ltm->tm_mday);
	std::string currentDate = std::string(buffer);

	sqlite3_bind_text(stmt, 1, currentDate.c_str(), -1, SQLITE_STATIC);
	if (sqlite3_step(stmt) != SQLITE_DONE)
	{
		sqlite3_finalize(stmt);
		return;
	}

	int paymentId = sqlite3_last_insert_rowid(db);
	sqlite3_finalize(stmt);

	const char* sqlInsertSubscription = R"(
        INSERT INTO subscriptions (name)
        VALUES (?);
    )";

	if (sqlite3_prepare_v2(db, sqlInsertSubscription, -1, &stmt, nullptr) != SQLITE_OK)
	{
		sqlite3_finalize(stmt);
		return;
	}

	sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
	if (sqlite3_step(stmt) != SQLITE_DONE)
	{
		sqlite3_finalize(stmt);
		return;
	}

	sqlite3_finalize(stmt);

	const char* sqlLinkAccountSubscription = R"(
        INSERT INTO accountsSubscriptions (email, subscription_name)
        VALUES (?, ?);
    )";

	if (sqlite3_prepare_v2(db, sqlLinkAccountSubscription, -1, &stmt, nullptr) != SQLITE_OK)
	{
		sqlite3_finalize(stmt);
		return;
	}

	sqlite3_bind_text(stmt, 1, email.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 2, name.c_str(), -1, SQLITE_STATIC);
	if (sqlite3_step(stmt) != SQLITE_DONE)
	{
		sqlite3_finalize(stmt);
		return;
	}

	sqlite3_finalize(stmt);

	const char* sqlLinkSubscriptionPayment = R"(
        INSERT INTO subscriptionPayments (subscription_name, payment_id)
        VALUES (?, ?);
    )";

	if (sqlite3_prepare_v2(db, sqlLinkSubscriptionPayment, -1, &stmt, nullptr) != SQLITE_OK)
	{
		sqlite3_finalize(stmt);
		return;
	}

	sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_int(stmt, 2, paymentId);
	if (sqlite3_step(stmt) != SQLITE_DONE)
	{
		sqlite3_finalize(stmt);
		return;
	}

	sqlite3_finalize(stmt);
}

void DatabaseManager::addLicensePlate(const std::string& email, const std::string& name, const std::string& licensePlate)
{
	sqlite3_stmt* stmt;

	const char* insertLicensePlateSQL = R"(
        INSERT INTO licensePlates (number) 
        VALUES (?);
    )";

	if (sqlite3_prepare_v2(db, insertLicensePlateSQL, -1, &stmt, nullptr) != SQLITE_OK)
		return;

	sqlite3_bind_text(stmt, 1, licensePlate.c_str(), -1, SQLITE_STATIC);

	if (sqlite3_step(stmt) != SQLITE_DONE)
	{
		sqlite3_finalize(stmt);
		return;
	}
	sqlite3_finalize(stmt);

	int licensePlateId = sqlite3_last_insert_rowid(db);

	const char* selectSubscriptionSQL = R"(
        SELECT subscription_name
        FROM accountsSubscriptions
        WHERE email = ? AND subscription_name = ?;
    )";

	if (sqlite3_prepare_v2(db, selectSubscriptionSQL, -1, &stmt, nullptr) != SQLITE_OK)
		return;

	sqlite3_bind_text(stmt, 1, email.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 2, name.c_str(), -1, SQLITE_STATIC);

	std::string subscriptionName;
	if (sqlite3_step(stmt) == SQLITE_ROW)
		subscriptionName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
	else
	{
		sqlite3_finalize(stmt);
		return;
	}
	sqlite3_finalize(stmt);

	const char* insertSubscriptionLicensePlateSQL = R"(
        INSERT INTO subscriptionLicensePlates (subscription_name, license_plate_id)
        VALUES (?, ?);
    )";

	if (sqlite3_prepare_v2(db, insertSubscriptionLicensePlateSQL, -1, &stmt, nullptr) != SQLITE_OK)
		return;

	sqlite3_bind_text(stmt, 1, subscriptionName.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_int(stmt, 2, licensePlateId);

	if (sqlite3_step(stmt) != SQLITE_DONE)
		return;

	sqlite3_finalize(stmt);
}

void DatabaseManager::deleteSubscription(const std::string& email, const std::string& name)
{
	const char* sqlDeleteLink = R"(
        DELETE FROM accountsSubscriptions
        WHERE email = ? AND subscription_name = ?;
    )";

	sqlite3_stmt* stmt;
	if (sqlite3_prepare_v2(db, sqlDeleteLink, -1, &stmt, nullptr) != SQLITE_OK)
	{
		sqlite3_finalize(stmt);
		return;
	}

	sqlite3_bind_text(stmt, 1, email.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 2, name.c_str(), -1, SQLITE_STATIC);

	if (sqlite3_step(stmt) != SQLITE_DONE)
	{
		sqlite3_finalize(stmt);
		return;
	}

	sqlite3_finalize(stmt);

	const char* sqlCheckSubscription = R"(
        SELECT COUNT(*) FROM accountsSubscriptions
        WHERE subscription_name = ?;
    )";

	if (sqlite3_prepare_v2(db, sqlCheckSubscription, -1, &stmt, nullptr) != SQLITE_OK)
	{
		sqlite3_finalize(stmt);
		return;
	}

	sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);

	int count = 0;
	if (sqlite3_step(stmt) == SQLITE_ROW)
		count = sqlite3_column_int(stmt, 0);

	sqlite3_finalize(stmt);

	if (count)
		return;

	const char* sqlDeleteSubscription = R"(
            DELETE FROM subscriptions
            WHERE name = ?;
        )";

	if (sqlite3_prepare_v2(db, sqlDeleteSubscription, -1, &stmt, nullptr) != SQLITE_OK)
	{
		sqlite3_finalize(stmt);
		return;
	}

	sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);

	if (sqlite3_step(stmt) != SQLITE_DONE)
	{
		sqlite3_finalize(stmt);
		return;
	}

	sqlite3_finalize(stmt);
}

void DatabaseManager::deleteLicensePlate(const std::string& email, const std::string& name, const std::string& licensePlate)
{
	sqlite3_stmt* stmt;

	const char* selectIdSQL = R"(
        SELECT id FROM licensePlates 
        WHERE number = ?;
    )";

	if (sqlite3_prepare_v2(db, selectIdSQL, -1, &stmt, nullptr) != SQLITE_OK)
		return;

	sqlite3_bind_text(stmt, 1, licensePlate.c_str(), -1, SQLITE_STATIC);

	int licensePlateId = -1;
	if (sqlite3_step(stmt) == SQLITE_ROW)
		licensePlateId = sqlite3_column_int(stmt, 0);
	else
	{
		sqlite3_finalize(stmt);
		return;
	}
	sqlite3_finalize(stmt);

	const char* deleteLinkSQL = R"(
        DELETE FROM subscriptionLicensePlates
        WHERE license_plate_id = ?;
    )";

	if (sqlite3_prepare_v2(db, deleteLinkSQL, -1, &stmt, nullptr) != SQLITE_OK)
		return;

	sqlite3_bind_int(stmt, 1, licensePlateId);

	if (sqlite3_step(stmt) != SQLITE_DONE)
	{
		sqlite3_finalize(stmt);
		return;
	}
	sqlite3_finalize(stmt);

	const char* deleteLicensePlateSQL = R"(
        DELETE FROM licensePlates
        WHERE id = ?;
    )";

	if (sqlite3_prepare_v2(db, deleteLicensePlateSQL, -1, &stmt, nullptr) != SQLITE_OK)
		return;

	if (sqlite3_step(stmt) != SQLITE_DONE)
		return;

	sqlite3_bind_int(stmt, 1, licensePlateId);
	sqlite3_finalize(stmt);
}

DatabaseManager::~DatabaseManager()
{
	if (db)
	{
		sqlite3_close(db);
		db = nullptr;
	}
}
