/*
 * customer.h
 *
 *  Created on: Apr 10, 2016
 *
 *	For flexibility and reusability reasons, this ADT doesn't check whether
 *	the parameters are correct, unless incorrect parameters may harm the program
 *	(e.g negative number as array length, or NULL pointer)
 *	For details - check functions descriptions.
 */

#ifndef CUSTOMER_H_
#define CUSTOMER_H_

#include "apartment.h"

typedef enum {
	CUSTOMER_SUCCESS,
	CUSTOMER_NULL_ARG,
	CUSTOMER_OUT_OF_MEM,
	CUSTOMER_WRONG_PAYMENT
} CustomerResult;

typedef struct customer_t* Customer;

/*
 * Creates new customer.
 * IMPORTANT: Doesn't checks whether parameters' values are correct logically.
 * (e.g  min_area, min_rooms, max_price are positive, etc.)
 * Conditions should be defined and checked by user
 *     @param min_area - Minimal apartment area, customer wants
 *     @param min_rooms - Minimal number of rooms, customer wants
 *     @param max_price - Maximal price customer is ready to pay
 * @Return
 * NULL in case of memory allocation error.
 * New allocated customer otherwise
 */
Customer customerCreate(int min_area, int min_rooms, int max_price);

/*
 * Allocates and creates new identical (including money paid amount)
 * copy of a customer.
 * @Return
 * NULL in case of null pointer as argument, or memory allocation error
 * otherwise, newly created copy of customer.
 */
Customer customerCopy(Customer customer);

/*
 * Frees all allocated memory and destroys object.
 * Can receive NULL.
 */
void customerDestroy(Customer customer);

/*
 * This function asserts customers aren't NULL
 * @Return
 * positive if customer1 spent more than customer2,
 * 0 if both customers spent same money,
 * negative otherwise
 */
int customerCompareByMoneyPaid(Customer customer_1, Customer customer_2);

/*
 * Changes amount of money, customer paid.
 * Can be positive or negative.
 *     @param amount - amount of money to pay
 * @Return
 * CUSTOMER_NULL_ARG if customer points to NULL
 * CUSTOMER_WRONG_PAYMENT if money paid value will become negative after payment
 * CUSTOMER_SUCCESS otherwise
 */
CustomerResult customerPay(Customer customer, int amount);

/*
 * This function asserts customer isn't NULL
 * @Return
 * Minimal number of rooms, customer wants
 */
int customerGetMinRooms(Customer customer);

/*
 * This function asserts customer isn't NULL
 * @Return
 * Minimal apartment area, customer wants
 */
int customerGetMinArea(Customer customer);

/*
 * This function asserts customer isn't NULL
 * @Return
 * Maximal price customer is ready to pay
 */
int customerGetMaxPrice(Customer customer);

/*
 * This function asserts customer isn't NULL
 * @Return
 * amount of money customer paid since his account have been created
 */
int customerGetMoneyPaid(Customer customer);

/*
 * Checks if the apartment fits customer's requirements:
 *    apartment's price is lower or equal than maximum price he can pay
 *    total area is greater or equals to minimal area he wants
 *    number of rooms greater or equal to minimal number of rooms he wants.
 * This function asserts customer and apartment aren't NULL
 * @Return
 * true if apartment fits all requirements
 * false otherwise
 */
bool customerApartmentFitsRequierements(Customer customer, Apartment apartment);

#endif /* CUSTOMER_H_ */
