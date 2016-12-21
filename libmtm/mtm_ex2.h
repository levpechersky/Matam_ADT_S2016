#ifndef MTM_EX2_H_
#define MTM_EX2_H_

#include <stdio.h> // For FILE*
// You should avoid including unnecessary header files
#include <stdbool.h>

/**
* Maximal input line length
*/
#define MAX_LEN 120

/**
* This type defines all errors for the system.
* These codes should be used for calling mtmPrintErrorMessage to report
* error messages.
*
* Notice that after calling mtmPrintErrorMessage with one of the first
* three messages you should abort the program according to the exercises
* rules
*/
typedef enum {
	MTM_OUT_OF_MEMORY, // You should exit program after this error
	MTM_INVALID_COMMAND_LINE_PARAMETERS, // You should exit the program after this error
	MTM_CANNOT_OPEN_FILE, // You should exit program after this error
	MTM_INVALID_PARAMETERS,
	MTM_EMAIL_ALREADY_EXISTS,
	MTM_EMAIL_DOES_NOT_EXIST,
	MTM_EMAIL_WRONG_ACCOUNT_TYPE,
	MTM_ALREADY_REQUESTED,
	MTM_NOT_REQUESTED,
	MTM_APARTMENT_SERVICE_ALREADY_EXISTS,
	MTM_APARTMENT_SERVICE_DOES_NOT_EXIST,
	MTM_APARTMENT_SERVICE_FULL,
	MTM_APARTMENT_ALREADY_EXISTS,
	MTM_APARTMENT_DOES_NOT_EXIST,
	MTM_PURCHASE_WRONG_PROPERTIES,
	MTM_REQUEST_WRONG_PROPERTIES,
	MTM_REQUEST_ILLOGICAL_PRICE,
} MtmErrorCode;

/**
* mtmPrintErrorMessage - prints an error message into the given output channel
* Use the standard error channel to print error messages.
*
* @param errorChannel - File descriptor for the error channel
* @param code - Error code to print message for.
*/
void mtmPrintErrorMessage(FILE* errorChannel, MtmErrorCode code);

/**
* mtmPrintRealtor - prints Realtor details.
*
* @param outputChannel - File descriptor for the output channel
* @param email - The email of the Realtor.
* @param email - The company name of the Realtor.
*/
void mtmPrintRealtor(FILE* outputChannel, const char* email,
	const char* companyName);

/**
* mtmPrintCustomer - prints customer details.
*
* @param outputChannel - File descriptor for the output channel
* @param email - The email of the customer.
* @param money_spent - The amount of money that the user spent.
*/
void mtmPrintCustomer(FILE* outputChannel, const char* email,
	int money_spent);

#endif /* MTM_EX2_H_ */
