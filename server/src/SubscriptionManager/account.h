#pragma once

#ifdef _WIN32
#ifdef ACCOUNT_EXPORTS
#define ACCOUNT_API __declspec(dllexport)
#else
#define ACCOUNT_API __declspec(dllimport)
#endif
#elif __linux__
#define ACCOUNT_API __attribute__((visibility("default")))
#else
#define ACCOUNT_API
#endif

#include <string>
#include <functional>

/**
 * @class Account
 * @brief Represents a user account with personal and contact information.
 *
 * The `Account` class holds the details of a user account including name, last name,
 * email, password, and phone number. This class provides getters and setters for each
 * field, along with comparison operators and a hash function for account-based collections.
 * It supports move and copy operations.
 */
class ACCOUNT_API Account
{
public:
	/**
	 * @brief Constructs an Account with all provided details.
	 * @details This constructor initializes all account fields using the provided parameters.
	 * Each parameter is assigned to its corresponding member variable.
	 * @param[in] name The name of the account holder.
	 * @param[in] lastName The last name of the account holder.
	 * @param[in] email The email associated with the account (must be unique).
	 * @param[in] password The account password. Consider encrypting or hashing it before use.
	 * @param[in] phone The phone number associated with the account, used for contact or recovery purposes.
	 */
	Account(const std::string& name, const std::string& lastName, const std::string& email, const std::string& password, const std::string& phone)
		: name(name),
		lastName(lastName),
		email(email),
		password(password),
		phone(phone)
	{}

	/**
	 * @brief Default constructor.
	 * @details Initializes an empty account object. All fields are left uninitialized or set to default values.
	 */
	Account() = default;

	/**
	 * @brief Copy constructor.
	 * @details Creates a new account as a copy of another. All fields are copied as-is.
	 * @param[in] other The account object to copy from.
	 */
	Account(const Account&) = default;

	/**
	 * @brief Move constructor.
	 * @details Transfers ownership of resources from another account object.
	 * After the move, the original object is left in a valid but unspecified state.
	 * @param[in] other The account object to move from.
	 */
	Account(Account&&) noexcept = default;

	/**
	 * @brief Copy assignment operator.
	 * @details Assigns all fields from another account to this one, overwriting existing values.
	 * @param[in] other The account object to copy from.
	 * @return A reference to the updated account object.
	 */
	Account& operator=(const Account&) = default;

	/**
	 * @brief Move assignment operator.
	 * @details Transfers ownership of resources from another account to this one. The original account is left in a valid but unspecified state.
	 * @param[in] other The account object to move from.
	 * @return A reference to the updated account object.
	 */
	Account& operator=(Account&&) noexcept = default;


public:
	/**
	 * @brief Gets the name of the account.
	 * @details This function retrieves the name stored in the account object. It does not modify any member variables.
	 * @return The name of the account as a std::string.
	 */
	std::string getName() const;

	/**
	 * @brief Sets the name of the account.
	 * @details If the provided name is empty, the operation is ignored. Otherwise, the internal name is updated.
	 * @param[in] name The new name to be set. If empty, the operation is ignored.
	 */
	void setName(const std::string& name);

	/**
	 * @brief Gets the last name of the account.
	 * @details This function retrieves the last name stored in the account object. It does not perform any modification.
	 * @return The last name of the account as a std::string.
	 */
	std::string getLastName() const;

	/**
	 * @brief Sets the last name of the account.
	 * @details If the provided last name is empty, the internal value is not changed.
	 * @param[in] lastName The new last name to be set. If empty, the operation is ignored.
	 */
	void setLastName(const std::string& lastName);

	/**
	 * @brief Gets the email associated with the account.
	 * @details Returns the email string that is used to identify the account. This value must be unique for each account.
	 * @return The email of the account as a std::string.
	 */
	std::string getEmail() const;

	/**
	 * @brief Sets the email of the account.
	 * @details If the provided email is empty, the operation is ignored. The email is essential for account identification.
	 * @param[in] email The new email to be set. If empty, the operation is ignored.
	 */
	void setEmail(const std::string& email);

	/**
	 * @brief Gets the password associated with the account.
	 * @details This function returns the current password in plain text. The password should be stored securely (e.g., hashed).
	 * @return The password of the account as a std::string.
	 */
	std::string getPassword() const;

	/**
	 * @brief Sets the password for the account.
	 * @details If the provided password is empty, the internal value is not updated. Consider encrypting or hashing the password before setting it.
	 * @param[in] password The new password to be set. If empty, the operation is ignored.
	 */
	void setPassword(const std::string& password);

	/**
	 * @brief Gets the phone number associated with the account.
	 * @details Retrieves the phone number linked to the account, which can be used for communication or two-factor authentication.
	 * @return The phone number of the account as a std::string.
	 */
	std::string getPhone() const;

	/**
	 * @brief Sets the phone number for the account.
	 * @details Updates the phone number if the provided value is non-empty. Phone numbers can be used for contact purposes or account recovery.
	 * @param[in] phone The new phone number to be set. If empty, the operation is ignored.
	 */
	void setPhone(const std::string& phone);

	/**
	 * @brief Compares two accounts based on their email addresses.
	 * @details The comparison is lexicographical, following standard `std::string` comparison rules. Used for sorting or ordered collections of accounts.
	 * @param[in] account The account to be compared with.
	 * @return True if the email of the current account is lexicographically smaller than the given account's email.
	 */
	bool operator<(const Account& account) const;

	/**
	 * @brief Compares two accounts for equality based on email and password.
	 * @details Two accounts are considered equal if both the email and password match exactly. This is useful for authentication or account validation.
	 * @param[in] account The account to be compared with.
	 * @return True if both the email and password are the same for both accounts.
	 */
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