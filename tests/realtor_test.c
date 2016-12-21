/*
 * realtor_test.c
 *
 *  Created on: Apr 20, 2016
 *
 */

#include "../realtor.h"

#include <stdbool.h>
#include <string.h>

#include "test_utilities.h"

//=============================================================================
//"service 1" with 4 apartments, "service 2" with 3 (full)
static void addServicesAndApartments(Realtor realtor) {
	realtorAddApartmentService(realtor, "service 1", 99);
	//price median = 3000, area median 6
	realtorAddApartment(realtor, "service 1", 1001, 2, 3, "eweewe", 100);
	//area 4, 2 rooms
	realtorAddApartment(realtor, "service 1", 1002, 3, 4, "eeweeeweeewe", 700);
	//area 9, 2 rooms
	realtorAddApartment(realtor, "service 1", 1003, 4, 5,
			"eeweeeeweewwweeeeeee", 5300); //area 15, 2 rooms
	realtorAddApartment(realtor, "service 1", 1004, 2, 2, "eeee", 89000);
	//area 4, 1 room
	realtorAddApartmentService(realtor, "service 2", 3);
	//price median = 900, area median 15
	realtorAddApartment(realtor, "service 2", 1001, 3, 4, "eeweeeweeewe", 100);
	//area 9, 2 rooms
	realtorAddApartment(realtor, "service 2", 1002, 8, 9,
	"eeeeweeeeeeeweweeeeeewwweeeeeeweweeeeeeweweeeewwwewwweweeeweeeweeeeeeeee",
			900); //area 53, 4 rooms
	realtorAddApartment(realtor, "service 2", 1003, 4, 5,
			"eeweeeeweewwweeeeeee", 35000); //area 15, 2 rooms
}
//=============================================================================

bool realtor_create_destroy_test() {
	Realtor realtor = NULL;
	ASSERT_TEST(NULL==(realtor = realtorCreate(NULL, 40)));
	ASSERT_TEST(NULL!=(realtor = realtorCreate("go", 40)));
	ASSERT_TEST(0 == realtorGetInfluenceRating(realtor));
	realtorDestroy(NULL);
	realtorDestroy(realtor);
	return true;
}

bool realtor_add_remove_service_test() {
	Realtor realtor = realtorCreate("Vamonos pest", 17);
	//Add
	ASSERT_TEST(REALTOR_NULL_ARG ==
		realtorAddApartmentService(NULL, "service1", 99));
	ASSERT_TEST(REALTOR_NULL_ARG ==
		realtorAddApartmentService(realtor, NULL, 99));
	ASSERT_TEST(REALTOR_INVALID_PARAMETERS ==
		realtorAddApartmentService(realtor, "service1", 0));
	ASSERT_TEST(REALTOR_INVALID_PARAMETERS ==
		realtorAddApartmentService(realtor, "service1", -10));
	ASSERT_TEST(REALTOR_SUCCESS ==
		realtorAddApartmentService(realtor, "service1", 99));
	ASSERT_TEST(REALTOR_APARTMENT_SERVICE_ALREADY_EXISTS ==
		realtorAddApartmentService(realtor, "service1", 99));
	ASSERT_TEST(REALTOR_SUCCESS ==
		realtorAddApartmentService(realtor, "service2", 2));
	//Remove
	ASSERT_TEST(REALTOR_NULL_ARG ==
		realtorRemoveApartmentService(NULL, "service1"));
	ASSERT_TEST(REALTOR_NULL_ARG ==
		realtorRemoveApartmentService(realtor, NULL));
	ASSERT_TEST(REALTOR_APARTMENT_SERVICE_DOES_NOT_EXIST ==
		realtorRemoveApartmentService(realtor, "service3"));
	ASSERT_TEST(REALTOR_SUCCESS ==
		realtorRemoveApartmentService(realtor, "service1"));
	ASSERT_TEST(REALTOR_APARTMENT_SERVICE_DOES_NOT_EXIST ==
		realtorRemoveApartmentService(realtor, "service1"));
	realtorDestroy(realtor);
	return true;
}

bool realtor_copy_compare_by_rating_test() {
	Realtor realtor = realtorCreate("Vamonos pest", 17);
	ASSERT_TEST(NULL == realtorCopy(NULL));
	Realtor copy = realtorCopy(realtor);
	ASSERT_TEST(NULL != copy);
	ASSERT_TEST(0 == realtorCompareByRating(realtor, copy));
	addServicesAndApartments(realtor);
	//"service 1" with 4 apartments, "service 2" with 3 (full)
	ASSERT_TEST(0 < realtorCompareByRating(realtor, copy));
	addServicesAndApartments(copy);
	//"service 1" with 4 apartments, "service 2" with 3 (full)
	ASSERT_TEST(0 == realtorCompareByRating(realtor, copy));
	ASSERT_TEST(
			REALTOR_SUCCESS
					== realtorRemoveApartmentService(realtor, "service 1"));
	ASSERT_TEST(0 > realtorCompareByRating(realtor, copy));
	realtorDestroy(realtor);
	realtorDestroy(copy);
	return true;
}

bool realtor_add_remove_apartment_test() {
	Realtor realtor = realtorCreate("Vamonos pest", 17);
	realtorAddApartmentService(realtor, "s", 2);
	//Add
	ASSERT_TEST(REALTOR_NULL_ARG ==
		realtorAddApartment(NULL, "s", 111, 2, 3, "eweewe", 300));
	ASSERT_TEST(REALTOR_NULL_ARG ==
		realtorAddApartment(realtor, NULL, 1, 2, 3, "eweewe", 300));
	ASSERT_TEST(REALTOR_NULL_ARG ==
		realtorAddApartment(realtor, "s", 111, 2, 3, NULL, 300));
	ASSERT_TEST(REALTOR_INVALID_PARAMETERS ==
		realtorAddApartment(realtor, "s", 111, 0, 3, "eweewe", 300));
	ASSERT_TEST(REALTOR_INVALID_PARAMETERS ==
		realtorAddApartment(realtor, "s", 111, 2, 0, "eweewe", 300));
	ASSERT_TEST(REALTOR_INVALID_PARAMETERS ==
		realtorAddApartment(realtor, "s", 111, -2, 3, "eweewe", 300));
	ASSERT_TEST(REALTOR_INVALID_PARAMETERS ==
		realtorAddApartment(realtor, "s", 111, 2, -3, "eweewe", 300));
	ASSERT_TEST(REALTOR_INVALID_PARAMETERS ==
		realtorAddApartment(realtor, "s", -111, 2, 3, "eweewe", 300));
	ASSERT_TEST(REALTOR_APARTMENT_SERVICE_DOES_NOT_EXIST ==
		realtorAddApartment(realtor, "no service", 111, 2, 3, "eweewe",300000));
	ASSERT_TEST(REALTOR_SUCCESS ==
		realtorAddApartment(realtor, "s", 111, 2, 3, "eweewe", 300));
	ASSERT_TEST(REALTOR_APARTMENT_ALREADY_EXISTS ==
		realtorAddApartment(realtor, "s", 111, 2, 3, "eweewe", 300));
	ASSERT_TEST(REALTOR_SUCCESS ==
		realtorAddApartment(realtor, "s", 222, 2, 2, "ewew", 500));
	ASSERT_TEST(REALTOR_APARTMENT_SERVICE_FULL ==
		realtorAddApartment(realtor, "s", 333, 1, 1, "e", 3000));
	//Remove
	ASSERT_TEST(REALTOR_NULL_ARG == realtorRemoveApartment(NULL, "s", 111));
	ASSERT_TEST(REALTOR_NULL_ARG == realtorRemoveApartment(realtor, NULL, 111));
	ASSERT_TEST(REALTOR_INVALID_PARAMETERS ==
		realtorRemoveApartment(realtor, "s", -111));
	ASSERT_TEST(REALTOR_APARTMENT_SERVICE_DOES_NOT_EXIST ==
		realtorRemoveApartment(realtor, "no service", 111));
	ASSERT_TEST(REALTOR_APARTMENT_NOT_FOUND ==
		realtorRemoveApartment(realtor, "s", 777));
	ASSERT_TEST(REALTOR_SUCCESS == realtorRemoveApartment(realtor, "s", 111));
	ASSERT_TEST(REALTOR_APARTMENT_NOT_FOUND ==
		realtorRemoveApartment(realtor, "s", 111));
	realtorDestroy(realtor);
	return true;
}

bool realtor_get_influence_rating_test() {
	Realtor realtor_1 = realtorCreate("Vamonos pest", 17);
	Realtor realtor_2 = realtorCreate("Vamonos pest", 99);
	addServicesAndApartments(realtor_1);
	//"service 1" with 4 apartments, "service 2" with 3 (full)
	//Get influence rating
	ASSERT_TEST(0 == realtorGetInfluenceRating(realtor_2));
	ASSERT_TEST(
			(7 * 1000000 + (3000 + 900) / 2 + 100000 * ((15 + 6) / 2))
					== realtorGetInfluenceRating(realtor_1));
	realtorRemoveApartmentService(realtor_1, "service 2");
	ASSERT_TEST(
			(4 * 1000000 + 3000 + 100000 * 6)
					== realtorGetInfluenceRating(realtor_1));
	realtorRemoveApartment(realtor_1, "service 1", 1003);
	ASSERT_TEST(
			(3 * 1000000 + 700 + 100000 * 4)
					== realtorGetInfluenceRating(realtor_1));
	realtorRemoveApartmentService(realtor_1, "service 1");
	ASSERT_TEST(0 == realtorGetInfluenceRating(realtor_1));
	realtorDestroy(realtor_1);
	realtorDestroy(realtor_2);
	return true;
}

bool realtor_get_tax_rate_get_company_name() {
	Realtor realtor_1 = realtorCreate("Vamonos pest", 17);
	Realtor realtor_2 = realtorCreate("Madrigal", 99);
	//Get tax rate
	ASSERT_TEST(17 == realtorGetTaxPercentage(realtor_1));
	ASSERT_TEST(99 == realtorGetTaxPercentage(realtor_2));
	//Get company name
	ASSERT_TEST(0 == strcmp(realtorGetCompanyName(realtor_1), "Vamonos pest"));
	ASSERT_TEST(0 != strcmp(realtorGetCompanyName(realtor_2), "Vamonos pest"));
	ASSERT_TEST(0 == strcmp(realtorGetCompanyName(realtor_2), "Madrigal"));
	realtorDestroy(realtor_1);
	realtorDestroy(realtor_2);
	return true;
}

bool realtor_search_all_services() {
	Realtor realtor_1 = realtorCreate("Vamonos pest", 17);
	Realtor realtor_2 = realtorCreate("Madrigal", 99);
	Realtor realtor_3 = realtorCreate("Nothing to sell", 1);
	addServicesAndApartments(realtor_1);
	addServicesAndApartments(realtor_2);
	//"service 1" with 4 apartments, "service 2" with 3 (full)
	Apartment apartment = NULL;
	ASSERT_TEST(REALTOR_NULL_ARG ==
		realtorSearchAllServices(NULL, 12, 2, 450000, &apartment));
	ASSERT_TEST(REALTOR_NULL_ARG ==
		realtorSearchAllServices(realtor_1, 12, 2, 450000, NULL));
	ASSERT_TEST(REALTOR_INVALID_PARAMETERS ==
		realtorSearchAllServices(realtor_1, -12, 2, 450000, &apartment));
	ASSERT_TEST(REALTOR_INVALID_PARAMETERS ==
		realtorSearchAllServices(realtor_1, 12, -2, 450000, &apartment));
	ASSERT_TEST(REALTOR_INVALID_PARAMETERS ==
		realtorSearchAllServices(realtor_1, 12, 2, -450000, &apartment));
	ASSERT_TEST(REALTOR_APARTMENT_NOT_FOUND ==
		realtorSearchAllServices(realtor_3, 1, 1, 450000, &apartment));
	ASSERT_TEST(REALTOR_APARTMENT_NOT_FOUND ==
		realtorSearchAllServices(realtor_1, 54, 1, 450000, &apartment));
	//max area 53
	ASSERT_TEST(REALTOR_APARTMENT_NOT_FOUND ==
		realtorSearchAllServices(realtor_1, 1, 5, 450000, &apartment));
	//max rooms 4
	ASSERT_TEST(REALTOR_APARTMENT_NOT_FOUND ==
		realtorSearchAllServices(realtor_1, 1, 1, 116, &apartment));
	//lowest price: 117
	ASSERT_TEST(REALTOR_APARTMENT_NOT_FOUND ==
		realtorSearchAllServices(realtor_2, 1, 1, 198, &apartment));
	//lowest price: 199
	ASSERT_TEST(REALTOR_SUCCESS ==
		realtorSearchAllServices(realtor_1, 1, 1, 117, &apartment));
	apartmentDestroy(apartment);
	ASSERT_TEST(REALTOR_SUCCESS ==
		realtorSearchAllServices(realtor_2, 1, 1, 199, &apartment));
	apartmentDestroy(apartment);
	ASSERT_TEST(REALTOR_SUCCESS ==
		realtorSearchAllServices(realtor_1, 53, 1, 450000, &apartment));
	apartmentDestroy(apartment);
	ASSERT_TEST(REALTOR_SUCCESS ==
		realtorSearchAllServices(realtor_1, 1, 4, 450000, &apartment));
	apartmentDestroy(apartment);
	realtorDestroy(realtor_1);
	realtorDestroy(realtor_2);
	realtorDestroy(realtor_3);
	return true;
}

bool realtor_get_apartment_by_id_test() {
	Realtor realtor_1 = realtorCreate("Vamonos pest", 17);
	addServicesAndApartments(realtor_1);
	Apartment apartment = NULL;
	ASSERT_TEST(REALTOR_NULL_ARG ==
		realtorGetApartmentById(NULL, "service 1", 1003, &apartment));
	ASSERT_TEST(REALTOR_NULL_ARG ==
		realtorGetApartmentById(realtor_1, NULL, 1003, &apartment));
	ASSERT_TEST(REALTOR_NULL_ARG ==
		realtorGetApartmentById(realtor_1, "service 1", 1003, NULL));
	ASSERT_TEST(REALTOR_APARTMENT_SERVICE_DOES_NOT_EXIST ==
		realtorGetApartmentById(realtor_1, "no service", 1003, &apartment));
	ASSERT_TEST(REALTOR_INVALID_PARAMETERS ==
		realtorGetApartmentById(realtor_1, "service 1", -1003, &apartment));
	ASSERT_TEST(REALTOR_APARTMENT_NOT_FOUND ==
		realtorGetApartmentById(realtor_1, "service 1", 1333354, &apartment));
	ASSERT_TEST(REALTOR_SUCCESS ==
		realtorGetApartmentById(realtor_1, "service 1", 1003, &apartment));
	ASSERT_TEST(5300 == apartmentGetPrice(apartment));
	realtorDestroy(realtor_1);
	apartmentDestroy(apartment);
	return true;
}

bool realtor_change_apartment_price_test() {
	Realtor realtor_1 = realtorCreate("Vamonos pest", 17);
	addServicesAndApartments(realtor_1);
	ASSERT_TEST(REALTOR_NULL_ARG ==
		realtorChangeApartmentPrice(NULL, "service 1", 1003, 10));
	ASSERT_TEST(REALTOR_NULL_ARG ==
		realtorChangeApartmentPrice(realtor_1, NULL, 1003, 10));
	ASSERT_TEST(REALTOR_APARTMENT_SERVICE_DOES_NOT_EXIST ==
		realtorChangeApartmentPrice(realtor_1, "no service", 1003, 10));
	ASSERT_TEST(REALTOR_INVALID_PARAMETERS ==
		realtorChangeApartmentPrice(realtor_1, "service 1", -1003, 10));
	ASSERT_TEST(REALTOR_APARTMENT_NOT_FOUND ==
		realtorChangeApartmentPrice(realtor_1, "service 1", 23545, 10));
	ASSERT_TEST(REALTOR_SUCCESS ==
		realtorChangeApartmentPrice(realtor_1, "service 1", 1003, 10));
	Apartment apartment = NULL;
	realtorGetApartmentById(realtor_1, "service 1", 1003, &apartment);
	ASSERT_TEST(5830 == apartmentGetPrice(apartment));
	realtorDestroy(realtor_1);
	apartmentDestroy(apartment);
	return true;
}

int main() {
	RUN_TEST(realtor_create_destroy_test);
	RUN_TEST(realtor_copy_compare_by_rating_test);
	RUN_TEST(realtor_add_remove_service_test);
	RUN_TEST(realtor_add_remove_apartment_test);
	RUN_TEST(realtor_get_influence_rating_test);
	RUN_TEST(realtor_get_tax_rate_get_company_name);
	RUN_TEST(realtor_search_all_services);
	RUN_TEST(realtor_get_apartment_by_id_test);
	RUN_TEST(realtor_change_apartment_price_test);
	return 0;
}
