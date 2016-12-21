/*
 * priceOffer_test.c
 *
 *  Created on: Apr 20, 2016
 *
 */

#include "../priceOffer.h"

#include <string.h>

#include "test_utilities.h"

bool priceOffer_create_destroy_test() {
	ASSERT_TEST(NULL==priceOfferCreate(NULL, "realtor@email",
			"Mario Bros. Service", 12345, 100));
	ASSERT_TEST(NULL==priceOfferCreate("customer@mail", NULL,
			"Mario Bros. Service", 12345, 100));
	ASSERT_TEST(NULL==priceOfferCreate("customer@mail", "realtor@email",
			NULL, 12345, 100));
	PriceOffer price_offer = priceOfferCreate("customer@mail", "realtor@email",
			"Mario Bros. Service", 12345, 100);
	ASSERT_TEST(NULL!=price_offer);
	priceOfferDestroy(NULL);
	priceOfferDestroy(price_offer);
	return true;
}

bool priceOffer_copy_compare_test() {
	PriceOffer price_offer = priceOfferCreate("customer@mail", "realtor@email",
			"Mario Bros. Service", 12345, 100);
	ASSERT_TEST(NULL!=price_offer);
	ASSERT_TEST(NULL==priceOfferCopy(NULL));
	PriceOffer copy = priceOfferCopy(price_offer);
	ASSERT_TEST(0==priceOfferCompare(NULL, NULL));
	ASSERT_TEST(1==priceOfferCompare(NULL, copy));
	ASSERT_TEST(1==priceOfferCompare(price_offer, NULL));
	ASSERT_TEST(0 == priceOfferCompare(price_offer, copy));
	PriceOffer duplicate_offer = priceOfferCreate("customer@mail",
			"realtor@email", "Kimchi's Service", 2, 200);
	ASSERT_TEST(0 == priceOfferCompare(price_offer, duplicate_offer));
	ASSERT_TEST(0 == priceOfferCompare(copy, duplicate_offer));
	priceOfferDestroy(price_offer);
	priceOfferDestroy(duplicate_offer);
	priceOfferDestroy(copy);
	return true;
}

bool priceOffer_getters_test() {
	PriceOffer offer_1 = priceOfferCreate("c_email@1", "r_email@2",
			"Mario Bros. Service", 252, 2500000);
	PriceOffer offer_2 = priceOfferCreate("customer@mail", "realtor@mail",
			"KGB", 45347, 100000000);
	ASSERT_TEST(0 == strcmp(priceOfferGetCustomerEmail(offer_1), "c_email@1"));
	ASSERT_TEST(0 == strcmp("r_email@2", priceOfferGetRealtorEmail(offer_1)));
	ASSERT_TEST(
			0
					== strcmp("Mario Bros. Service",
							priceOfferGetServiceName(offer_1)));
	ASSERT_TEST(252 == priceOfferGetApartmentId(offer_1));
	ASSERT_TEST(
			0 == strcmp("customer@mail", priceOfferGetCustomerEmail(offer_2)));
	ASSERT_TEST(
			0 == strcmp("realtor@mail", priceOfferGetRealtorEmail(offer_2)));
	ASSERT_TEST(0 == strcmp("KGB", priceOfferGetServiceName(offer_2)));
	ASSERT_TEST(45347 == priceOfferGetApartmentId(offer_2));
	//swapped c_ and r_
	ASSERT_TEST(0 != strcmp(priceOfferGetCustomerEmail(offer_1), "r_email@1"));
	ASSERT_TEST(0 != strcmp("c_email@2", priceOfferGetRealtorEmail(offer_1)));
	ASSERT_TEST(
			0 != strcmp("No Such Service", priceOfferGetServiceName(offer_1)));
	ASSERT_TEST(2500000 != priceOfferGetApartmentId(offer_1));
	priceOfferDestroy(offer_1);
	priceOfferDestroy(offer_2);
	return true;
}

int main() {
	RUN_TEST(priceOffer_create_destroy_test);
	RUN_TEST(priceOffer_copy_compare_test);
	RUN_TEST(priceOffer_getters_test);
	return 0;
}

