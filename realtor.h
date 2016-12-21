/*
 * realtor.h
 *
 *  Created on: Apr 10, 2016
 *
 *  For flexibility and reusability reasons, this ADT doesn't check whether
 *	the parameters are correct, unless incorrect parameters may harm the program
 *	(e.g negative number as array length, or NULL pointer).
 *	Although, this ADT uses ApartmentService and Apartment types, which do check
 *	parameters, related to apartments and services.
 *	For details - check functions descriptions.
 */

#ifndef REALTOR_H_
#define REALTOR_H_

#include "apartment.h"
#include "apartment_service.h"

typedef enum {
	REALTOR_SUCCESS,
	REALTOR_NULL_ARG,
	REALTOR_OUT_OF_MEM,
	REALTOR_INVALID_PARAMETERS,
	REALTOR_APARTMENT_SERVICE_ALREADY_EXISTS,
	REALTOR_APARTMENT_SERVICE_DOES_NOT_EXIST,
	REALTOR_APARTMENT_SERVICE_FULL,
	REALTOR_APARTMENT_ALREADY_EXISTS,
	REALTOR_APARTMENT_NOT_FOUND
} RealtorResult;

typedef struct realtor_t* Realtor;

#define APARTMENTS_COUNT_COEFF 1000000
#define AREA_COEFF 100000

#define MIN_DISCOUNT_PERCENT -100

/*
 * Allocates memory and creates new realtor.
 *     @param company_name - name of a company, realtor works for
 *     @param tax_percentage - percent of tax realtor gets from each purchase.
 *     						   IMPORTANT: tax_percentage value is not checked.
 * @Return
 * NULL if company_name points to NULL, or in case of memory allocation errors.
 * new allocated realtor object otherwise
 */
Realtor realtorCreate(const char* company_name, int tax_percentage);

/*
 * Allocates memory and creates new copy of the realtor. All services (if any)
 * and apartments are copied as well.
 * @Return
 * NULL  if realtor is NULL or in case of memory allocation errors.
 * new allocated realtor object otherwise
 */
Realtor realtorCopy(Realtor realtor);

/*
 * Frees all allocated memory and destroys object and every apartment and
 * service, he added.
 * Can receive NULL.
 */
void realtorDestroy(Realtor realtor);

/*
 * For given realtor creates and adds an apartment to one of his services, known
 * by service_name.
 *     @param realtor - realtor to add apartment to.
 *     @param service_name - name of service to add apartment to.
 *     @param id - id number of new apartment, at specific service
 *     			   each id must be unique.
 *     @param width - width of new apartment, must be positive.
 *     @param height - height of new apartment, must be positive.
 *     @param matrix - a string of 'e' and 'w' and length of width*height,
 *     				   describing apartment layout. 'e' stand for empty, and
 *     				   'w' stands for wall.
 *     @param price - price of new apartment, must be positive.
 *@Return:
 * REALTOR_NULL_ARG if realtor or service_name or matrix is NULL.
 * REALTOR_OUT_OF_MEM in case of memory allocation fault.
 * REALTOR_INVALID_PARAMETERS if width or length aren't positive or
 * 			id is negative (id 0 is ok), or matrix is not as described above.
 * REALTOR_APARTMENT_SERVICE_DOES_NOT_EXIST if there's no service
 * 			with a given name
 * REALTOR_APARTMENT_SERVICE_FULL if apartment service have reached maximum
 * 			apartments number
 * REALTOR_APARTMENT_ALREADY_EXISTS if there's an apartment with given id at
 * 			given service
 * REALTOR_SUCCESS otherwise
 */
RealtorResult realtorAddApartment(Realtor realtor, const char* service_name,
	int id, int width, int length, const char* matrix, int price);

/*
 * For realtor removes an apartment from his service, known by service_name.
 * Apartment is removed by id.
 *     @param realtor - realtor to remove an apartment from one of his services
 *     @param service_name - name of service to remove apartment from
 *     @param id - id number of apartment to remove
 *@Return:
 * REALTOR_NULL_ARG if realtor or service_name or matrix is NULL.
 * REALTOR_INVALID_PARAMETERS if id is negative (id 0 is ok)
 * REALTOR_APARTMENT_SERVICE_DOES_NOT_EXIST if there's no service with a given
 * 		name
 * REALTOR_APARTMENT_NOT_FOUND if there's no apartment with given id,
 * 			or service is empty
 * REALTOR_SUCCESS otherwise
 */
RealtorResult realtorRemoveApartment(Realtor realtor, const char* service_name,
	int id);

/*
 * For given realtor creates and adds an apartment to one of his services, known
 * by service_name.
 *     @param realtor - realtor to add service to.
 *     @param service_name - name of a new service. Must be unique.
 *     @param max_apartments - maximal capacity (of apartments) of a new service
 *@Return:
 * REALTOR_NULL_ARG if realtor or service_name is NULL.
 * REALTOR_OUT_OF_MEM in case of memory allocation fault.
 * REALTOR_INVALID_PARAMETERS if max_apartments isn't positive
 * REALTOR_APARTMENT_SERVICE_ALREADY_EXISTS if there already is a service
 * 			with the same name
 * REALTOR_SUCCESS otherwise
 */
RealtorResult realtorAddApartmentService(Realtor realtor,
	const char* service_name, int maxApartments);

/*
 * For realtor removes an apartment service by it's name.
 *     @param realtor - realtor to remove a service from.
 *     @param service_name - name of a service to remove.
 *@Return:
 * REALTOR_NULL_ARG if realtor or service_name or matrix is NULL.
 * REALTOR_APARTMENT_SERVICE_DOES_NOT_EXIST if there's no service with a
 * 			given name
 * REALTOR_SUCCESS otherwise
 */
RealtorResult realtorRemoveApartmentService(Realtor realtor,
	const char* service_name);

/*
 * This function asserts that realtor isn't NULL
 * Rating is calculated by formula:
 * APARTMENTS_COUNT_COEFF * (total number of apartments)
 * + (an average of price medians of apartments at each service)
 * + AREA_COEFF * (an average of area medians of apartments at each service)
 * @Return:
 * Realtor's influence rating
 */
int realtorGetInfluenceRating(Realtor realtor);

/*
 * This function asserts that realtor isn't NULL
 * @Return
 * positive if realtor_1 rating is greater than realtor_2
 * 0 if both realtors have the same rating
 * negative otherwise
 */
int realtorCompareByRating(Realtor realtor_1, Realtor realtor_2);

/*
 * This function asserts that realtor isn't NULL
 * @Return:
 * Realtor's tax percentage
 */
int realtorGetTaxPercentage(Realtor realtor);

/*
 * Searches all realtor's apartment services for an apartment that fits given
 * requierements. If there few such apartments, first found returned.
 *     @param realtor - realtor to look for apartments
 *     @param area - minimal area (must be greater or equal)
 *     @param rooms - minimal number of rooms (must be greater or equal)
 *     @param price -  maximal apartment price (including tax)
 *     @param apartment - output variable pointer, found apartment is copied to.
 *@Return:
 * REALTOR_NULL_ARG if realtor or apartment is NULL.
 * REALTOR_INVALID_PARAMETERS if area, rooms or price is negative
 * REALTOR_APARTMENT_NOT_FOUND if no apartment matches given conditions
 * 		(pay attention, that price value for search is even lower, than given
 * 		price parameter, because we search for apartments, where
 * 		(raw price + tax)<=(given price))
 * REALTOR_SUCCESS otherwise
 */
RealtorResult realtorSearchAllServices(Realtor realtor, int area, int rooms,
	int price, Apartment* apartment);

/*
 * This function aserts that realtor isn't NULL
 * @Return
 * Pointer to realtor's company name. Doesn't copies the string.
 */
const char* realtorGetCompanyName(Realtor realtor);

/*
 * Applies discount to specific apartment with given id at service,
 * named service_name.
 * 		@param realtor - realtor, that owns the apartment
 *      @param service_name - name of a service, containing the apartment.
 *      @param id - apartment id. Non negative
 *      @param discount_percent - percent of discount to apply, may be positive
 *      	or negative, but greater than MIN_DISCOUNT_PERCENT
 * @Return:
 * REALTOR_NULL_ARG if realtor or service_name is NULL.
 * REALTOR_APARTMENT_SERVICE_DOES_NOT_EXIST if there's no service with a given
 * 		name
 * REALTOR_INVALID_PARAMETERS if id is negative (id 0 is ok) or discount percent
 * 		lower than MIN_DISCOUNT_PERCENT
 * REALTOR_APARTMENT_NOT_FOUND if there's no apartment with given id,
 * 			or service is empty
 * REALTOR_SUCCESS otherwise
 */
RealtorResult realtorChangeApartmentPrice(Realtor realtor,
	const char* service_name, int id, int discount_percent);

/*
 * Searches for a service with service_name, and apartment with given id at the
 * service. If found, apartment is stored at out_apartment.
 * 		@param realtor - realtor, that owns the apartment
 *      @param service_name - name of a service, containing the apartment.
 *      @param id - apartment id. Non negative
 *      @param out_apartment - output variable for found apartment
 * @Return:
 * REALTOR_NULL_ARG if realtor or service_name or apartment is NULL.
 * REALTOR_APARTMENT_SERVICE_DOES_NOT_EXIST if there's no service with a given
 * 		name
 * REALTOR_INVALID_PARAMETERS if id is negative (id 0 is ok)
 * REALTOR_APARTMENT_NOT_FOUND if there's no apartment with given id,
 * 			or service is empty
 * REALTOR_SUCCESS otherwise
 */
RealtorResult realtorGetApartmentById(Realtor realtor, const char* service_name,
	int id, Apartment* out_apartment);

#endif /* REALTOR_H_ */
