#include "account.h"

std::string Account::getEmail() const
{
	return email;
}

std::string Account::getPassword() const
{
	return password;
}

bool Account::operator<(const Account& account) const
{
	return email < account.email;
}

bool Account::operator==(const Account& account) const
{
	return email == account.email && password == account.password;
}
