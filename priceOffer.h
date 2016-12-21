/*
 * priceOffer.h
 *
 *  Created on: Apr 10, 2016
 *
 *  For flexibility and reusability reasons, this ADT doesn't check whether
 *	the parameters are correct, unless incorrect parameters may harm the program
 *	(e.g negative number as array length, or NULL pointer).
 *	For details - check functions descriptions.
 */

#ifndef PRICEOFFER_H_
#define PRICEOFFER_H_

#include "utilities.h"
#include "email.h"

typedef struct priceOffer_t* PriceOffer;

/*
 * IMPORTANT: since price offer object doesn't know anything about service,
 * realtors or customers,all parameters must be validated (e.g realtor_email
 * should really belong to realtor, and service with service_name should exist)
 * before calling this function.
 *     @param customer_email - email adress of customer, who sent the offer
 *     @param realtor_email - email adress of realtor, who the offer was sent to
 *     @param service_name - the name of an apartment service, where an
 *     						 announcement was published
 *     @param apartment_id - id number of an apartment, customer wants to buy
 *     @param new_price - price, that customer offers to pay for the apartment
 * @Return
 * NULL in case of null pointer as argument, or memory allocation error
 * new Price Offer otherwise
 */
PriceOffer priceOfferCreate(const Email customer_email,
		const Email realtor_email, const char* service_name,
		 int apartment_id, int new_price);

/*
 * Allocates and creates new identical copy of given price offer
 * @Return
 * NULL in case of null pointer as argument, or memory allocation error
 * otherwise, newly created copy of price_offer
 */
PriceOffer priceOfferCopy(PriceOffer price_offer);

/*
 * Frees all allocated memory and destroys object
 * Can receive NULL
 */
void priceOfferDestroy(PriceOffer price_offer);

/*
 * 2 Offers assumed equal if both were sent from same customer
 * to the same realtor.
 * Also, if both offers are NULL - they are considered equal
 * If only one of offers is NULL - they are considered unequal
 * Since 2 pairs of emails are compared, there's no meaning for one
 * offer to be considered "greater" than another one.
 * @return:
 * 0 if offers are considered equal
 * 1 otherwise
 */
int priceOfferCompare(PriceOffer price_offer_1, PriceOffer price_offer_2);

/*
 * this function asserts price_offer isn't NULL
 * @Return
 * Pointer to customer email
 */
const Email priceOfferGetCustomerEmail(PriceOffer price_offer);

/*
 * this function asserts price_offer isn't NULL
 * @Return
 * Pointer to realtor email
 */
const Email priceOfferGetRealtorEmail(PriceOffer price_offer);

/*
 * this function asserts price_offer isn't NULL
 * @Return
 * Pointer to string of name of service,
 * that contains requested apartment
 */
const char* priceOfferGetServiceName(PriceOffer price_offer);

/*
 * this function asserts price_offer isn't NULL
 * @Return
 * Requested apartment id
 */
int priceOfferGetApartmentId(PriceOffer price_offer);

/*
 * this function asserts price_offer isn't NULL
 * @Return
 * Offered price
 */
int priceOfferGetPrice(PriceOffer price_offer);

#endif /* PRICEOFFER_H_ */
