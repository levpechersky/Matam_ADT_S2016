/*
 * yad3Service.h
 *
 *  Created on: Apr 9, 2016
 *
 */

#ifndef YAD3SERVICE_H_
#define YAD3SERVICE_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "email.h"
#include "utilities.h"

typedef enum {
	YAD3_SUCCESS,
	YAD3_NULL_ARG,
	YAD3_OUT_OF_MEMORY,
	YAD3_INVALID_PARAMETERS,
	YAD3_EMAIL_ALREADY_EXISTS,
	YAD3_EMAIL_DOES_NOT_EXIST,
	YAD3_EMAIL_WRONG_ACCOUNT_TYPE,
	YAD3_ALREADY_REQUESTED,
	YAD3_NOT_REQUESTED,
	YAD3_APARTMENT_SERVICE_ALREADY_EXISTS,
	YAD3_APARTMENT_SERVICE_DOES_NOT_EXIST,
	YAD3_APARTMENT_SERVICE_FULL,
	YAD3_APARTMENT_ALREADY_EXISTS,
	YAD3_APARTMENT_DOES_NOT_EXIST,
	YAD3_PURCHASE_WRONG_PROPERTIES,
	YAD3_REQUEST_WRONG_PROPERTIES,
	YAD3_REQUEST_ILLOGICAL_PRICE
} Yad3Result;

typedef enum {
	DECLINE, ACCEPT
} Response;//to price offer

//Realtor's tax percentage limits:
#define MIN_TAX_PERCENTAGE 1
#define MAX_TAX_PERCENTAGE 100
//Price increment step, apartments' price must be multiply of this value:
#define PRICE_INCREMENT_STEP 100
//Lower limits for report functions:
#define REPORT_MIN_CUSTOMER_PAYMENT 0
#define REPORT_MIN_REALTOR_RATING 0

typedef struct yad3Service_t* Yad3Service;

/*
 * Allocates memory and creates a new Yad3 service.
 * @Return:
 * NULL in case of memory allocation fault
 * New empty service otherwise
 */
Yad3Service yad3ServiceCreate();

/*
 * Frees all allocated memory and destroys object.
 * Can receive NULL.
 */
void yad3ServiceDestroy(Yad3Service yad_3);

/*
 * Adds a realtor to the system.
 *     @param email - new realtor's email. Must be unique and contain exactly
 *     		one "@"
 *     @param company_name - name of a company, realtor works for
 *     @param tax_percentage - percent of tax realtor gets from each purchase.
 *     		must be between (or equal) MIN_TAX_PERCENTAGE and MAX_TAX_PERCENTAGE
 *@Return:
 * YAD3_NULL_ARG if yad_3 or email or company_name is NULL
 * YAD3_OUT_OF_MEMORY in case of memory allocation fault,
 * YAD3_INVALID_PARAMETERS if email or tax percentage is invalid
 * YAD3_EMAIL_ALREADY_EXISTS if user with give email exists
 * YAD3_SUCCESS otherwise
 */
Yad3Result yad3RealtorAdd(Yad3Service yad_3, const Email email,
	const char* company_name, int tax_percentage);

/*
 * Removes a realtor from the system. Every his data, e.g apartments
 * in his possesion will be removed as well. All price offers to that realtor
 * are removed.
 * 		@param email - email of account to delete. Must contain exactly one "@"
 *@Return:
 * YAD3_NULL_ARG if yad_3 or email is NULL
 * YAD3_OUT_OF_MEMORY in case of memory allocation fault,
 * YAD3_INVALID_PARAMETERS if email is invalid
 * YAD3_EMAIL_WRONG_ACCOUNT_TYPE if user isn't a realtor
 * YAD3_EMAIL_DOES_NOT_EXIST if user with given email doesn't exist
 * YAD3_SUCCESS otherwise
 */
Yad3Result yad3RealtorRemove(Yad3Service yad_3, const Email email);

/*
 * For realtor adds an apartment to one of his services,
 * by it's name. Apartment should have unique ID, positive values of
 * price, width and height, and valid matrix of width*height of w and e
 * characters, where w stands for wall, and e - for empty.
 * 		@param email - realtor's email to add apartment to. Must contain
 * 			exactly one "@"
 * 		@param service_name - new service name must be unique at realtor
 *		@param id - new apartment id. Must be unique at service. Non negative.
 * 		@param width - width of new apartment, must be positive
 * 		@param height - height of new apartment, must be positive
 *		@param matrix - a string of 'e' and 'w' and length of width*height,
 *     				   describing apartment layout. 'e' stand for empty, and
 *     				   'w' stands for wall.
 *      @param price - price of new apartment, must be positive
 *@Return:
 * YAD3_NULL_ARG if yad_3 or email or service_name or matrix is NULL
 * YAD3_OUT_OF_MEMORY in case of memory allocation fault,
 * YAD3_INVALID_PARAMETERS if email or matrix is invalid or price, width, height
 * 		aren't positive, or id is negative, or price isn't
 * 		PRICE_INCREMENT_STEP multiply
 * YAD3_EMAIL_DOES_NOT_EXIST if user with given email doesn't exist
 * YAD3_EMAIL_WRONG_ACCOUNT_TYPE if user isn't a realtor
 * YAD3_APARTMENT_SERVICE_DOES_NOT_EXIST there no service with a given name
 * YAD3_APARTMENT_SERVICE_FULL if apartment service have reached maximum
 * 		apartments number
 * YAD3_APARTMENT_ALREADY_EXISTS if there's an apartment with given id at given
 * 		service
 * YAD3_SUCCESS otherwise
 */
Yad3Result yad3RealtorAddApartment(Yad3Service yad_3, const Email email,
	const char* service_name, int id, int width, int height, int price,
	const char* matrix);

/*
 * For realtor removes an apartment from his service,
 * by it's name. Apartment removed by ID. Every offer for that apartment is
 * removed.
 * 		@param email - realtor's email to remove apartment from. Must contain
 * 			exactly one "@"
 *      @param service_name - name of service to remove apartment from
 *      @param id - id number of apartment to remove. Non negative
 *@Return:
 * YAD3_NULL_ARG if yad_3 or service_name or email is NULL
 * YAD3_INVALID_PARAMETERS if email or id is invalid
 * YAD3_EMAIL_DOES_NOT_EXIST if user with given email doesn't exist
 * YAD3_EMAIL_WRONG_ACCOUNT_TYPE if user isn't a realtor
 * YAD3_APARTMENT_SERVICE_DOES_NOT_EXIST there no service with a given name
 * YAD3_APARTMENT_DOES_NOT_EXIST if there's no apartment with given id at given
 * 		 service
 * YAD3_SUCCESS otherwise
 */
Yad3Result yad3RealtorRemoveApartment(Yad3Service yad_3, const Email email,
	const char* service_name, const int id);

/*
 * For realtor adds an apartment service
 * Each realtor allowed to host any number of services.
 * Each service must have a unique name (at specific realtor's).
 * 		@param email - realtor's email to add service to. Must contain
 * 			exactly one "@"
 *      @param service_name - name of new service. Must be unique.
 *      @param max_apartments - maximum apartment capacity of a new service.
 *      	Must be positive.
 *@Return:
 * YAD3_NULL_ARG if yad_3 or service_name or email is NULL
 * YAD3_OUT_OF_MEMORY in case of memory allocation fault,
 * YAD3_INVALID_PARAMETERS if email is invalid or max_apartments <= 0
 * YAD3_EMAIL_DOES_NOT_EXIST if user with given email doesn't exist
 * YAD3_EMAIL_WRONG_ACCOUNT_TYPE if user isn't a realtor
 * YAD3_APARTMENT_SERVICE_ALREADY_EXISTS if there exists a service with a
 * 		given name
 * YAD3_SUCCESS otherwise
 */
Yad3Result yad3RealtorAddApartmentService(Yad3Service yad_3, const Email email,
	const char* service_name, int max_apartments);

/*
 * For realtor removes an apartment service by it's name.
 * Every offer for every apartment at the service is removed.
 * 		@param email - realtor's email to remove service from. Must contain
 * 			exactly one "@"
 *      @param service_name - name of service to remove.
 *@Return:
 * YAD3_NULL_ARG if yad_3 or service_name or email is NULL
 * YAD3_OUT_OF_MEMORY in case of memory allocation fault,
 * YAD3_INVALID_PARAMETERS if email is invalid
 * YAD3_EMAIL_DOES_NOT_EXIST if user with given email doesn't exist
 * YAD3_EMAIL_WRONG_ACCOUNT_TYPE if user isn't a realtor
 * YAD3_APARTMENT_SERVICE_DOES_NOT_EXIST there no service with a given name
 * YAD3_SUCCESS otherwise
 */
Yad3Result yad3RealtorRemoveApartmentService(Yad3Service yad_3,
	const Email email, const char* service_name);

/*
 * Adds a customer to the system.
 *     @param email - new customer's email. Must be unique and contain exactly
 *     		one "@"
 *     @param min_area - minimal apartment area customer wants. Positive
 *     @param min_rooms - minimal number of rooms customer wants. Positive
 *     @param max_price - maximal price, that customer is ready to pay. Positive
 *@Return:
 * YAD3_NULL_ARG if yad_3 or email is NULL
 * YAD3_OUT_OF_MEMORY in case of memory allocation fault,
 * YAD3_INVALID_PARAMETERS if email is invalid or min_area, min_rooms, max_price
 * 		are negative or zero (at least one)
 * YAD3_EMAIL_ALREADY_EXISTS if user with given email exists
 * YAD3_SUCCESS otherwise
 */
Yad3Result yad3CustomerAdd(Yad3Service yad_3, const Email email, int min_area,
	int min_rooms, int max_price);

/*
 * Removes a customer from the system. Every offer ever sent by him is removed.
 * 		@param email - email of account to delete. Must contain exactly one "@"
 *@Return:
 * YAD3_NULL_ARG if yad_3 or email is NULL
 * YAD3_INVALID_PARAMETERS if email is invalid
 * YAD3_EMAIL_DOES_NOT_EXIST if user with given email doesn't exist
 * YAD3_EMAIL_WRONG_ACCOUNT_TYPE if user isn't a customer
 * YAD3_SUCCESS otherwise
 */
Yad3Result yad3CustomerRemove(Yad3Service yad_3, const Email email);

/*
 * Purchases an apartment by service name and id for the customer
 * from the realtor by their emails.
 * Apartment's price for the customer contains actual price and realtors tax.
 * Apartment can be purchased only if it fits customer's conditions
 * All offers about the apartment are deleted.
 * 		@param customer_email - email of customer purchasing the apartment.
 * 			Must contain exactly one "@"
 * 		@param realtor_email - email of realtor selling the apartment.
 * 			Must contain exactly one "@"
 * 		@param service_name - name of realtor's apartment service, containing
 * 			the apartment.
 * 		@param apartment_id - the apartment to purchase.
 *@Return:
 * YAD3_NULL_ARG if yad_3 or customer_email or realtor_email or
 * 		service_name is NULL
 * YAD3_OUT_OF_MEMORY in case of memory allocation fault,
 * YAD3_INVALID_PARAMETERS if one of emails is invalid or id is negative
 * YAD3_EMAIL_DOES_NOT_EXIST if user with given email doesn't exist
 * 		(at least one of them)
 * YAD3_EMAIL_WRONG_ACCOUNT_TYPE if at least one of user isn't of expected type
 * 		(e.g email adresses swapped)
 * YAD3_APARTMENT_SERVICE_DOES_NOT_EXIST if there's no service with a given name
 * YAD3_APARTMENT_DOES_NOT_EXIST if there's no apartment with given id
 * 		at given service
 * YAD3_PURCHASE_WRONG_PROPERTIES if the apartment doesn't fit customer's
 * 		requirements
 * YAD3_SUCCESS otherwise
 */
Yad3Result yad3CustomerPurchase(Yad3Service yad_3, const Email customer_email,
	const Email realtor_email, const char* service_name,const int apartment_id);

/*
 * Customer can send a price offer to a realtor for a given apartment.
 * A price offered already contains apartments price and tax.
 * Additional offer can't be sent, if the realtor had not answered the previous
 * offer (if any).
 * Offer can be sent only for appartment that fits customer's conditions.
 * 		@param customer_email - email of customer purchasing the apartment.
 * 			Must contain exactly one "@"
 * 		@param realtor_email - email of realtor selling the apartment.
 * 			Must contain exactly one "@"
 * 		@param service_name - name of realtor's apartment service, containing
 * 			the apartment.
 * 		@param apartment_id - the apartment to purchase.
 * 		@param new_price - price customer offers to pay (including taxes).
 * 			Should be lower (or equal) than customer's maximal price, and be
 * 			lower, than regular purchase
 *@Return:
 * YAD3_NULL_ARG if yad_3 or customer_email or realtor_email or
 * 		service_name is NULL
 * YAD3_OUT_OF_MEMORY in case of memory allocation fault,
 * YAD3_INVALID_PARAMETERS if one of emails is invalid or id is negative
 * 		or price isn't positive
 * YAD3_EMAIL_DOES_NOT_EXIST if user with given email doesn't exist
 * 		(at least one of them)
 * YAD3_EMAIL_WRONG_ACCOUNT_TYPE if at least one of user isn't of expected type
 * 		(e.g email adresses swapped)
 * YAD3_ALREADY_REQUESTED if there's already unanswered offer from the customer
 * 		to the realtor
 * YAD3_APARTMENT_SERVICE_DOES_NOT_EXIST if there's no service with a given name
 * YAD3_APARTMENT_DOES_NOT_EXIST if there's no apartment with given id
 * 		at given service
 * YAD3_REQUEST_WRONG_PROPERTIES if the apartment doesn't fit customer's
 * 		conditions, or offered price is higher than customer's defined maximum
 * YAD3_REQUEST_ILLOGICAL_PRICE if the price offered is higher than regular
 * 		purchase
 * YAD3_SUCCESS otherwise
 */
Yad3Result yad3CustomerMakeOffer(Yad3Service yad_3, const Email email,
	const Email realtor_email, const char* service_name, int apartment_id,
	int new_price);

/*
 * Realtor can respond to customer's price offer, if realtor accepts, purchase
 * is made. Price offer removed from the system, and customer is allowed to send
 * another one.
 * 		@param customer_email - email of customer purchasing the apartment.
 * 			Must contain exactly one "@"
 * 		@param realtor_email - email of realtor selling the apartment.
 * 			Must contain exactly one "@"
 * 		@param choise - ACCEPT or DECLINE
 *@Return:
 * YAD3_NULL_ARG if yad_3 or customer_email or realtor_email is NULL
 * YAD3_INVALID_PARAMETERS if one of emails is invalid
 * YAD3_EMAIL_DOES_NOT_EXIST if user (at least one) with given email doesn't
 * 		exist
 * YAD3_EMAIL_WRONG_ACCOUNT_TYPE if at least one of user isn't of expected type
 * 		(e.g email adresses swapped)
 * YAD3_NOT_REQUESTED there are no unanswered offers from that customer
 * YAD3_SUCCESS otherwise
 */
Yad3Result yad3RealtorRespondToOffer(Yad3Service yad_3,
	const Email customer_email, const Email realtor_email, Response choise);

/*
 * Prints every realtor that has 1 or more apartments, that fit customer's
 * conditions, and can be bought with customerPurchase (if there's no such
 * realtors - nothing will be printed).
 * Report is sorted by emails in A-Z order and printed by mtmPrintRealtor
 * This function asserts output!=NULL
 * 		@param customer_email - email of customer we search realtors for
 * 		@param output - output file/channel to print reports
 *@Return:
 * YAD3_NULL_ARG if yad_3 or customer_email is NULL
 * YAD3_OUT_OF_MEMORY in case of memory allocation fault,
 * YAD3_INVALID_PARAMETERS if email is invalid
 * YAD3_EMAIL_DOES_NOT_EXIST if user with given email doesn't exist
 * YAD3_EMAIL_WRONG_ACCOUNT_TYPE if user isn't a customer
 * YAD3_SUCCESS otherwise
 */
Yad3Result yad3ReportRelevantRealtors(Yad3Service yad_3,
	const Email customer_email, FILE* output);

/*
 * Prints list of customers by amount of money they paid in descending order.
 * If 2 or more customers paid same money, results are sorted by emails
 * in A-Z order. If none of customers have ever paid more than
 * REPORT_MIN_CUSTOMER_PAYMENT (0 by default) - nothing will be printed.
 * Reports are printed by mtmPrintCustomer
 * This function asserts output!=NULL
 * 		@param count - number of customers to print. Should be positive
 *@Return:
 * YAD3_NULL_ARG if yad_3  is NULL
 * YAD3_OUT_OF_MEMORY in case of memory allocation fault,
 * YAD3_INVALID_PARAMETERS if count isn't positive
 * YAD3_SUCCESS otherwise
 */
Yad3Result yad3ReportMostPayingCustomers(Yad3Service yad_3, int count,
	FILE* output);

/*
 * Prints list of realtors by their influence rating.
 * If 2 or more realtors have same  influence rating, results are sorted by
 * emails in A-Z order. If a realtor has rating lower than
 * REPORT_MIN_REALTOR_RATING (0 by default) - his name will not appear.
 * Reports are printed by mtmPrintRealtor
 * This function asserts output!=NULL
 * 		@param count - number of realtors to print. Should be positive
 *@Return:
 * YAD3_NULL_ARG if yad_3  is NULL
 * YAD3_OUT_OF_MEMORY in case of memory allocation fault,
 * YAD3_INVALID_PARAMETERS if count isn't positive
 * YAD3_SUCCESS otherwise
 */
Yad3Result yad3ReportSignificantRealtors(Yad3Service yad_3, int count,
	FILE* output);

#endif /* YAD3SERVICE_H_ */
