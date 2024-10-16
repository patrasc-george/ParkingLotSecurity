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
	Account(const std::string& email, const std::string& password) : email(email), password(password) {}

	Account() = default;

	Account(const Account&) = default;

	Account(Account&&) noexcept = default;

	Account& operator=(const Account&) = default;

	Account& operator=(Account&&) noexcept = default;

public:
	std::string getEmail() const;

	void setEmail(const std::string& email);

	std::string getPassword() const;

	void setPassword(const std::string& password);

	bool operator<(const Account& account) const;

	bool operator==(const Account& account) const;

private:
	std::string email;
	std::string password;
};

struct AccountHash
{
	std::size_t operator()(const Account& account) const
	{
		return std::hash<std::string>{}(account.getEmail());
	}
};