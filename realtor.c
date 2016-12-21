/*
 * realtor.c
 *
 *  Created on: Apr 10, 2016
 *
 */

#include "realtor.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "realtor.h"
#include "utilities.h"
#include "map.h"
#include "apartment.h"
#include "apartment_service.h"

typedef struct realtor_t {
	char* company_name;
	Map services;
	int tax_percentage;
	int influence_rating;
} realtor_t;

//====Casting functions to/from generic types===================================
/* Apartment service as Map element: */

/* Allocates memory and copies name string
 * @Return:
 * NULL in case of malloc errors
 * new copy of string otherwise */
static MapKeyElement copyServiceName(constMapKeyElement key_element) {
	char* tmp = malloc(strlen((char*) key_element) + 1);
	NULL_PTR_CHECK(tmp,NULL);
	return (MapKeyElement) (strcpy(tmp, (char*) key_element));
}
/* Frees all allocated memory. Can receive NULL */
static void freeServiceName(MapKeyElement key_element) {
	free((char*)key_element);
}
/* Compares 2 names lexicographically. Actually, just wrapper for strcmp.
 * IMPORTANT: As strcmp, this function cannot receive NULL.  */
static int serviceCompare(constMapKeyElement key_element_1,
	constMapKeyElement key_element_2) {
	return strcmp((const char*) key_element_1, (const char*) key_element_2);
}
static Map createMapOfServices() {
	return mapCreate((copyMapDataElements) serviceCopy,
		(copyMapKeyElements) copyServiceName,
		(freeMapDataElements) serviceDestroy,
		(freeMapKeyElements) freeServiceName,
		(compareMapKeyElements) serviceCompare);
}
//====Error types conversion====================================================
static RealtorResult errorConvertMapToRealtor(MapResult result) {
	switch (result) {
	case MAP_ITEM_ALREADY_EXISTS: //Map doesn't ever throw this error
		return REALTOR_APARTMENT_SERVICE_ALREADY_EXISTS;
	case MAP_ITEM_DOES_NOT_EXIST:
		return REALTOR_APARTMENT_SERVICE_DOES_NOT_EXIST;
	case MAP_NULL_ARGUMENT:
		return REALTOR_NULL_ARG;
	case MAP_OUT_OF_MEMORY:
		return REALTOR_OUT_OF_MEM;
	default:
		return REALTOR_SUCCESS;
	}
}

static RealtorResult errorConvertServiceToRealtor(ApartmentServiceResult result)
{
	switch (result) {
	case APARTMENT_SERVICE_NO_FIT:
		return REALTOR_APARTMENT_NOT_FOUND;
	case APARTMENT_SERVICE_OUT_OF_BOUNDS:
		return REALTOR_INVALID_PARAMETERS;
	case APARTMENT_SERVICE_EMPTY:
		return REALTOR_APARTMENT_NOT_FOUND;
	case APARTMENT_SERVICE_ALREADY_EXISTS:
		return REALTOR_APARTMENT_ALREADY_EXISTS;
	case APARTMENT_SERVICE_FULL:
		return REALTOR_APARTMENT_SERVICE_FULL;
	case APARTMENT_SERVICE_NULL_ARG:
		return REALTOR_NULL_ARG;
	case APARTMENT_SERVICE_OUT_OF_MEM:
		return REALTOR_OUT_OF_MEM;
	default:
		return REALTOR_SUCCESS;
	}
}
//====Inner use functions=======================================================
static bool matrixIsValid(const char* matrix, int width, int height) {
	assert(matrix);
	if (strlen(matrix) != width * height) {
		return false;
	}
	int i = 0;
	while (matrix[i]) {
		if (matrix[i] != 'w' && matrix[i] != 'e') {
			return false;
		}
		i++;
	}
	return true;
}

static int allServicesApartmentsCount(Realtor realtor) {
	assert(realtor);
	int result = 0;
	MAP_FOREACH(char*, service_name,realtor->services)
	{
		ApartmentService service = mapGet(realtor->services, service_name);
		result += serviceNumberOfApatments(service);
	}
	return result;
}

static int findMedianAverage(Realtor realtor,
	ApartmentServiceResult (*getMedian)(ApartmentService, int*)) {
	assert(realtor && getMedian);
	if (mapGetSize(realtor->services) <= 0) {
		return 0; //so we do not divide by 0
	}
	int sum = 0, service_median = 0, non_empty_services = 0;
	MAP_FOREACH(char*, service_name,realtor->services)
	{
		ApartmentService service = mapGet(realtor->services, service_name);
		assert(service);
		getMedian(service, &service_median);
		non_empty_services += (serviceNumberOfApatments(service) > 0);
		sum += (serviceNumberOfApatments(service) > 0) ? service_median : 0;
	}
	return non_empty_services ? (sum / non_empty_services) : 0;
}

static inline int allServicesAreaMediansAverage(Realtor realtor) {
	assert(realtor);
	return findMedianAverage(realtor, serviceAreaMedian);
}

static inline int allSevicesPriceMediansAverage(Realtor realtor) {
	assert(realtor);
	return findMedianAverage(realtor, servicePriceMedian);
}

static void updateInfluenceRating(Realtor realtor) {
	assert(realtor);
	realtor->influence_rating = APARTMENTS_COUNT_COEFF
		* allServicesApartmentsCount(realtor)
		+ allSevicesPriceMediansAverage(realtor)
		+ AREA_COEFF * allServicesAreaMediansAverage(realtor);
}

static void destroySquares(SquareType** squares, int length) {
	if (!squares) {
		return;
	}
	for (int i = 0; i < length; i++) {
		free(squares[i]);
	}
	free(squares);
}

/*
 * String of matrix must be of width*length size and must contain e and w only!
 */
static SquareType** convertStringToSquares(const char* matrix, int width,
	int length) {
	//must to be (and is) checked in calling function:
	assert(matrix && width > 0 && length > 0);
	SquareType** squares = malloc(sizeof(*squares) * length);
	NULL_PTR_CHECK(squares, NULL);
	for (int i = 0; i < length; i++) {
		//This allows to simply use destroySquares
		//in case of malloc error in following 'for' loop
		squares[i] = NULL;
	}
	for (int i = 0; i < length; i++) {
		squares[i] = malloc(sizeof(*squares[i]) * width);
		if (!squares[i]) {
			destroySquares(squares, length);
			return NULL;
		}
	}
	for (int i = 0; i < length; i++) {
		for (int j = 0; j < width; j++) {
			squares[i][j] = matrix[i * width + j] == 'e' ? EMPTY : WALL;
		}
	}
	return squares;
}

static inline int getPriceWithoutTax(int price, int tax_percentage) {
	return 100 * price / (100 + tax_percentage);
}

//====Public functions==========================================================
Realtor realtorCreate(const char* company_name, int tax_percentage) {
	NULL_PTR_CHECK(company_name, NULL);
	Realtor new_realtor = malloc(sizeof(*new_realtor));
	NULL_PTR_CHECK(new_realtor, NULL);
	new_realtor->company_name = malloc(strlen(company_name) + 1);
	new_realtor->services = createMapOfServices();
	if (!new_realtor->company_name || !new_realtor->services) {
		realtorDestroy(new_realtor);
		return NULL;
	}
	strcpy(new_realtor->company_name, company_name);
	new_realtor->tax_percentage = tax_percentage;
	new_realtor->influence_rating = 0;
	return new_realtor;
}

Realtor realtorCopy(Realtor realtor) {
	NULL_PTR_CHECK(realtor, NULL);
	Realtor realtor_copy = realtorCreate(realtor->company_name,
		realtor->tax_percentage);
	NULL_PTR_CHECK(realtor_copy, NULL);
	mapDestroy(realtor_copy->services);
	realtor_copy->services = mapCopy(realtor->services);
	if (!realtor_copy->services) {
		realtorDestroy(realtor_copy);
		return NULL;
	}
	realtor_copy->influence_rating = realtor->influence_rating;
	return realtor_copy;
}

void realtorDestroy(Realtor realtor) {
	if (!realtor) {
		return;
	}
	mapDestroy(realtor->services);
	free(realtor->company_name);
	free(realtor);
}

RealtorResult realtorAddApartment(Realtor realtor, const char* service_name,
	int id, int width, int height, const char* matrix, int price) {
	if (!realtor || !service_name || !matrix) {
		return REALTOR_NULL_ARG;
	}
	if (width <= 0 || height <= 0 || !matrixIsValid(matrix, width, height)) {
		return REALTOR_INVALID_PARAMETERS;
	}
	SquareType** squares = convertStringToSquares(matrix, width, height);
	NULL_PTR_CHECK(squares, REALTOR_OUT_OF_MEM);
	Apartment apartment = apartmentCreate(squares, height, width, price);
	destroySquares(squares, height);
	NULL_PTR_CHECK(apartment, REALTOR_OUT_OF_MEM);
	ApartmentService service = mapGet(realtor->services, service_name);
	ApartmentServiceResult result = serviceAddApartment(service, apartment, id);
	apartmentDestroy(apartment);
	if (result == APARTMENT_SERVICE_NULL_ARG) {
		return REALTOR_APARTMENT_SERVICE_DOES_NOT_EXIST;
	}
	updateInfluenceRating(realtor);
	return errorConvertServiceToRealtor(result);
}

RealtorResult realtorRemoveApartment(Realtor realtor, const char* service_name,
	int id) {
	NULL_PTR_CHECK(realtor, REALTOR_NULL_ARG);
	NULL_PTR_CHECK(service_name, REALTOR_NULL_ARG);
	ApartmentService service = mapGet(realtor->services, service_name);
	NULL_PTR_CHECK(service, REALTOR_APARTMENT_SERVICE_DOES_NOT_EXIST);
	ApartmentServiceResult result = serviceDeleteById(service, id);
	updateInfluenceRating(realtor);
	return errorConvertServiceToRealtor(result);
}

RealtorResult realtorAddApartmentService(Realtor realtor,
	const char* service_name, int max_apartments) {
	NULL_PTR_CHECK(realtor, REALTOR_NULL_ARG);
	NULL_PTR_CHECK(service_name, REALTOR_NULL_ARG);
	if (max_apartments <= 0) {
		return REALTOR_INVALID_PARAMETERS;
	}
	if (mapContains(realtor->services, service_name)) {
		return REALTOR_APARTMENT_SERVICE_ALREADY_EXISTS;
	}
	ApartmentService service = serviceCreate(max_apartments);
	NULL_PTR_CHECK(service, REALTOR_OUT_OF_MEM);
	MapResult result = mapPut(realtor->services, service_name, service);
	serviceDestroy(service);
	return errorConvertMapToRealtor(result);
}

RealtorResult realtorRemoveApartmentService(Realtor realtor,
	const char* service_name) {
	NULL_PTR_CHECK(realtor, REALTOR_NULL_ARG);
	NULL_PTR_CHECK(service_name, REALTOR_NULL_ARG);
	MapResult result = mapRemove(realtor->services, service_name);
	updateInfluenceRating(realtor);
	return errorConvertMapToRealtor(result);
}

int realtorGetInfluenceRating(Realtor realtor) {
	assert(realtor);
	return realtor->influence_rating;
}

int realtorCompareByRating(Realtor realtor_1, Realtor realtor_2) {
	assert(realtor_1);
	assert(realtor_2);
	return realtor_1->influence_rating - realtor_2->influence_rating;
}

int realtorGetTaxPercentage(Realtor realtor) {
	assert(realtor);
	return realtor->tax_percentage;
}

RealtorResult realtorSearchAllServices(Realtor realtor, int area, int rooms,
	int price, Apartment* apartment) {
	NULL_PTR_CHECK(realtor, REALTOR_NULL_ARG);
	NULL_PTR_CHECK(apartment, REALTOR_NULL_ARG);
	price = getPriceWithoutTax(price, realtor->tax_percentage);
	ApartmentService service;
	ApartmentServiceResult result = APARTMENT_SERVICE_NO_FIT;
	MAP_FOREACH(MapKeyElement, current, realtor->services)
	{
		service = mapGet(realtor->services, current);
		assert(service);
		result = serviceSearch(service, area, rooms, price, apartment);
		if (result == APARTMENT_SERVICE_SUCCESS) {
			return REALTOR_SUCCESS;
		}
	}
	return errorConvertServiceToRealtor(result);
}

const char* realtorGetCompanyName(Realtor realtor) {
	assert(realtor);
	return realtor->company_name;
}

RealtorResult realtorChangeApartmentPrice(Realtor realtor,
	const char* service_name, int id, int discount_percent) {
	NULL_PTR_CHECK(realtor, REALTOR_NULL_ARG);
	NULL_PTR_CHECK(service_name, REALTOR_NULL_ARG);
	if (discount_percent < MIN_DISCOUNT_PERCENT || id < 0) {
		return REALTOR_INVALID_PARAMETERS;
	}
	ApartmentService service = mapGet(realtor->services, service_name);
	NULL_PTR_CHECK(service, REALTOR_APARTMENT_SERVICE_DOES_NOT_EXIST);
	Apartment apartment = NULL;
	/* Since serviceGetById returns a copy, and there's no way to get a pointer
	 * to the apartment - we need to extract it, change and put it back */
	serviceGetById(service, id, &apartment);
	NULL_PTR_CHECK(apartment, REALTOR_APARTMENT_NOT_FOUND);
	serviceDeleteById(service, id);
	apartmentChangePrice(apartment, discount_percent);
	serviceAddApartment(service, apartment, id);
	apartmentDestroy(apartment);
	return REALTOR_SUCCESS;
}

RealtorResult realtorGetApartmentById(Realtor realtor, const char* service_name,
	int id, Apartment* apartment) {
	if(!realtor || !service_name || !apartment){
		return REALTOR_NULL_ARG;
	}
	ApartmentService service = mapGet(realtor->services, service_name);
	NULL_PTR_CHECK(service, REALTOR_APARTMENT_SERVICE_DOES_NOT_EXIST);
	ApartmentServiceResult result = serviceGetById(service, id, apartment);
	return errorConvertServiceToRealtor(result);
}
