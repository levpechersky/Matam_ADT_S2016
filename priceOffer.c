/*
 * priceOffer.c
 *
 *  Created on: Apr 10, 2016
 *
 */

#include "priceOffer.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "utilities.h"
#include "email.h"

typedef struct priceOffer_t {
	Email customer_email;
	Email realtor_email;
	char* service_name;
	int apartment_id;
	int offered_price;
} priceOffer_t;

//Used by priceOfferCompare function
#define EQUAL 0
#define NOT_EQUAL 1

//====Inner use functions=======================================================

//=====Public functions=========================================================

PriceOffer priceOfferCreate(const Email customer_email,
		const Email realtor_email, const char* service_name,
		 int apartment_id, int new_price) {
	if (!customer_email || !realtor_email || !service_name) {
		return NULL;
	}
	PriceOffer new_offer = malloc(sizeof(*new_offer));
	NULL_PTR_CHECK(new_offer, NULL);
	new_offer->customer_email = malloc(strlen(customer_email) + 1);
	new_offer->realtor_email = malloc(strlen(realtor_email) + 1);
	new_offer->service_name = malloc(strlen(service_name) + 1);
	if (!new_offer->customer_email || !new_offer->realtor_email
			|| !new_offer->service_name) {
		priceOfferDestroy(new_offer);
		return NULL;
	}
	strcpy(new_offer->customer_email, customer_email);
	strcpy(new_offer->realtor_email, realtor_email);
	strcpy(new_offer->service_name, service_name);
	new_offer->apartment_id = apartment_id;
	new_offer->offered_price = new_price;
	return new_offer;
}

PriceOffer priceOfferCopy(PriceOffer price_offer) {
	NULL_PTR_CHECK(price_offer, NULL);
	PriceOffer copy = priceOfferCreate(price_offer->customer_email,
			price_offer->realtor_email, price_offer->service_name,
			price_offer->apartment_id, price_offer->offered_price);
	return copy;
}

void priceOfferDestroy(PriceOffer price_offer) {
	if (!price_offer) {
		return;
	}
	free(price_offer->customer_email);
	free(price_offer->realtor_email);
	free(price_offer->service_name);
	free(price_offer);
}

int priceOfferCompare(PriceOffer price_offer_1, PriceOffer price_offer_2) {
	if (!price_offer_1 && !price_offer_2) {
		return EQUAL;
	}
	if (!price_offer_1 || !price_offer_2) {
		return NOT_EQUAL;
	}
	// 0 - equal, 1 - unequal
	return emailCompare(price_offer_1->customer_email,
			price_offer_2->customer_email)
			|| emailCompare(price_offer_1->realtor_email,
					price_offer_2->realtor_email);
}

const Email priceOfferGetCustomerEmail(PriceOffer price_offer) {
	assert(price_offer);
	return price_offer->customer_email;
}

const Email priceOfferGetRealtorEmail(PriceOffer price_offer) {
	assert(price_offer);
	return price_offer->realtor_email;
}

const char* priceOfferGetServiceName(PriceOffer price_offer) {
	assert(price_offer);
	return price_offer->service_name;
}

int priceOfferGetApartmentId(PriceOffer price_offer) {
	assert(price_offer);
	return price_offer->apartment_id;
}

int priceOfferGetPrice(PriceOffer price_offer) {
	assert(price_offer);
	return price_offer->offered_price;
}
