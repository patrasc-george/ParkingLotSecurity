#include "account.h"

std::string Account::getEmail() const
{
	return email;
}

void Account::setEmail(const std::string& email)
{
	this->email = email;
}

std::string Account::getPassword() const
{
	return password;
}

void Account::setPassword(const std::string& password)
{
	this->password = password;
}

bool Account::operator<(const Account& account) const
{
	return email < account.email;
}

bool Account::operator==(const Account& account) const
{
	return email == account.email && password == account.password;
}
