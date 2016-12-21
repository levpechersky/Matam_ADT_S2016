#ifndef APARTMENT_SERVICE_H
#define APARTMENT_SERVICE_H

#include "apartment.h"

typedef struct apartment_service_t* ApartmentService;

typedef enum {
	APARTMENT_SERVICE_SUCCESS,
	APARTMENT_SERVICE_OUT_OF_MEM,
	APARTMENT_SERVICE_NULL_ARG,
	APARTMENT_SERVICE_FULL,
	APARTMENT_SERVICE_NO_FIT,
	APARTMENT_SERVICE_OUT_OF_BOUNDS,
	APARTMENT_SERVICE_EMPTY,
	APARTMENT_SERVICE_ALREADY_EXISTS
} ApartmentServiceResult;

ApartmentService serviceCreate(int maxNumOfApartments);

ApartmentService serviceCopy(ApartmentService service);

void serviceDestroy(ApartmentService service);

int serviceNumberOfApatments(ApartmentService service);

ApartmentServiceResult serviceAddApartment(ApartmentService service, 									   
										   Apartment apartment, int id);

ApartmentServiceResult servicePriceMedian(ApartmentService service,
										  int* outResult);

ApartmentServiceResult serviceAreaMedian(ApartmentService service, 
										 int* outResult);

ApartmentServiceResult serviceDeleteApartment(ApartmentService service,
											  Apartment apartment);

ApartmentServiceResult serviceDeleteById(ApartmentService service, int id);

ApartmentServiceResult serviceGetById(ApartmentService service, int id,
									  Apartment* outApartment);

ApartmentServiceResult serviceSearch(ApartmentService service, int area,
									 int rooms, int price, 
									 Apartment* outApartment);

#endif /* APARTMENT_SERVICE_H */
