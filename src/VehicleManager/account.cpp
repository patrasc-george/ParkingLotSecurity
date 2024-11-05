#include "account.h"

std::string Account::getName() const
{
	return name;
}

void Account::setName(const std::string& name)
{
	this->name = name;
}

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

std::string Account::getPhone() const
{
	return phone;
}

void Account::setPhone(const std::string& phone)
{
	this->phone = phone;
}

bool Account::operator<(const Account& account) const
{
	return email < account.email;
}

bool Account::operator==(const Account& account) const
{
	return email == account.email && password == account.password;
}
