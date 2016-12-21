/*
 * customer_test.c
 *
 *  Created on: Apr 20, 2016
 *
 */
#include "../yad3Service.h"

#include <string.h>
#include <stdbool.h>

#include "../customer.h"
#include "../apartment.h"
#include "test_utilities.h"

//=============================================================================
/*
 * generates square-shaped all empty apartments
 * prices and sizes arrays size should be equal or greater than apartments_num
 */
static Apartment* generateApartments(int apartments_num, int* prices,
		int* sizes) {
	int max_dimensions = 0;
	for (int i = 0; i < apartments_num; i++) {
		max_dimensions = max_dimensions > sizes[i] ? max_dimensions : sizes[i];
	}
	SquareType** squares = malloc(sizeof(*squares) * max_dimensions);
	for (int i = 0; i < max_dimensions; i++) {
		squares[i] = malloc(sizeof(*squares[i]) * max_dimensions);
		for (int j = 0; j < max_dimensions; j++) {
			squares[i][j] = EMPTY;
		}
	}
	Apartment* apartments = malloc(sizeof(*apartments) * apartments_num);
	for (int i = 0; i < apartments_num; i++) {
		apartments[i] = apartmentCreate(squares, sizes[i], sizes[i], prices[i]);
	}
	for (int i = 0; i < max_dimensions; i++) {
		free(squares[i]);
	}
	free(squares);
	return apartments;
}

static void destroyApartments(Apartment* apartments, int size) {
	for (int i = 0; i < size; i++) {
		apartmentDestroy(apartments[i]);
	}
	free(apartments);
}
//=============================================================================

bool customer_create_destroy_test() {
	Customer new_customer = customerCreate(1, 1, 2000);
	ASSERT_TEST(NULL!=new_customer);
	customerDestroy(NULL);
	customerDestroy(new_customer);
	return true;
}

bool customer_copy_compare_test() {
	Customer new_customer = customerCreate(30, 3, 2000);
	Customer new_customer_2 = customerCreate(30, 3, 2000);
	ASSERT_TEST(NULL!=new_customer);
	ASSERT_TEST(NULL == customerCopy(NULL));
	Customer copy = customerCopy(new_customer);
	ASSERT_TEST(NULL != copy);
	ASSERT_TEST(0 == customerCompareByMoneyPaid(new_customer, new_customer_2));
	customerPay(new_customer, 1000);
	ASSERT_TEST(0 < customerCompareByMoneyPaid(new_customer, new_customer_2));
	ASSERT_TEST(0 < customerCompareByMoneyPaid(new_customer, copy));
	customerPay(copy, 999);
	ASSERT_TEST(0 < customerCompareByMoneyPaid(new_customer, copy));
	customerPay(copy, 1);
	ASSERT_TEST(0 == customerCompareByMoneyPaid(new_customer, copy));
	customerPay(copy, 1);
	ASSERT_TEST(0 > customerCompareByMoneyPaid(new_customer, copy));
	customerDestroy(copy);
	customerDestroy(new_customer);
	customerDestroy(new_customer_2);
	return true;
}

bool customer_pay_getmoneyPaid_test() {
	Customer new_customer = customerCreate(3, 3, 200);
	ASSERT_TEST(0 == customerGetMoneyPaid(new_customer));
	ASSERT_TEST(CUSTOMER_NULL_ARG == customerPay(NULL, 12345));
	ASSERT_TEST(CUSTOMER_SUCCESS == customerPay(new_customer, 12345));
	ASSERT_TEST(CUSTOMER_SUCCESS == customerPay(new_customer, -345));
	ASSERT_TEST(12000 == customerGetMoneyPaid(new_customer));
	ASSERT_TEST(CUSTOMER_WRONG_PAYMENT == customerPay(new_customer, -12001));
	ASSERT_TEST(12000 == customerGetMoneyPaid(new_customer));
	customerDestroy(new_customer);
	return true;
}

bool customer_get_min_rooms_area_max_price() {
	Customer new_customer_1 = customerCreate(1, 3, 299);
	Customer new_customer_2 = customerCreate(196, 1, 1300);
	ASSERT_TEST(1 == customerGetMinArea(new_customer_1));
	ASSERT_TEST(3 == customerGetMinRooms(new_customer_1));
	ASSERT_TEST(299 == customerGetMaxPrice(new_customer_1));
	ASSERT_TEST(196 == customerGetMinArea(new_customer_2));
	ASSERT_TEST(1 == customerGetMinRooms(new_customer_2));
	ASSERT_TEST(1300 == customerGetMaxPrice(new_customer_2));
	customerDestroy(new_customer_1);
	customerDestroy(new_customer_2);
	return true;
}

bool customer_apartmentFitsRequierements_test() {
	int apartment_num = 4;
	int prices[] = { 300, 2500, 1300, 600 };
	int sizes[] = { 1, 4, 20, 14 };
	Apartment* apartments = generateApartments(apartment_num, prices, sizes);
	Customer new_customer1 = customerCreate(1, 1, 299);
	Customer new_customer2 = customerCreate(14 * 14, 1, 1300);
	Customer new_customer3 = customerCreate(1, 2, 10000);
	ASSERT_TEST(false == customerApartmentFitsRequierements(new_customer1,
			apartments[0]));
	ASSERT_TEST(false == customerApartmentFitsRequierements(new_customer1,
			apartments[1]));
	ASSERT_TEST(false == customerApartmentFitsRequierements(new_customer2,
			apartments[0]));
	ASSERT_TEST(true == customerApartmentFitsRequierements(new_customer2,
			apartments[2]));
	ASSERT_TEST(true == customerApartmentFitsRequierements(new_customer2,
			apartments[3]));
	ASSERT_TEST(false == customerApartmentFitsRequierements(new_customer3,
			apartments[2]));
	ASSERT_TEST(false == customerApartmentFitsRequierements(new_customer3,
			apartments[3]));
	customerDestroy(new_customer1);
	customerDestroy(new_customer2);
	customerDestroy(new_customer3);
	destroyApartments(apartments, apartment_num);
	return true;
}

int main() {
	RUN_TEST(customer_create_destroy_test);
	RUN_TEST(customer_copy_compare_test);
	RUN_TEST(customer_pay_getmoneyPaid_test);
	RUN_TEST(customer_get_min_rooms_area_max_price);
	RUN_TEST(customer_apartmentFitsRequierements_test);
	return 0;
}
