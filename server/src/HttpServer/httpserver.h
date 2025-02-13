#pragma once

#include "httplib.h"
#include "subscriptionmanager.h"
#include "websocketserver.h"
#include "logger.h"

#include <thread>
#include <iostream>
#include <Poco/Net/SMTPClientSession.h>
#include <Poco/Net/MailMessage.h>
#include <Poco/Net/SSLManager.h>
#include <Poco/Net/ConsoleCertificateHandler.h>
#include <Poco/Net/AcceptCertificateHandler.h>
#include <Poco/Net/SecureStreamSocket.h>
#include <Poco/Net/SecureSMTPClientSession.h>
#include <Poco/Net/DNS.h>
#include <Poco/Net/MailMessage.h>
#include <Poco/AutoPtr.h>
#include <Poco/UUIDGenerator.h>
#include <Poco/DigestEngine.h>
#include <Poco/SHA1Engine.h>
#include <Poco/Random.h>
#include <Poco/RandomStream.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>
#include <Poco/JSON/Object.h>

/**
 * @class HttpServer
 * @brief A class for managing an HTTP server that handles various API requests.
 *
 * This class represents an HTTP server that is capable of managing a variety of API endpoints related to account management, parking validation, subscription management, and communication (email, SMS).
 * It initializes the server, sets up WebSocket communication, and configures HTTP endpoints to handle requests such as account creation, login, password recovery, vehicle management, and others.
 * The server also ensures secure communication via API keys and handles background operations such as sending emails and SMS notifications.
 * The server operates in a multithreaded environment where WebSocket communication runs in the background.
 */
class HttpServer
{
public:
	/**
	 * @brief Constructs an HttpServer instance, initializes logging, and sets up WebSocket and HTTP endpoints.
	 * @details This constructor sets up the server with necessary configurations for handling API requests.
	 * It initializes a logger, configures an authentication key, and starts a background thread to
	 * run a WebSocket server. The WebSocket server listens on port 9002.
	 * Additionally, the constructor sets up various HTTP API endpoints with `OPTIONS` and `POST` methods,
	 * each handling specific routes for the application's functionality such as account creation, validation,
	 * password recovery, and vehicle management.
	 * Each of these routes sets appropriate CORS headers to allow cross-origin requests and ensure secure interaction
	 * between the client and server.
	 */
	HttpServer();

	/**
	 * @brief Destructor for the HttpServer class.
	 * @details Ensures the server is properly stopped and the thread is joined before destruction to avoid resource leaks.
	 */
	~HttpServer();

private:
	/**
	 * @brief Generates a random token with a 6-digit numeric value.
	 * @details This function uses a random number generator to generate a 6-digit numeric token.
	 *          It uses a random device to seed the generator and a uniform distribution to ensure
	 *          that the token is always between 100000 and 999999. The token is then converted to a string
	 *          and returned as the result.
	 * @return A string representing a randomly generated 6-digit token.
	 */
	std::string generateToken();

	/**
	 * @brief Sends an email with the specified subject and content.
	 * @details This function sends an email using the POCO library's SMTP client. It retrieves the email
	 *          credentials (email address and password) from environment variables. If these credentials
	 *          are not set, a critical log message is generated. The function then creates a mail message
	 *          with the provided subject and content, and sends it using a secure SMTP session. In case of failure,
	 *          an exception is caught, logged, and the function returns false.
	 * @param[in] email The recipient's email address.
	 * @param[in] subject The subject of the email.
	 * @param[in] content The content/body of the email.
	 * @return Returns true if the email was sent successfully, otherwise false.
	 */
	bool sendEmail(const std::string& email, const std::string& subject, const std::string& content);

	/**
	 * @brief Sends an SMS to a specified phone number with the provided content.
	 * @details This function sends an SMS using the Twilio API. It retrieves the necessary Twilio credentials
	 *          (SID, Token, and Phone number) from environment variables. If these credentials are missing,
	 *          a critical log message is generated. The function uses these credentials to authenticate and
	 *          sends the SMS via a secure HTTPS connection. If the SMS fails to send or the response status is
	 *          not successful, the function logs an error and returns false.
	 * @param[in] phone The recipient's phone number (in international format, without a "+" symbol).
	 * @param[in] content The content of the SMS message.
	 * @return Returns true if the SMS was sent successfully, otherwise false.
	 */
	bool sendSMS(const std::string& phone, const std::string& content);

	/**
	 * @brief Handles POST requests for vehicle parking validation or QR code processing.
	 * @details This function processes POST requests by first checking the validity of the API key.
	 *          If valid, it checks for the presence of a "licensePlate" parameter or a file named "qrCodeImage".
	 *          If a license plate is provided, the function attempts to pay for the parking using the provided vehicle data.
	 *          If a QR code image is provided, it is decoded to extract the ticket number, which is then used for parking validation.
	 *          If the vehicle is found and the payment is successful, a success message is returned with the vehicle's details.
	 *          If any error occurs during the validation or payment process, an error message is returned.
	 * @param[in] request The HTTP request object containing the incoming parameters and files.
	 * @param[out] response The HTTP response object to be populated with the response data.
	 */
	void post(const httplib::Request& request, httplib::Response& response);

	/**
	 * @brief Creates a new user account after validating input data.
	 * @details This function processes POST requests for creating new accounts. It first checks the API key validity,
	 *          and then retrieves the parameters such as name, last name, email, password, phone, and captcha token.
	 *          The captcha token is verified through a third-party service (Google reCAPTCHA). If the captcha is valid,
	 *          it checks if the email and phone number are already associated with an existing account. If either already
	 *          exists, an error message is returned. If all checks pass, a temporary account is created. A success message
	 *          is returned if the account is created successfully, otherwise an error message is returned.
	 * @param[in] request The HTTP request object containing the account creation parameters.
	 * @param[out] response The HTTP response object to be populated with the response data.
	 */
	void createAccount(const httplib::Request& request, httplib::Response& response);

	/**
	 * @brief Sends a validation email to the user to confirm their account.
	 * @details This function generates a token for email validation and sends a confirmation email to the user.
	 *          The email contains a link with the generated token that the user needs to follow to validate their account.
	 *          If the email is successfully sent, a success message is returned. If the email fails to send, an error message
	 *          is returned with a reason for the failure.
	 * @param[in] request The HTTP request object containing the user's name and email address for validation.
	 * @param[out] response The HTTP response object to be populated with the response data.
	 */
	void validateViaEmail(const httplib::Request& request, httplib::Response& response);

	/**
	 * @brief Sends an SMS validation code for the user's account.
	 * @details This function generates a unique validation token and sends it via SMS to the phone number provided in the request.
	 *          The request must contain the user's email and phone number. The function first verifies the API key.
	 *          If the validation SMS is sent successfully, a success message is returned. If sending the SMS fails, an error message is returned.
	 * @param[in] request The HTTP request object containing the user's email and phone number for SMS validation.
	 * @param[out] response The HTTP response object to be populated with the response data.
	 */
	void validateViaSMS(const httplib::Request& request, httplib::Response& response);

	/**
	 * @brief Resends the SMS validation code to the user.
	 * @details This function retrieves the stored validation token for the specified email and sends it again via SMS to the provided phone number.
	 *          The request must contain the user's email and phone number. The function first verifies the API key.
	 *          If the SMS is resent successfully, a success message is returned. If sending the SMS fails, an error message is returned.
	 * @param[in] request The HTTP request object containing the user's email and phone number for resending the SMS validation code.
	 * @param[out] response The HTTP response object to be populated with the response data.
	 */
	void resendValidateSMS(const httplib::Request& request, httplib::Response& response);

	/**
	 * @brief Validates the user's account based on the provided token.
	 * @details This function verifies the provided token and, if valid, adds the corresponding account to the system.
	 *          The request must contain the token for validation. If the token is valid, the account is added, and a success message with the account's email is returned.
	 *          If the token is invalid, an error message is returned.
	 * @param[in] request The HTTP request object containing the token for account validation.
	 * @param[out] response The HTTP response object to be populated with the response data.
	 */
	void validate(const httplib::Request& request, httplib::Response& response);

	/**
	 * @brief Handles the user login process.
	 * @details This function processes login requests by verifying the provided credentials. It supports both admin and user logins.
	 *          For admin login, the function checks the provided password against the stored admin credentials.
	 *          For user login, the function verifies the email and password or handles login from a redirect.
	 *          If login is successful, user details and subscriptions are returned. If login fails, an error message is returned.
	 * @param[in] request The HTTP request object containing the user's input credentials (email, password, etc.).
	 * @param[out] response The HTTP response object to be populated with the response data.
	 */
	void login(const httplib::Request& request, httplib::Response& response);

	/**
	 * @brief Retrieves all email addresses associated with accounts in the system.
	 * @details This function checks the validity of the provided API key. If the key is valid, it fetches all email addresses stored in the system
	 *          and returns them in the response. If the API key is invalid, an error message is returned.
	 * @param[in] request The HTTP request object.
	 * @param[out] response The HTTP response object to be populated with the emails list.
	 */
	void getAdmin(const httplib::Request& request, httplib::Response& response);

	/**
	 * @brief Initiates password recovery via email.
	 * @details This function allows a user to recover their password by receiving a reset link through email. The request must contain the user's email address.
	 *          The function first validates the provided API key. If the email is valid and the account exists, a recovery token is generated and emailed
	 *          to the user with a link to reset the password. If any issue arises (e.g., invalid email or missing API key), an error response is sent.
	 * @param[in] request The HTTP request object containing the user's email for password recovery.
	 * @param[out] response The HTTP response object to be populated with the result of the password recovery request.
	 */
	void recoverPasswordViaEmail(const httplib::Request& request, httplib::Response& response);

	/**
	 * @brief Initiates password recovery via SMS.
	 * @details This function allows a user to recover their password by receiving a reset code via SMS. The request must contain the user's phone number.
	 *          The function first validates the provided API key. If the phone number is valid and the account exists, a recovery token is generated and sent
	 *          via SMS to the user. If any issue arises (e.g., invalid phone number or missing API key), an error response is sent.
	 * @param[in] request The HTTP request object containing the user's phone number for password recovery.
	 * @param[out] response The HTTP response object to be populated with the result of the password recovery request.
	 */
	void recoverPasswordViaSMS(const httplib::Request& request, httplib::Response& response);

	/**
	 * @brief Resends the password recovery code via SMS.
	 * @details This function allows a user to request a new password recovery code via SMS. The request must contain the user's phone number.
	 *          The function first validates the provided API key. If the phone number is valid, the system retrieves the stored recovery token and sends it again via SMS.
	 *          If any issue arises (e.g., invalid phone number or missing API key), an error response is sent.
	 * @param[in] request The HTTP request object containing the user's phone number for resending the password recovery code.
	 * @param[out] response The HTTP response object to be populated with the result of the resend request.
	 */
	void resendRecoverPassword(const httplib::Request& request, httplib::Response& response);

	/**
	 * @brief Verifies a password reset token.
	 * @details This function checks the validity of a password reset token. If valid, the associated email is returned in the response.
	 *          The request must contain the API key and the token to be verified.
	 * @param[in] request The HTTP request object containing the necessary parameters (API key and token).
	 * @param[out] response The HTTP response object to be populated with the result of the token verification.
	 */
	void verifyResetPasswordToken(const httplib::Request& request, httplib::Response& response);

	/**
	 * @brief Resets the password for a given email.
	 * @details This function updates the password for the user's account. The request must contain the API key, email, and new password.
	 *          If the password is successfully updated, a success message is returned.
	 * @param[in] request The HTTP request object containing the necessary parameters (API key, email, and new password).
	 * @param[out] response The HTTP response object to be populated with the result of the password reset operation.
	 */
	void resetPassword(const httplib::Request& request, httplib::Response& response);

	/**
	 * @brief Retrieves all vehicles associated with a specific subscription.
	 * @details This function retrieves the list of vehicles associated with a given subscription for a specific account.
	 *          The request must contain the API key, email address, and subscription name. If the API key is valid,
	 *          the vehicles are retrieved and returned in the response. If any required parameter is missing or invalid,
	 *          an error response is returned.
	 * @param[in] request The HTTP request object containing the necessary parameters (API key, email, and subscription name).
	 * @param[out] response The HTTP response object to be populated with the vehicles list for the given subscription.
	 */
	void getSubscriptionVehicles(const httplib::Request& request, httplib::Response& response);

	/**
	 * @brief Adds a new subscription to an account.
	 * @details This function allows an account to add a new subscription. The request must contain the API key, email address, and subscription name.
	 *          If the API key is valid and the subscription is successfully added, a success response is returned. Otherwise, an error response is sent.
	 * @param[in] request The HTTP request object containing the necessary parameters (API key, email, and subscription name).
	 * @param[out] response The HTTP response object to be populated with the result of the add subscription operation.
	 */
	void addSubscription(const httplib::Request& request, httplib::Response& response);

	/**
	 * @brief Deletes an existing subscription from an account.
	 * @details This function allows an account to delete a subscription. The request must contain the API key, email address, and subscription name.
	 *          If the API key is valid and the subscription is successfully deleted, a success response is returned. Otherwise, an error response is sent.
	 * @param[in] request The HTTP request object containing the necessary parameters (API key, email, and subscription name).
	 * @param[out] response The HTTP response object to be populated with the result of the delete subscription operation.
	 */
	void deleteSubscription(const httplib::Request& request, httplib::Response& response);

	/**
	 * @brief Retrieves the parking history of a vehicle.
	 * @details This function retrieves the history of parking sessions for a specific vehicle identified by its license plate.
	 *          The request must contain the API key and license plate number. The response includes a list of parking sessions,
	 *          along with total time parked and payment details. If any parameter is missing or invalid, an error response is returned.
	 * @param[in] request The HTTP request object containing the necessary parameters (API key and license plate).
	 * @param[out] response The HTTP response object to be populated with the vehicle's parking history, total time parked, and payment details.
	 */
	void getVehicleHistory(const httplib::Request& request, httplib::Response& response);

	/**
	 * @brief Adds a new vehicle to a subscription.
	 * @details This function allows an account to add a vehicle to a specified subscription. The request must contain the API key, email address,
	 *          subscription name, and vehicle's license plate. The vehicle's license plate is converted to uppercase before being added.
	 *          If the vehicle is successfully added, the updated list of vehicles for the subscription is returned. If any issue arises,
	 *          an error response is sent.
	 * @param[in] request The HTTP request object containing the necessary parameters (API key, email, subscription name, and license plate).
	 * @param[out] response The HTTP response object to be populated with the result of the add vehicle operation.
	 */
	void addVehicle(const httplib::Request& request, httplib::Response& response);

	/**
	 * @brief Deletes a vehicle from a subscription.
	 * @details This function removes a vehicle identified by its license plate from a specific subscription for a given account.
	 *          The request must contain the API key, email address, subscription name, and license plate. If the vehicle is successfully deleted,
	 *          a success response is returned. If any parameter is missing or invalid, an error response is sent.
	 * @param[in] request The HTTP request object containing the necessary parameters (API key, email, subscription name, and license plate).
	 * @param[out] response The HTTP response object to be populated with the result of the delete vehicle operation.
	 */
	void deleteVehicle(const httplib::Request& request, httplib::Response& response);

	/**
	 * @brief Updates account information (name, last name, phone).
	 * @details This function updates the account information for a given email address. The request must contain the API key and any combination of
	 *          new name, new last name, and new phone. If the phone is already associated with another account, the update fails.
	 * @param[in] request The HTTP request object containing the necessary parameters (API key, email, and new account information).
	 * @param[out] response The HTTP response object to be populated with the result of the update operation.
	 */
	void updateAccountInformation(const httplib::Request& request, httplib::Response& response);

	/**
	 * @brief Updates account credentials (email or password).
	 * @details This function updates an account's email or password. The request must contain the API key and the current email, with optional new email and new password.
	 *          If the user has admin privileges, the update can be confirmed with a token.
	 * @param[in] request The HTTP request object containing the necessary parameters (API key, email, new credentials, and admin flag).
	 * @param[out] response The HTTP response object to be populated with the result of the update account operation.
	 */
	void updateAccount(const httplib::Request& request, httplib::Response& response);

	/**
	 * @brief Sends a validation email for account update confirmation.
	 * @details This function sends a validation email to the user for confirming an account update. The email contains a link with a token for validation.
	 *          The request must contain the API key, email, and name. If the token is successfully generated and set, the email is sent.
	 * @param[in] request The HTTP request object containing the necessary parameters (API key, email, and name).
	 * @param[out] response The HTTP response object to be populated with the result of the validation request.
	 */
	void validateUpdateViaEmail(const httplib::Request& request, httplib::Response& response);

	/**
	 * @brief Sends a validation SMS for account update confirmation.
	 * @details This function sends a validation SMS to the user for confirming an account update. The SMS contains a token for validation.
	 *          The request must contain the API key, email, and phone number. If the token is successfully generated and set, the SMS is sent.
	 * @param[in] request The HTTP request object containing the necessary parameters (API key, email, and phone).
	 * @param[out] response The HTTP response object to be populated with the result of the validation request.
	 */
	void validateUpdateViaSMS(const httplib::Request& request, httplib::Response& response);

	/**
	 * @brief Resends a validation SMS for account update confirmation.
	 * @details This function resends an SMS with the validation token to the user's phone to confirm an account update.
	 *          The request must contain the API key, email, and phone. If a valid token exists for the given email, the SMS is sent.
	 * @param[in] request The HTTP request object containing the necessary parameters (API key, email, and phone).
	 * @param[out] response The HTTP response object to be populated with the result of the resend SMS operation.
	 */
	void resendValidateUpdateSMS(const httplib::Request& request, httplib::Response& response);

	/**
	 * @brief Validates an account update using a token.
	 * @details This function validates a token to confirm an account update. The request must contain the API key and token.
	 *          If the token is valid, the account update is applied, and the email is returned in the response.
	 * @param[in] request The HTTP request object containing the necessary parameters (API key and token).
	 * @param[out] response The HTTP response object to be populated with the result of the validation.
	 */
	void validateUpdate(const httplib::Request& request, httplib::Response& response);

	/**
	 * @brief Handles user contact messages.
	 * @details This function processes a contact message sent by a user. The request must contain the API key, email, subject, and message.
	 *          An automated confirmation email is sent to the user, acknowledging receipt of their message.
	 * @param[in] request The HTTP request object containing the necessary parameters (API key, email, subject, and message).
	 * @param[out] response The HTTP response object to be populated with the result of the contact message handling.
	 */
	void contact(const httplib::Request& request, httplib::Response& response);

	/**
	 * @brief Subscribes a user to the newsletter.
	 * @details This function subscribes a user to the ParkPass newsletter. The request must contain the API key and email.
	 *          A confirmation email is sent to the user, thanking them for subscribing to the newsletter.
	 * @param[in] request The HTTP request object containing the necessary parameters (API key and email).
	 * @param[out] response The HTTP response object to be populated with the result of the subscription operation.
	 */
	void subscribeNewsletter(const httplib::Request& request, httplib::Response& response);

	/**
	 * @brief Unsubscribes a user from the newsletter.
	 * @details This function removes the user from the ParkPass newsletter subscription. The request must contain the API key and email.
	 *          A confirmation email is sent to the user, confirming the unsubscription.
	 * @param[in] request The HTTP request object containing the necessary parameters (API key and email).
	 * @param[out] response The HTTP response object to be populated with the result of the unsubscription.
	 */
	void unsubscribeNewsletter(const httplib::Request& request, httplib::Response& response);

private:
	std::thread thread;
	httplib::Server server;
	std::unique_ptr<WebSocketServer> webSocketServer;
	SubscriptionManager subscriptionManager;
	Logger& logger;
	std::string key;
	std::string siteUrl;
};
