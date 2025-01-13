#pragma once

#ifdef ACCOUNT_EXPORTS
#define ACCOUNT_API __declspec(dllexport)
#else
#define ACCOUNT_API __declspec(dllimport)
#endif

#include <string>
#include <functional>

class ACCOUNT_API Account
{
public:
	Account(const std::string& name, const std::string& lastName, const std::string& email, const std::string& password, const std::string& phone)
		: name(name),
		lastName(lastName),
		email(email),
		password(password),
		phone(phone)
	{}

	Account() = default;

	Account(const Account&) = default;

	Account(Account&&) noexcept = default;

	Account& operator=(const Account&) = default;

	Account& operator=(Account&&) noexcept = default;

public:
	std::string getName() const;

	void setName(const std::string& name);

	std::string getLastName() const;

	void setLastName(const std::string& lastName);

	std::string getEmail() const;

	void setEmail(const std::string& email);

	std::string getPassword() const;

	void setPassword(const std::string& password);

	std::string getPhone() const;

	void setPhone(const std::string& phone);

	bool operator<(const Account& account) const;

	bool operator==(const Account& account) const;

private:
	std::string name;
	std::string lastName;
	std::string email;
	std::string password;
	std::string phone;
};

struct AccountHash
{
	std::size_t operator()(const Account& account) const
	{
		return std::hash<std::string>{}(account.getEmail());
	}
};