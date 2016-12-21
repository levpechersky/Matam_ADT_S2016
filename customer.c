/*
 * customer.c
 *
 *  Created on: Apr 10, 2016
 *
 */

#include "customer.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include "customer.h"
#include "utilities.h"
#include "apartment.h"

typedef struct customer_t {
	int min_area;
	int min_rooms;
	int max_price;
	int money_paid;
} customer_t;

//====Inner use functions=======================================================

//====Public functions==========================================================

Customer customerCreate(int min_area, int min_rooms, int max_price) {
	Customer new_customer = malloc(sizeof(*new_customer));
	NULL_PTR_CHECK(new_customer, NULL);
	new_customer->min_area = min_area;
	new_customer->min_rooms = min_rooms;
	new_customer->max_price = max_price;
	new_customer->money_paid = 0;
	return new_customer;
}

Customer customerCopy(Customer customer) {
	NULL_PTR_CHECK(customer, NULL);
	Customer copy = customerCreate(customer->min_area, customer->min_rooms,
			customer->max_price);
	NULL_PTR_CHECK(copy, NULL);
	assert(customerGetMoneyPaid(customer) >= 0);
	customerPay(copy, customer->money_paid);
	return copy;
}

void customerDestroy(Customer customer) {
	if (!customer) {
		return;
	}
	free(customer);
}

int customerCompareByMoneyPaid(Customer customer_1, Customer customer_2) {
	assert(customer_1);
	assert(customer_2);
	return customer_1->money_paid - customer_2->money_paid;
}

CustomerResult customerPay(Customer customer, int amount) {
	NULL_PTR_CHECK(customer, CUSTOMER_NULL_ARG);
	if (customer->money_paid + amount < 0) {
		return CUSTOMER_WRONG_PAYMENT;
	}
	customer->money_paid += amount;
	return CUSTOMER_SUCCESS;
}

int customerGetMinRooms(Customer customer) {
	assert(customer);
	return customer->min_rooms;
}

int customerGetMinArea(Customer customer) {
	assert(customer);
	return customer->min_area;
}

int customerGetMaxPrice(Customer customer) {
	assert(customer);
	return customer->max_price;
}

int customerGetMoneyPaid(Customer customer) {
	assert(customer);
	return customer->money_paid;
}

bool customerApartmentFitsRequierements(Customer customer, Apartment apartment){
	assert(customer);
	assert(apartment);
	return apartmentGetPrice(apartment) <= customer->max_price
			&& apartmentTotalArea(apartment) >= customer->min_area
			&& apartmentNumOfRooms(apartment) >= customer->min_rooms;
}

