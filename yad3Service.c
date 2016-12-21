/*
 * yad3Service.c
 *
 *  Created on: Apr 10, 2016
 *
 */

#include "yad3Service.h"

#include <assert.h>
#include <string.h>
#include "mtm_ex2.h"
#include "customer.h"
#include "realtor.h"
#include "priceOffer.h"
#include "utilities.h"
#include "email.h"
#include "set.h"
#include "map.h"
#include "list.h"
#include "apartment.h"
#include "apartment_service.h"

typedef struct yad3Service_t {
	Map realtors;
	Map customers;
	Set offers;
} yad3Service_t;

typedef enum {
	REALTOR, CUSTOMER
} UserType;

//=====Casting functions to/from generic types==================================
static Map createMapOfRealtors() {
	return mapCreate((copyMapDataElements) realtorCopy,
		(copyMapKeyElements) emailCopy, (freeMapDataElements) realtorDestroy,
		(freeMapKeyElements) emailDestroy, (compareMapKeyElements)emailCompare);
}
static Map createMapOfCustomers() {
	return mapCreate((copyMapDataElements) customerCopy,
		(copyMapKeyElements) emailCopy, (freeMapDataElements) customerDestroy,
		(freeMapKeyElements) emailDestroy, (compareMapKeyElements)emailCompare);
}
static Set createSetOfOffers() {
	return setCreate((copySetElements) priceOfferCopy,
		(freeSetElements) priceOfferDestroy,
		(compareSetElements) priceOfferCompare);
}

//=====Helper types, and functions==============================================
/* Container type for email and user, who can be realtor or customer.
 * Use for map to list conversion */
typedef void* (*UserCopy)(void*);
typedef void (*UserDestroy)(void*);
typedef struct userAndEmail_t* UserAndEmail;
typedef struct userAndEmail_t {
	void* user;
	Email email;
	UserCopy userCopy;
	UserDestroy userDestroy;
} userAndEmail_t;

/* Frees all memory. container can be NULL.
 * If container isn't NULL, userDestroy field must be initialized.*/
static void userAndEmailDestroy(ListElement container) {
	if (!container) {
		return;
	}
	assert(((UserAndEmail ) container)->userDestroy);
	((UserAndEmail) container)->userDestroy(((UserAndEmail) container)->user);
	emailDestroy(((UserAndEmail) container)->email);
	free(container);
}

/* Allocates memory and creates a new container
 * @Return:
 * NULL in case of memory allocation error
 * new UserAndEmail container otherwise */
static UserAndEmail userAndEmailCreate(Email email, void* user,
	UserCopy userCopy, UserDestroy userDestroy) {
	assert(email && user && userCopy && userDestroy);
	UserAndEmail new_container = malloc(sizeof(*new_container));
	NULL_PTR_CHECK(new_container, NULL);
	new_container->userCopy = userCopy;
	new_container->userDestroy = userDestroy;
	new_container->user = (void*) userCopy(user);
	new_container->email = emailCopy(email);
	if (!new_container->user || !new_container->email) {
		userAndEmailDestroy(new_container);
		return NULL;
	}
	return new_container;
}

/* Allocates memory and creates a new copy of a container
 * @Return:
 * NULL in case of memory allocation error
 * new copy of UserAndEmail container otherwise */
static ListElement userAndEmailCopy(ListElement container) {
	NULL_PTR_CHECK(container, NULL);
	UserAndEmail new_container = userAndEmailCreate(
		((UserAndEmail) container)->email, ((UserAndEmail) container)->user,
		((UserAndEmail) container)->userCopy,
		((UserAndEmail) container)->userDestroy);
	return (ListElement) new_container;
}

/* Gets Map and userCopy/Destroy functions, e.g realtorCopy, realtorDestroy
 * Creates a new list of UserAndEmail type from map elements.
 * @Return:
 * NULL in case of memory allocation error
 * new list otherwise. */
static List convertUserMapToList(Map map, UserCopy userCopy,
	UserDestroy userDestroy) {
	List list = listCreate(userAndEmailCopy, userAndEmailDestroy);
	NULL_PTR_CHECK(list, NULL);
	MAP_FOREACH(MapKeyElement, iterator, map)
	{
		ListElement element = (ListElement) userAndEmailCreate(iterator,
			mapGet(map, iterator), userCopy, userDestroy);
		if (!element) {
			listDestroy(list);
			return NULL;
		}
		listInsertLast(list, element);
		userAndEmailDestroy(element);
	}
	return list;
}

//====Error types conversion====================================================
static Yad3Result errorConvertMapToYad3(MapResult result) {
	switch (result) {
	case MAP_ITEM_ALREADY_EXISTS:
		return YAD3_EMAIL_ALREADY_EXISTS;
	case MAP_ITEM_DOES_NOT_EXIST:
		return YAD3_EMAIL_DOES_NOT_EXIST;
	case MAP_NULL_ARGUMENT:
		return YAD3_NULL_ARG;
	case MAP_OUT_OF_MEMORY:
		return YAD3_OUT_OF_MEMORY;
	default:
		return YAD3_SUCCESS;
	}
}

static Yad3Result errorConvertRealtorToYad3(RealtorResult result) {
	switch (result) {
	case REALTOR_NULL_ARG:
		return YAD3_NULL_ARG;
	case REALTOR_OUT_OF_MEM:
		return YAD3_OUT_OF_MEMORY;
	case REALTOR_INVALID_PARAMETERS:
		return YAD3_INVALID_PARAMETERS;
	case REALTOR_APARTMENT_SERVICE_ALREADY_EXISTS:
		return YAD3_APARTMENT_SERVICE_ALREADY_EXISTS;
	case REALTOR_APARTMENT_SERVICE_DOES_NOT_EXIST:
		return YAD3_APARTMENT_SERVICE_DOES_NOT_EXIST;
	case REALTOR_APARTMENT_SERVICE_FULL:
		return YAD3_APARTMENT_SERVICE_FULL;
	case REALTOR_APARTMENT_ALREADY_EXISTS:
		return YAD3_APARTMENT_ALREADY_EXISTS;
	case REALTOR_APARTMENT_NOT_FOUND:
		return YAD3_APARTMENT_DOES_NOT_EXIST;
	default:
		return YAD3_SUCCESS;
	}
}

static Yad3Result errorConverSetToYad3(SetResult result) {
	switch (result) {
	case SET_ITEM_ALREADY_EXISTS:
		return YAD3_ALREADY_REQUESTED;
	case SET_ITEM_DOES_NOT_EXIST:
		return YAD3_NOT_REQUESTED;
	case SET_NULL_ARGUMENT:
		return YAD3_NULL_ARG;
	case SET_OUT_OF_MEMORY:
		return YAD3_OUT_OF_MEMORY;
	default:
		return YAD3_SUCCESS;
	}
}

//====Other inner use functions=================================================
/* Allocates memory, copies a string and returns pointer to a copy.
 * @Return:
 * NULL - if source is NULL or in case of malloc error
 * pointer to a new copy otherwise */
const char* strCopy(const char* source) {
	NULL_PTR_CHECK(source, NULL);
	char* destination = malloc(strlen(source) + 1);
	NULL_PTR_CHECK(destination, NULL);
	return strcpy(destination, source);
}

/* Returns true if user with given email exists at the system */
static bool checkUserExists(Yad3Service yad_3, Email email) {
	assert(yad_3); //map handles email == NULL
	return mapContains(yad_3->realtors, email)
		|| mapContains(yad_3->customers, email);
}

/* Performs email adress check and returns suitable error (or success).
 * Email is checked for not being NULL, being valid (external function),
 * and belonging to expected user type. */
static Yad3Result checkExistingEmail(Yad3Service yad_3, Email email,
	UserType userType) {
	NULL_PTR_CHECK(email, YAD3_NULL_ARG);
	if (!emailIsValid(email)) {
		return YAD3_INVALID_PARAMETERS;
	}
	if (!checkUserExists(yad_3, email)) {
		return YAD3_EMAIL_DOES_NOT_EXIST;
	}
	if ((mapContains(yad_3->customers, email) && userType == REALTOR)
		|| (mapContains(yad_3->realtors, email) && userType == CUSTOMER)) {
		return YAD3_EMAIL_WRONG_ACCOUNT_TYPE;
	}
	return YAD3_SUCCESS;
}

/* Performs email adress check for 2 emails, where one is supposed to belong to
 * realtor, and another one - to customer. Returns the most critical error.
 * Order of importance (and of check) is:
 * YAD3_NULL_ARG
 * YAD3_INVALID_PARAMETERS
 * YAD3_EMAIL_DOES_NOT_EXIST
 * YAD3_EMAIL_WRONG_ACCOUNT_TYPE */
static Yad3Result checkEmails(Yad3Service yad_3, Email realtor_email,
	Email customer_email) {
	Yad3Result result = checkExistingEmail(yad_3, realtor_email, REALTOR);
	IF_NOT_EQUAL_RETURN(result, YAD3_SUCCESS, result);
	return checkExistingEmail(yad_3, customer_email, CUSTOMER);
}

/* Returns true if tax is between (or equal) MIN and MAX tax percentage
 * (defined at header) */
static inline bool taxIsValid(int tax_percentage) {
	return tax_percentage >= MIN_TAX_PERCENTAGE
		&& tax_percentage <= MAX_TAX_PERCENTAGE;
}

/* deletes all price offers for user
 * user can be customer or realtor.
 * Use when deleting users */
static void deleteAllOffersByEmail(Set offer_set, Email email) {
	assert(offer_set && email);
	bool offer_deleted = true;
	while (offer_deleted) {
		offer_deleted = false;
		SET_FOREACH(PriceOffer, current, offer_set)
		{
			if (!emailCompare(email, priceOfferGetCustomerEmail(current))
				|| !emailCompare(email, priceOfferGetRealtorEmail(current))) {
				setRemove(offer_set, current);
			}
		}
	}
}

/* deletes all price offers for an apartment (by id)
 * Use when deleting apartments
 * @Return:
 * false in case of memory allocation error, true otherwise */
static bool deleteAllOffersById(Set offer_set, Email realtor_email,
	const char* service_name, int id) {
	assert(offer_set && realtor_email);
	const char* tmp_name = strCopy(service_name);
	NULL_PTR_CHECK(tmp_name, false);
	bool offer_deleted = true;
	while (offer_deleted) {
		offer_deleted = false;
		SET_FOREACH(PriceOffer, current, offer_set)
		{
			if (!emailCompare(realtor_email, priceOfferGetRealtorEmail(current))
				&& !strcmp(tmp_name, priceOfferGetServiceName(current))
				&& id == priceOfferGetApartmentId(current)) {
				setRemove(offer_set, current);
				offer_deleted = true;
			}
		}
	}
	free((char*) tmp_name);
	return true;
}

/* deletes all price offers for a service by name
 * Use when deleting services */
static void deleteAllOffersByServiceName(Set offer_set,
	const char* service_name) {
	assert(offer_set && service_name);
	bool offer_deleted = true;
	while (offer_deleted) {
		offer_deleted = false;
		SET_FOREACH(PriceOffer, current, offer_set)
		{
			if (strcmp(priceOfferGetServiceName(current), service_name) == 0) {
				setRemove(offer_set, current);
			}
		}
	}
}

/*
 * create a set element with only unique parameters initialized
 * (all other parameters take arbitrary values, usually illegal)
 * Mostly used for searching sets for elements with given id
 */
static inline PriceOffer createTmpOffer(Email customer_email,
	Email realtor_email) {
	return priceOfferCreate(customer_email, realtor_email, "", -1, -1);
}

/* Returns true if offer exists, and if so, returns pointer to offer to output
 * variable price_offer_out. Otherwise, price_offer_out stays unchanged */
static bool getOfferByEmails(Yad3Service yad_3, Email realtor_email,
	Email customer_email, PriceOffer* price_offer_out) {
	assert(yad_3 && price_offer_out); //create offer function handles NULL email
	PriceOffer tmp_offer = createTmpOffer(customer_email, realtor_email);
	assert(tmp_offer);
	bool result = false;
	SET_FOREACH(PriceOffer, current, yad_3->offers)
	{
		if (!priceOfferCompare(tmp_offer, current)) {
			*price_offer_out = current;
			result = true;
			break;
		}
	}
	priceOfferDestroy(tmp_offer);
	return result;
}

/* Returns true if offer exists. Unlike getOfferByEmails, doesn't return or
 * copy the offer in any way. */
static bool offerExists(Set set_of_offers, Email customer_email,
	Email realtor_email) {
	assert(set_of_offers && customer_email && realtor_email);
	PriceOffer offer = createTmpOffer(customer_email, realtor_email);
	assert(offer);
	bool result = setIsIn(set_of_offers, offer);
	priceOfferDestroy(offer);
	return result;
}

/* Returns true if offered_price is lower than regular price + tax */
static inline bool priceIsLogical(int offerred_price, int old_price,
	Realtor realtor) {
	assert(realtor);
	return offerred_price
		< (((100 + realtorGetTaxPercentage(realtor)) * old_price) / 100);
}

//====Sorting and filtering for report functions================================
/*This filter removes each realtor, that doesn't have any apartment,
 * customer can buy. */
static bool filterRealtorHasSuitableApartments(ListElement realtorAndEmail,
	ListFilterKey customer) {
	assert(realtorAndEmail && customer);
	Apartment tmp_apartment = NULL;
	Realtor realtor = ((UserAndEmail) realtorAndEmail)->user;
	RealtorResult result = realtorSearchAllServices(realtor,
		customerGetMinArea(customer), customerGetMinRooms(customer),
		customerGetMaxPrice(customer), &tmp_apartment);
	apartmentDestroy(tmp_apartment);
	return result == REALTOR_SUCCESS;
}

/* This filter removes each realtor, who's rating is lower or equal
 * to min_rating */
static bool filterRealtorByMinRating(ListElement realtorAndEmail,
	ListFilterKey min_rating) {
	assert(realtorAndEmail && min_rating);
	Realtor realtor = ((UserAndEmail) realtorAndEmail)->user;
	return realtorGetInfluenceRating(realtor) > (*(int*) min_rating);
}

/* This filter removes each customer, who paid less or equal
 * then min_payment */
static bool filterCustomerByMinPayment(ListElement customerAndEmail,
	ListFilterKey min_payment) {
	assert(customerAndEmail && min_payment);
	Customer customer = ((UserAndEmail) customerAndEmail)->user;
	return customerGetMoneyPaid(customer) > (*(int*) min_payment);
}

/* Adapter function for UserAndEmail type and emailCompare function */
static int sortUserByMail(ListElement user_1, ListElement user_2) {
	assert(user_1 && user_2);
	return emailCompare(((UserAndEmail) user_1)->email,
		((UserAndEmail) user_2)->email);
}

/* Look sortCustomersByPaymentAndEmail and sortRealtorsByRatingAndEmail
 * comments */
static int sortUserByTwoConditions(UserAndEmail user_1, UserAndEmail user_2,
	CompareListElements primaryCompare, CompareListElements secondaryCompare) {
	assert(user_1 && user_2);
	int result = -primaryCompare(user_1->user, user_2->user);
	if (!result) {
		return secondaryCompare(user_1, user_2);
	}
	return result;
}
/* Sorting by money paid (customers only) in descending order
 * sub-sorting (for users with same amount of money paid) in alphabetical
 * ascending order of email */
static int sortCustomersByPaymentAndEmail(ListElement user_1,
	ListElement user_2) {
	return sortUserByTwoConditions((UserAndEmail) user_1, (UserAndEmail) user_2,
		(CompareListElements) customerCompareByMoneyPaid, sortUserByMail);
}
/* Sorting by rating (realtors only) in descending order
 * sub-sorting (for users with same rating) in alphabetical
 * ascending order of email */
static int sortRealtorsByRatingAndEmail(ListElement user_1, ListElement user_2){
	return sortUserByTwoConditions((UserAndEmail) user_1, (UserAndEmail) user_2,
		(CompareListElements) realtorCompareByRating, sortUserByMail);
}

/* Gets Map and copy/destroy functions, user provided when creating that map.
 * filter and compare are pointers to filter and compare functions, provided
 * by user (look CompareListElements and FilterListElement in list.h)
 * listFilterKey is an additional parameter for filter function (look list.h)
 * Converts Map to List, and applies given filter and then sorting function.
 * Returns NULL in case of malloc errors */
static List convertFilterAndSort(Map map, UserCopy userCopy,
	UserDestroy userDestroy, ListFilterKey listFilterKey,
	FilterListElement filter, CompareListElements compare) {
	List list = convertUserMapToList(map, userCopy, userDestroy);
	NULL_PTR_CHECK(list, NULL);
	List filtered_list = listFilter(list, filter, listFilterKey);
	listDestroy(list);
	NULL_PTR_CHECK(filtered_list, NULL);
	ListResult result = listSort(filtered_list, compare);
	IF_NOT_EQUAL_RETURN(result, LIST_SUCCESS, NULL);
	return filtered_list;
}

//====Public functions==========================================================
Yad3Service yad3ServiceCreate() {
	Yad3Service new_service = malloc(sizeof(*new_service));
	NULL_PTR_CHECK(new_service, NULL);
	new_service->realtors = createMapOfRealtors();
	new_service->customers = createMapOfCustomers();
	new_service->offers = createSetOfOffers();
	if (!new_service->realtors || !new_service->customers
		|| !new_service->offers) {
		yad3ServiceDestroy(new_service);
		return NULL;
	}
	return new_service;
}

void yad3ServiceDestroy(Yad3Service yad_3) {
	if (!yad_3) {
		return;
	}
	mapDestroy(yad_3->realtors);
	mapDestroy(yad_3->customers);
	setDestroy(yad_3->offers);
	free(yad_3);
}

Yad3Result yad3RealtorAdd(Yad3Service yad_3, const Email email,
	const char* company_name, int tax_percentage) {
	NULL_PTR_CHECK(yad_3, YAD3_NULL_ARG);
	NULL_PTR_CHECK(company_name, YAD3_NULL_ARG);
	NULL_PTR_CHECK(email, YAD3_NULL_ARG);
	if (!emailIsValid(email) || !taxIsValid(tax_percentage)) {
		return YAD3_INVALID_PARAMETERS;
	}
	if (checkUserExists(yad_3, email)) {
		return YAD3_EMAIL_ALREADY_EXISTS;
	}
	Realtor new_realtor = realtorCreate(company_name, tax_percentage);
	NULL_PTR_CHECK(new_realtor, YAD3_OUT_OF_MEMORY);
	MapResult result = mapPut(yad_3->realtors, email, new_realtor);
	realtorDestroy(new_realtor);
	return errorConvertMapToYad3(result);
}

Yad3Result yad3RealtorRemove(Yad3Service yad_3, const Email email) {
	NULL_PTR_CHECK(yad_3, YAD3_NULL_ARG);
	Yad3Result email_check_result = checkExistingEmail(yad_3, email, REALTOR);
	IF_NOT_EQUAL_RETURN(email_check_result, YAD3_SUCCESS, email_check_result);
	MapResult result = mapRemove(yad_3->realtors, email);
	if (result == MAP_SUCCESS) {
		deleteAllOffersByEmail(yad_3->offers, email);
	}
	return errorConvertMapToYad3(result);
}

Yad3Result yad3RealtorAddApartment(Yad3Service yad_3, const Email email,
	const char* service_name, int id, int width, int height, int price,
	const char* matrix) {
	if (!yad_3 || !service_name || !matrix) {
		return YAD3_NULL_ARG;
	}
	if (price <= 0 || price % PRICE_INCREMENT_STEP != 0 || id < 0 || width <= 0
		|| height <= 0) {
		return YAD3_INVALID_PARAMETERS;
	}
	Yad3Result email_check_result = checkExistingEmail(yad_3, email, REALTOR);
	IF_NOT_EQUAL_RETURN(email_check_result, YAD3_SUCCESS, email_check_result);
	Realtor realtor = mapGet(yad_3->realtors, email); //safe after email check
	RealtorResult result = realtorAddApartment(realtor, service_name, id, width,
		height, matrix, price);
	return errorConvertRealtorToYad3(result);
}

Yad3Result yad3RealtorRemoveApartment(Yad3Service yad_3, const Email email,
	const char* service_name, const int id) {
	NULL_PTR_CHECK(yad_3, YAD3_NULL_ARG);
	NULL_PTR_CHECK(service_name, YAD3_NULL_ARG);
	NON_NEGATIVE_CHECK(id, YAD3_INVALID_PARAMETERS);
	Yad3Result email_check_result = checkExistingEmail(yad_3, email, REALTOR);
	IF_NOT_EQUAL_RETURN(email_check_result, YAD3_SUCCESS, email_check_result);
	Realtor realtor = mapGet(yad_3->realtors, email);
	RealtorResult result = realtorRemoveApartment(realtor, service_name, id);
	if (result == REALTOR_SUCCESS) {
		if (!deleteAllOffersById(yad_3->offers, email, service_name, id)) {
			return YAD3_OUT_OF_MEMORY;
		}
	}
	return errorConvertRealtorToYad3(result);
}

Yad3Result yad3RealtorAddApartmentService(Yad3Service yad_3, const Email email,
	const char* service_name, int max_apartments) {
	NULL_PTR_CHECK(yad_3, YAD3_NULL_ARG);
	NULL_PTR_CHECK(service_name, YAD3_NULL_ARG);
	POSITIVE_CHECK(max_apartments, YAD3_INVALID_PARAMETERS);
	Yad3Result email_check_result = checkExistingEmail(yad_3, email, REALTOR);
	IF_NOT_EQUAL_RETURN(email_check_result, YAD3_SUCCESS, email_check_result);
	Realtor realtor = mapGet(yad_3->realtors, email);
	RealtorResult result = realtorAddApartmentService(realtor, service_name,
		max_apartments);
	return errorConvertRealtorToYad3(result);
}

Yad3Result yad3RealtorRemoveApartmentService(Yad3Service yad_3,
	const Email email, const char* service_name) {
	NULL_PTR_CHECK(yad_3, YAD3_NULL_ARG);
	NULL_PTR_CHECK(service_name, YAD3_NULL_ARG);
	Yad3Result email_check_result = checkExistingEmail(yad_3, email, REALTOR);
	IF_NOT_EQUAL_RETURN(email_check_result, YAD3_SUCCESS, email_check_result);
	Realtor realtor = mapGet(yad_3->realtors, email);
	RealtorResult result = realtorRemoveApartmentService(realtor, service_name);
	if (result == REALTOR_SUCCESS) {
		deleteAllOffersByServiceName(yad_3->offers, service_name);
	}
	return errorConvertRealtorToYad3(result);
}

Yad3Result yad3CustomerAdd(Yad3Service yad_3, const Email email, int min_area,
	int min_rooms, int max_price) {
	NULL_PTR_CHECK(yad_3, YAD3_NULL_ARG);
	NULL_PTR_CHECK(email, YAD3_NULL_ARG);
	if (!emailIsValid(email) || min_area <= 0 || min_rooms <= 0
		|| max_price <= 0) {
		return YAD3_INVALID_PARAMETERS;
	}
	if (checkUserExists(yad_3, email)) {
		return YAD3_EMAIL_ALREADY_EXISTS;
	}
	Customer new_customer = customerCreate(min_area, min_rooms, max_price);
	NULL_PTR_CHECK(new_customer, YAD3_OUT_OF_MEMORY);
	MapResult add_to_map_result = mapPut(yad_3->customers, email, new_customer);
	customerDestroy(new_customer);
	return errorConvertMapToYad3(add_to_map_result);
}

Yad3Result yad3CustomerRemove(Yad3Service yad_3, const Email email) {
	NULL_PTR_CHECK(yad_3, YAD3_NULL_ARG);
	Yad3Result email_check_result = checkExistingEmail(yad_3, email, CUSTOMER);
	IF_NOT_EQUAL_RETURN(email_check_result, YAD3_SUCCESS, email_check_result);
	MapResult remove_from_map_result = mapRemove(yad_3->customers, email);
	if (remove_from_map_result == MAP_SUCCESS) {
		deleteAllOffersByEmail(yad_3->offers, email);
	}
	return errorConvertMapToYad3(remove_from_map_result);
}

Yad3Result yad3CustomerPurchase(Yad3Service yad_3, const Email customer_email,
	const Email realtor_email, const char* service_name, const int id) {
	NULL_PTR_CHECK(yad_3, YAD3_NULL_ARG);
	NULL_PTR_CHECK(service_name, YAD3_NULL_ARG);
	NON_NEGATIVE_CHECK(id, YAD3_INVALID_PARAMETERS);
	Yad3Result result = checkEmails(yad_3, realtor_email, customer_email);
	IF_NOT_EQUAL_RETURN(result, YAD3_SUCCESS, result);
	Apartment apartment = NULL;
	Realtor realtor = mapGet(yad_3->realtors, realtor_email);
	result = errorConvertRealtorToYad3(
		realtorGetApartmentById(realtor, service_name, id, &apartment));
	IF_NOT_EQUAL_RETURN(result, YAD3_SUCCESS, result);
	//add tax, original apartment stays unchanged
	apartmentChangePrice(apartment, realtorGetTaxPercentage(realtor));
	Customer customer = mapGet(yad_3->customers, customer_email);
	if (!customerApartmentFitsRequierements(customer, apartment)) {
		apartmentDestroy(apartment);
		return YAD3_PURCHASE_WRONG_PROPERTIES;
	}
	//offers are deleted, when apartment removed
	yad3RealtorRemoveApartment(yad_3, realtor_email, service_name, id);
	customerPay(customer, apartmentGetPrice(apartment));
	apartmentDestroy(apartment);
	return YAD3_SUCCESS;
}

Yad3Result yad3CustomerMakeOffer(Yad3Service yad_3, Email customer_email,
	Email realtor_email, const char* service_name, int id, int new_price) {
	NULL_PTR_CHECK(yad_3, YAD3_NULL_ARG);
	NULL_PTR_CHECK(service_name, YAD3_NULL_ARG);
	NON_NEGATIVE_CHECK(id, YAD3_INVALID_PARAMETERS);
	POSITIVE_CHECK(new_price, YAD3_INVALID_PARAMETERS);
	Yad3Result result = checkEmails(yad_3, realtor_email, customer_email);
	IF_NOT_EQUAL_RETURN(result, YAD3_SUCCESS, result);
	if (offerExists(yad_3->offers, customer_email, realtor_email)) {
		return YAD3_ALREADY_REQUESTED;
	}
	Realtor realtor = mapGet(yad_3->realtors, realtor_email);
	Customer customer = mapGet(yad_3->customers, customer_email);
	Apartment apartment = NULL;
	result = errorConvertRealtorToYad3(
		realtorGetApartmentById(realtor, service_name, id, &apartment));
	IF_NOT_EQUAL_RETURN(result, YAD3_SUCCESS, result);
	int old_price = apartmentGetPrice(apartment);
	apartmentChangePrice(apartment, -100);
	//Setting price (of copy) to 0 to check only area and rooms requirements
	if (new_price > customerGetMaxPrice(customer)
		|| !customerApartmentFitsRequierements(customer, apartment)) {
		apartmentDestroy(apartment);
		return YAD3_REQUEST_WRONG_PROPERTIES;
	}
	apartmentDestroy(apartment);
	if (!priceIsLogical(new_price, old_price, realtor)) {
		return YAD3_REQUEST_ILLOGICAL_PRICE;
	}
	PriceOffer new_offer = priceOfferCreate(customer_email, realtor_email,
		service_name, id, new_price);
	NULL_PTR_CHECK(new_offer, YAD3_OUT_OF_MEMORY);
	SetResult add_to_set_result = setAdd(yad_3->offers, new_offer);
	priceOfferDestroy(new_offer);
	return errorConverSetToYad3(add_to_set_result);
}

Yad3Result yad3RealtorRespondToOffer(Yad3Service yad_3,
	const Email customer_email, const Email realtor_email, Response response) {
	NULL_PTR_CHECK(yad_3, YAD3_NULL_ARG);
	Yad3Result result = checkEmails(yad_3, realtor_email, customer_email);
	IF_NOT_EQUAL_RETURN(result, YAD3_SUCCESS, result);
	PriceOffer offer = NULL;
	if (!getOfferByEmails(yad_3, realtor_email, customer_email, &offer)) {
		return YAD3_NOT_REQUESTED;
	}
	if (response == ACCEPT) {
		Realtor realtor = mapGet(yad_3->realtors, realtor_email);
		const char* service_name = priceOfferGetServiceName(offer);
		int id = priceOfferGetApartmentId(offer);
		realtorChangeApartmentPrice(realtor, service_name, id, -100);
		Customer customer = mapGet(yad_3->customers, customer_email);
		//we set price to 0, and then take exact sum customer offered
		customerPay(customer, priceOfferGetPrice(offer));
#ifndef NDEBUG
		assert(yad3CustomerPurchase(yad_3, customer_email, realtor_email,
				service_name, id) == YAD3_SUCCESS);
#else
		yad3CustomerPurchase(yad_3, customer_email, realtor_email,
			service_name, id);
#endif
	} else {
		setRemove(yad_3->offers, offer);
	}
	return YAD3_SUCCESS;
}

Yad3Result yad3ReportRelevantRealtors(Yad3Service yad_3, const Email email,
	FILE* output) {
	assert(output);
	NULL_PTR_CHECK(yad_3, YAD3_NULL_ARG);
	Yad3Result email_check_result = checkExistingEmail(yad_3, email, CUSTOMER);
	IF_NOT_EQUAL_RETURN(email_check_result, YAD3_SUCCESS, email_check_result);
	Customer customer = mapGet(yad_3->customers, email);
	List list = convertFilterAndSort(yad_3->realtors, (UserCopy) realtorCopy,
		(UserDestroy) realtorDestroy, (ListFilterKey) customer,
		filterRealtorHasSuitableApartments, sortUserByMail);
	NULL_PTR_CHECK(list, YAD3_OUT_OF_MEMORY);
	LIST_FOREACH(UserAndEmail, current, list)
	{
		mtmPrintRealtor(output, current->email,
			realtorGetCompanyName(current->user));
	}
	listDestroy(list);
	return YAD3_SUCCESS;
}

Yad3Result yad3ReportMostPayingCustomers(Yad3Service yad_3, int count,
	FILE* output) {
	assert(output);
	NULL_PTR_CHECK(yad_3, YAD3_NULL_ARG);
	POSITIVE_CHECK(count, YAD3_INVALID_PARAMETERS);
	int min_payment = REPORT_MIN_CUSTOMER_PAYMENT;
	List list = convertFilterAndSort(yad_3->customers, (UserCopy) customerCopy,
		(UserDestroy) customerDestroy, (ListFilterKey) &min_payment,
		filterCustomerByMinPayment, sortCustomersByPaymentAndEmail);
	NULL_PTR_CHECK(list, YAD3_OUT_OF_MEMORY);
	LIST_FOREACH(UserAndEmail,current,list)
	{
		if (!count--) {
			break;
		}
		mtmPrintCustomer(output, current->email,
			customerGetMoneyPaid(current->user));
	}
	listDestroy(list);
	return YAD3_SUCCESS;
}

Yad3Result yad3ReportSignificantRealtors(Yad3Service yad_3, int count,
	FILE* output) {
	assert(output);
	NULL_PTR_CHECK(yad_3, YAD3_NULL_ARG);
	POSITIVE_CHECK(count, YAD3_INVALID_PARAMETERS);
	int min_rating = REPORT_MIN_REALTOR_RATING;
	List list = convertFilterAndSort(yad_3->realtors, (UserCopy) realtorCopy,
		(UserDestroy) realtorDestroy, (ListFilterKey) &min_rating,
		filterRealtorByMinRating, sortRealtorsByRatingAndEmail);
	NULL_PTR_CHECK(list, YAD3_OUT_OF_MEMORY);
	LIST_FOREACH(UserAndEmail,current,list)
	{
		if (!count--) {
			break;
		}
		mtmPrintRealtor(output, current->email,
			realtorGetCompanyName(current->user));
	}
	listDestroy(list);
	return YAD3_SUCCESS;
}
