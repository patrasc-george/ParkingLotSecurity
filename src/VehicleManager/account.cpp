#include "account.h"

std::string Account::getName() const
{
	return name;
}

void Account::setName(const std::string& name)
{
	if (name.empty())
		return;

	this->name = name;
}

std::string Account::getLastName() const
{
	return lastName;
}

void Account::setLastName(const std::string& lastName)
{
	if (lastName.empty())
		return;

	this->lastName = lastName;
}

std::string Account::getEmail() const
{
	return email;
}

void Account::setEmail(const std::string& email)
{
	if (email.empty())
		return;

	this->email = email;
}

std::string Account::getPassword() const
{
	return password;
}

void Account::setPassword(const std::string& password)
{
	if (password.empty())
		return;

	this->password = password;
}

std::string Account::getPhone() const
{
	return phone;
}

void Account::setPhone(const std::string& phone)
{
	if (phone.empty())
		return;

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
