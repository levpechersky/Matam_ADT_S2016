/*
 * yad3Service_test.c
 *
 *  Created on: Apr 10, 2016
 *
 */

#include "../yad3Service.h"

#include <string.h>

#include "../customer.h"
#include "test_utilities.h"

//=============================================================================
static void addRealtorCustomerAndApartments(Yad3Service yad_3) {
	yad3CustomerAdd(yad_3, "want@to.buy", 10, 2, 5000);
	yad3RealtorAdd(yad_3, "want@to.sell", "Pollos", 10);
	yad3RealtorAddApartmentService(yad_3, "want@to.sell", "Pollos Hermanos", 6);
	char matrix1[] = "eeeweeeeeeweeeeeeweeewwwwwwweeeweee";//5*7,4 rooms,area 24
	char matrix2[] = "eeewwweee";	//3*3, 2 rooms, area 6
	char matrix3[] = "eeeeeeeeeeeee";	//1*13, 1 room, area 13
	yad3RealtorAddApartment(yad_3, "want@to.sell", "Pollos Hermanos",
		222, 7, 5, 500, matrix1);
	yad3RealtorAddApartment(yad_3, "want@to.sell", "Pollos Hermanos",
		1, 7, 5, 20000, matrix1);
	yad3RealtorAddApartment(yad_3, "want@to.sell", "Pollos Hermanos",
		666, 3, 3, 200000, matrix2);
	yad3RealtorAddApartment(yad_3, "want@to.sell", "Pollos Hermanos",
		999, 1, 13, 200000, matrix3);
}
//=============================================================================

bool yad3_create_destroy_test() {
	Yad3Service service = NULL;
	ASSERT_TEST(NULL!=(service = yad3ServiceCreate()));
	yad3ServiceDestroy(NULL);
	yad3ServiceDestroy(service);
	return true;
}

bool yad3_realtor_add_remove_test() {
	Yad3Service yad_3 = yad3ServiceCreate();
	ASSERT_TEST(YAD3_NULL_ARG == yad3RealtorAdd(NULL,"mail@com","brazzers",50));
	ASSERT_TEST(YAD3_NULL_ARG == yad3RealtorAdd(yad_3,NULL,"brazzers",50));
	ASSERT_TEST(YAD3_NULL_ARG == yad3RealtorAdd(yad_3,"mail@com",NULL,50));
	ASSERT_TEST(YAD3_INVALID_PARAMETERS ==
		yad3RealtorAdd(yad_3, "mail@com", "brazzers", 0));
	ASSERT_TEST(YAD3_INVALID_PARAMETERS ==
		yad3RealtorAdd(yad_3, "mailcom", "brazzers", 20));
	ASSERT_TEST(YAD3_INVALID_PARAMETERS ==
		yad3RealtorAdd(yad_3, "mail@com", "brazzers", 101));
	ASSERT_TEST(YAD3_INVALID_PARAMETERS ==
		yad3RealtorAdd(yad_3, "mail@com", "brazzers", -10));
	ASSERT_TEST(YAD3_SUCCESS ==
		yad3RealtorAdd(yad_3, "mail@com", "brazzers", 10));
	//adding 1 customer, than realtor with the same name
	yad3CustomerAdd(yad_3, "no@idea.ca", 4, 7, 5000);
	ASSERT_TEST(YAD3_EMAIL_ALREADY_EXISTS ==
		yad3RealtorAdd(yad_3, "no@idea.ca", "big bang", 23));
	ASSERT_TEST(YAD3_SUCCESS == yad3RealtorAdd(yad_3, "w@com", "wacom", 33));
	ASSERT_TEST(YAD3_NULL_ARG == yad3RealtorRemove(NULL, "mail@com"));
	ASSERT_TEST(YAD3_NULL_ARG == yad3RealtorRemove(yad_3, NULL));
	ASSERT_TEST(YAD3_INVALID_PARAMETERS == yad3RealtorRemove(yad_3, ""));
	ASSERT_TEST(YAD3_EMAIL_WRONG_ACCOUNT_TYPE ==
		yad3RealtorRemove(yad_3, "no@idea.ca"));
	ASSERT_TEST(YAD3_EMAIL_DOES_NOT_EXIST ==
		yad3RealtorRemove(yad_3, "mail@not.found"));
	ASSERT_TEST(YAD3_SUCCESS == yad3RealtorRemove(yad_3, "mail@com"));
	ASSERT_TEST(YAD3_SUCCESS == yad3RealtorRemove(yad_3, "w@com"));
	yad3ServiceDestroy(yad_3);
	return true;
}

bool yad3_realtor_add_remove_service() {
	Yad3Service yad_3 = yad3ServiceCreate();
	yad3RealtorAdd(yad_3, "Saul@goodman", "Saul", 17);
	//Add
	ASSERT_TEST(YAD3_NULL_ARG ==
		yad3RealtorAddApartmentService(NULL,"Saul@goodman","Saul",50));
	ASSERT_TEST(YAD3_NULL_ARG ==
		yad3RealtorAddApartmentService(yad_3,NULL,"Saul",50));
	ASSERT_TEST(YAD3_NULL_ARG ==
		yad3RealtorAddApartmentService(yad_3,"Saul@goodman",NULL,50));
	ASSERT_TEST(YAD3_INVALID_PARAMETERS ==
		yad3RealtorAddApartmentService(yad_3, "Saulgoodman", "Saul", 50));
	ASSERT_TEST(YAD3_INVALID_PARAMETERS ==
		yad3RealtorAddApartmentService(yad_3, "Saul@goodman", "Saul", -50));
	ASSERT_TEST(YAD3_INVALID_PARAMETERS ==
		yad3RealtorAddApartmentService(yad_3, "Saul@goodman", "Saul", 0));
	ASSERT_TEST(YAD3_EMAIL_DOES_NOT_EXIST ==
		yad3RealtorAddApartmentService(yad_3, "Gus@fring", "Pollos", 50));
	yad3CustomerAdd(yad_3, "Jesse@pinkman", 65, 4, 671000);
	ASSERT_TEST(YAD3_EMAIL_WRONG_ACCOUNT_TYPE ==
		yad3RealtorAddApartmentService(yad_3, "Jesse@pinkman", "Pollos", 50));
	ASSERT_TEST(YAD3_SUCCESS ==
		yad3RealtorAddApartmentService(yad_3, "Saul@goodman", "Saul", 50));
	ASSERT_TEST(YAD3_APARTMENT_SERVICE_ALREADY_EXISTS ==
		yad3RealtorAddApartmentService(yad_3, "Saul@goodman", "Saul", 50));
	//Remove
	ASSERT_TEST(YAD3_NULL_ARG ==
		yad3RealtorRemoveApartmentService(NULL,"Saul@goodman","Saul"));
	ASSERT_TEST(YAD3_NULL_ARG ==
		yad3RealtorRemoveApartmentService(yad_3,NULL,"Saul"));
	ASSERT_TEST(YAD3_NULL_ARG ==
		yad3RealtorRemoveApartmentService(yad_3,"Saul@goodman",NULL));
	ASSERT_TEST(YAD3_INVALID_PARAMETERS ==
		yad3RealtorRemoveApartmentService(yad_3, "Saulgoodman", "Saul"));
	ASSERT_TEST(YAD3_EMAIL_DOES_NOT_EXIST ==
		yad3RealtorRemoveApartmentService(yad_3, "Gus@fring", "Pollos"));
	ASSERT_TEST(YAD3_EMAIL_WRONG_ACCOUNT_TYPE ==
		yad3RealtorRemoveApartmentService(yad_3, "Jesse@pinkman", "Pollos"));
	ASSERT_TEST(YAD3_APARTMENT_SERVICE_DOES_NOT_EXIST ==
		yad3RealtorRemoveApartmentService(yad_3, "Saul@goodman", ""));
	ASSERT_TEST(YAD3_SUCCESS ==
		yad3RealtorRemoveApartmentService(yad_3, "Saul@goodman", "Saul"));
	yad3ServiceDestroy(yad_3);
	return true;
}

bool yad3_realtor_add_remove_apartment() {
	Yad3Service yad_3 = yad3ServiceCreate();
	yad3RealtorAdd(yad_3, "Re@ltor", "RealCompany", 17);
	yad3RealtorAddApartmentService(yad_3, "Re@ltor", "RealService", 2);
	char matrix[] = "eeeweeeeeeweeeeeeweeewwwwwwweeeweee"; //5*7
	ASSERT_TEST(YAD3_NULL_ARG == yad3RealtorAddApartment(NULL,"Re@ltor",
		"RealService",12345,7,5,65000,matrix));
	ASSERT_TEST(YAD3_NULL_ARG == yad3RealtorAddApartment(yad_3,NULL,
		"RealService",12345,7,5,65000,matrix));
	ASSERT_TEST(YAD3_NULL_ARG == yad3RealtorAddApartment(yad_3,"Re@ltor",
		NULL,12345,7,5,65000,matrix));
	ASSERT_TEST(YAD3_NULL_ARG == yad3RealtorAddApartment(yad_3,"Re@ltor",
		"RealService",12345,7,5,65000,NULL));
	ASSERT_TEST(YAD3_INVALID_PARAMETERS == yad3RealtorAddApartment(yad_3,
		"Re@@@@@ltor", "RealService", 12345, 7, 5, 65000, matrix));
	ASSERT_TEST(YAD3_INVALID_PARAMETERS == yad3RealtorAddApartment(yad_3,
		"Re@ltor", "RealService", -1, 7, 5, 65000, matrix));
	ASSERT_TEST(YAD3_INVALID_PARAMETERS == yad3RealtorAddApartment(yad_3,
		"Re@ltor", "RealService", 12345, -7, 5, 65000, matrix));
	ASSERT_TEST(YAD3_INVALID_PARAMETERS == yad3RealtorAddApartment(yad_3,
		"Re@ltor", "RealService", 12345, 7, -5, 65000, matrix));
	ASSERT_TEST(YAD3_INVALID_PARAMETERS == yad3RealtorAddApartment(yad_3,
		"Re@ltor", "RealService", 12345, 7, 5, -65000, matrix));
	ASSERT_TEST(YAD3_INVALID_PARAMETERS == yad3RealtorAddApartment(yad_3,
		"Re@ltor", "RealService", 12345, 0, 5, 65000, matrix));
	ASSERT_TEST(YAD3_INVALID_PARAMETERS == yad3RealtorAddApartment(yad_3,
		"Re@ltor", "RealService", 12345, 7, 0, 65000, matrix));
	ASSERT_TEST(YAD3_INVALID_PARAMETERS == yad3RealtorAddApartment(yad_3,
		"Re@ltor", "RealService", 12345, 7, 5, 0, matrix));
	ASSERT_TEST(YAD3_INVALID_PARAMETERS == yad3RealtorAddApartment(yad_3,
		"Re@ltor", "RealService", 12345, 7, 5, 65000, "ewewewewewewewewewew"));
	ASSERT_TEST(YAD3_INVALID_PARAMETERS == yad3RealtorAddApartment(yad_3,
		"Re@ltor", "RealService", 12345, 7, 5, 65000,
			"ewewewewewewewaaaaaaaaaaeewweewweew"));
	ASSERT_TEST(YAD3_INVALID_PARAMETERS == yad3RealtorAddApartment(yad_3,
		"Re@ltor", "RealService", 12345, 7, 5, 65033, matrix));
	ASSERT_TEST(YAD3_EMAIL_DOES_NOT_EXIST == yad3RealtorAddApartment(yad_3,
		"Walter@white", "RealService", 12345, 7, 5, 65000, matrix));
	yad3CustomerAdd(yad_3, "Jesse@pinkman", 65, 4, 671000);
	ASSERT_TEST(YAD3_EMAIL_WRONG_ACCOUNT_TYPE == yad3RealtorAddApartment(yad_3,
		"Jesse@pinkman", "RealService", 12345, 7, 5, 65000, matrix));
	ASSERT_TEST(YAD3_APARTMENT_SERVICE_DOES_NOT_EXIST ==
		yad3RealtorAddApartment(yad_3, "Re@ltor", "No such service",
			12345, 7, 5, 65000, matrix));
	ASSERT_TEST(YAD3_SUCCESS == yad3RealtorAddApartment(yad_3, "Re@ltor",
		"RealService", 12345, 7, 5, 65000, matrix));
	ASSERT_TEST(YAD3_APARTMENT_ALREADY_EXISTS == yad3RealtorAddApartment(yad_3,
		"Re@ltor", "RealService", 12345, 7, 5, 65000, matrix));
	ASSERT_TEST(YAD3_SUCCESS == yad3RealtorAddApartment(yad_3, "Re@ltor",
		"RealService", 23456, 7, 5, 65000, matrix));
	ASSERT_TEST(YAD3_APARTMENT_SERVICE_FULL == yad3RealtorAddApartment(yad_3,
		"Re@ltor", "RealService", 34567, 7, 5, 65000, matrix));
//Remove:
	ASSERT_TEST(YAD3_NULL_ARG ==
		yad3RealtorRemoveApartment(NULL,"Re@ltor","RealService",12345));
	ASSERT_TEST(YAD3_NULL_ARG ==
		yad3RealtorRemoveApartment(yad_3,NULL,"RealService",12345));
	ASSERT_TEST(YAD3_NULL_ARG ==
		yad3RealtorRemoveApartment(yad_3,"Re@ltor",NULL,12345));
	ASSERT_TEST(YAD3_INVALID_PARAMETERS ==
		yad3RealtorRemoveApartment(yad_3, "Re@ltor", "RealService", -1));
	ASSERT_TEST(YAD3_INVALID_PARAMETERS ==
		yad3RealtorRemoveApartment(yad_3, "Realtor", "RealService", 12345));
	ASSERT_TEST(YAD3_EMAIL_DOES_NOT_EXIST ==
		yad3RealtorRemoveApartment(yad_3, "Walter@white","RealService", 12345));
	ASSERT_TEST(YAD3_EMAIL_WRONG_ACCOUNT_TYPE ==
		yad3RealtorRemoveApartment(yad_3,"Jesse@pinkman", "RealService",12345));
	ASSERT_TEST(YAD3_APARTMENT_SERVICE_DOES_NOT_EXIST ==
		yad3RealtorRemoveApartment(yad_3, "Re@ltor", "No such service", 12345));
	ASSERT_TEST(YAD3_APARTMENT_DOES_NOT_EXIST ==
		yad3RealtorRemoveApartment(yad_3, "Re@ltor", "RealService", 1000));
	ASSERT_TEST(YAD3_SUCCESS ==
		yad3RealtorRemoveApartment(yad_3, "Re@ltor", "RealService", 12345));
	yad3ServiceDestroy(yad_3);
	return true;
}

bool yad3_customer_add_remove_test() {
	Yad3Service yad_3 = yad3ServiceCreate();
	ASSERT_TEST(YAD3_NULL_ARG == yad3CustomerAdd(NULL, "no@idea.ca", 4,7,5000));
	ASSERT_TEST(YAD3_NULL_ARG == yad3CustomerAdd(yad_3, NULL, 4,7,5000));
	ASSERT_TEST(YAD3_INVALID_PARAMETERS ==
		yad3CustomerAdd(yad_3, "no@idea.ca", 0, 7, 5000));
	ASSERT_TEST(YAD3_INVALID_PARAMETERS ==
		yad3CustomerAdd(yad_3, "no@idea.ca", 4, 0, 5000));
	ASSERT_TEST(YAD3_INVALID_PARAMETERS ==
		yad3CustomerAdd(yad_3, "no@idea.ca", 4, 7, 0));
	ASSERT_TEST(YAD3_INVALID_PARAMETERS ==
		yad3CustomerAdd(yad_3, "no@idea.ca", -4, 7, 5000));
	ASSERT_TEST(YAD3_INVALID_PARAMETERS ==
		yad3CustomerAdd(yad_3, "no@idea.ca", 4, -7, 5000));
	ASSERT_TEST(YAD3_INVALID_PARAMETERS ==
		yad3CustomerAdd(yad_3, "no@idea.ca", 4, 7, -5000));
	ASSERT_TEST(YAD3_INVALID_PARAMETERS ==
		yad3CustomerAdd(yad_3, "no-idea.ca", 4, 7, 5000));
	ASSERT_TEST(YAD3_SUCCESS ==
		yad3CustomerAdd(yad_3, "no@idea.ca", 4, 7, 5000));
	ASSERT_TEST(YAD3_EMAIL_ALREADY_EXISTS ==
		yad3CustomerAdd(yad_3, "no@idea.ca", 11, 2, 23500));
	//now adding 1 realtor, then a customer with same mail
	yad3RealtorAdd(yad_3, "realtor@mail.com", "WTF inc.", 25);
	ASSERT_TEST(YAD3_EMAIL_ALREADY_EXISTS ==
		yad3CustomerAdd(yad_3, "realtor@mail.com", 1, 4, 2500));
	ASSERT_TEST(YAD3_SUCCESS ==
		yad3CustomerAdd(yad_3, "no@idea.xxx", 25, 4, 10000));
	ASSERT_TEST(YAD3_NULL_ARG == yad3CustomerRemove(NULL, "some@user"));
	ASSERT_TEST(YAD3_NULL_ARG == yad3CustomerRemove(yad_3, NULL));
	ASSERT_TEST(YAD3_INVALID_PARAMETERS == yad3CustomerRemove(yad_3, ""));
	ASSERT_TEST(YAD3_EMAIL_WRONG_ACCOUNT_TYPE ==
		yad3CustomerRemove(yad_3, "realtor@mail.com"));
	ASSERT_TEST(YAD3_EMAIL_DOES_NOT_EXIST ==
		yad3CustomerRemove(yad_3, "some_other@user"));
	ASSERT_TEST(YAD3_SUCCESS == yad3CustomerRemove(yad_3, "no@idea.xxx"));
	ASSERT_TEST(YAD3_SUCCESS == yad3CustomerRemove(yad_3, "no@idea.ca"));
	yad3ServiceDestroy(yad_3);
	return true;
}

bool yad3_make_offer_test() {
	Yad3Service yad_3 = yad3ServiceCreate();
	addRealtorCustomerAndApartments(yad_3);
	ASSERT_TEST(YAD3_NULL_ARG == yad3CustomerMakeOffer(NULL, "want@to.buy",
		"want@to.sell", "Pollos Hermanos",1,1000));
	ASSERT_TEST(YAD3_NULL_ARG == yad3CustomerMakeOffer(yad_3, NULL,
		"want@to.sell", "Pollos Hermanos",1,1000));
	ASSERT_TEST(YAD3_NULL_ARG == yad3CustomerMakeOffer(yad_3, "want@to.buy",
		NULL, "Pollos Hermanos",1,1000));
	ASSERT_TEST(YAD3_INVALID_PARAMETERS == yad3CustomerMakeOffer(yad_3,
		"wrongEmail", "want@to.sell", "Pollos Hermanos", 1, 1000));
	ASSERT_TEST(YAD3_INVALID_PARAMETERS == yad3CustomerMakeOffer(yad_3,
		"want@to.buy", "wrongEmail", "Pollos Hermanos", 1, 1000));
	ASSERT_TEST(YAD3_INVALID_PARAMETERS == yad3CustomerMakeOffer(yad_3,
		"want@to.buy", "want@to.sell", "Pollos Hermanos", -1, 1000));
	ASSERT_TEST(YAD3_INVALID_PARAMETERS == yad3CustomerMakeOffer(yad_3,
		"want@to.buy", "want@to.sell", "Pollos Hermanos", 1, -1000));
	ASSERT_TEST(YAD3_INVALID_PARAMETERS == yad3CustomerMakeOffer(yad_3,
		"want@to.buy", "want@to.sell", "Pollos Hermanos", 1, 0));
	ASSERT_TEST(YAD3_EMAIL_DOES_NOT_EXIST == yad3CustomerMakeOffer(yad_3,
		"want@to.sleep", "want@to.sell", "Pollos Hermanos", 1, 1000));
	ASSERT_TEST(YAD3_EMAIL_DOES_NOT_EXIST == yad3CustomerMakeOffer(yad_3,
		"want@to.buy", "want@to.smoke", "Pollos Hermanos", 1, 1000));
	ASSERT_TEST(YAD3_EMAIL_WRONG_ACCOUNT_TYPE == yad3CustomerMakeOffer(yad_3,
		"want@to.sell", "want@to.sell", "Pollos Hermanos", 1, 1000));
	ASSERT_TEST(YAD3_EMAIL_WRONG_ACCOUNT_TYPE == yad3CustomerMakeOffer(yad_3,
		"want@to.buy", "want@to.buy", "Pollos Hermanos", 1, 1000));
	ASSERT_TEST(YAD3_EMAIL_WRONG_ACCOUNT_TYPE == yad3CustomerMakeOffer(yad_3,
		"want@to.buy", "want@to.buy", "Pollos Hermanos", 1, 1000));
	ASSERT_TEST(YAD3_APARTMENT_SERVICE_DOES_NOT_EXIST ==
		yad3CustomerMakeOffer(yad_3, "want@to.buy", "want@to.sell",
			"Grey Matter", 1, 1000));
	ASSERT_TEST(YAD3_APARTMENT_DOES_NOT_EXIST == yad3CustomerMakeOffer(yad_3,
		"want@to.buy", "want@to.sell", "Pollos Hermanos", 1234, 1000));
	//less area
	ASSERT_TEST(YAD3_REQUEST_WRONG_PROPERTIES == yad3CustomerMakeOffer(yad_3,
		"want@to.buy", "want@to.sell", "Pollos Hermanos", 666, 1000));
	//less rooms
	ASSERT_TEST(YAD3_REQUEST_WRONG_PROPERTIES == yad3CustomerMakeOffer(yad_3,
		"want@to.buy", "want@to.sell", "Pollos Hermanos", 999, 1000));
	//price too big
	ASSERT_TEST(YAD3_REQUEST_WRONG_PROPERTIES == yad3CustomerMakeOffer(yad_3,
		"want@to.buy", "want@to.sell", "Pollos Hermanos", 1, 10000));
	ASSERT_TEST(YAD3_REQUEST_ILLOGICAL_PRICE == yad3CustomerMakeOffer(yad_3,
		"want@to.buy", "want@to.sell", "Pollos Hermanos", 222, 1000));
	ASSERT_TEST(YAD3_SUCCESS == yad3CustomerMakeOffer(yad_3, "want@to.buy",
		"want@to.sell", "Pollos Hermanos", 1, 1000));
	ASSERT_TEST(YAD3_ALREADY_REQUESTED == yad3CustomerMakeOffer(yad_3,
		"want@to.buy", "want@to.sell", "Pollos Hermanos", 1, 1000));
	yad3ServiceDestroy(yad_3);
	return true;
}

bool yad3_purchase_test() {
	Yad3Service yad_3 = yad3ServiceCreate();
	addRealtorCustomerAndApartments(yad_3);
	char matrix2[] = "eeewwweee";	//3*3, 2 rooms, area 6
	char matrix3[] = "eeeeeeeeeeeee";	//1*13, 1 room, area 13
	yad3RealtorAddApartment(yad_3, "want@to.sell", "Pollos Hermanos",
		100, 3, 3, 3000, matrix2);	//3*3, 2 rooms, area 6
	yad3RealtorAddApartment(yad_3, "want@to.sell", "Pollos Hermanos",
		200, 1, 13, 2000, matrix3);	//1*13, 1 room, area 13
	ASSERT_TEST(YAD3_NULL_ARG == yad3CustomerPurchase(NULL,
		"want@to.buy", "want@to.sell", "Pollos Hermanos", 222));
	ASSERT_TEST(YAD3_NULL_ARG == yad3CustomerPurchase(yad_3,
		NULL, "want@to.sell", "Pollos Hermanos", 222));
	ASSERT_TEST(YAD3_NULL_ARG == yad3CustomerPurchase(yad_3,
		"want@to.buy", NULL, "Pollos Hermanos", 222));
	ASSERT_TEST(YAD3_NULL_ARG == yad3CustomerPurchase(yad_3,
		"want@to.buy", "want@to.sell", NULL, 222));
	ASSERT_TEST(YAD3_INVALID_PARAMETERS == yad3CustomerPurchase(yad_3,
		"wrongEmail", "want@to.sell", "Pollos Hermanos", 222));
	ASSERT_TEST(YAD3_INVALID_PARAMETERS == yad3CustomerPurchase(yad_3,
		"want@to.buy", "wrongEmail", "Pollos Hermanos", 222));
	ASSERT_TEST(YAD3_EMAIL_DOES_NOT_EXIST == yad3CustomerPurchase(yad_3,
		"some_other@user", "want@to.sell", "Pollos Hermanos", 222));
	ASSERT_TEST(YAD3_EMAIL_DOES_NOT_EXIST == yad3CustomerPurchase(yad_3,
		"want@to.buy", "some_other@user", "Pollos Hermanos", 222));
	ASSERT_TEST(YAD3_EMAIL_WRONG_ACCOUNT_TYPE == yad3CustomerPurchase(yad_3,
		"want@to.buy", "want@to.buy", "Pollos Hermanos", 222));
	ASSERT_TEST(YAD3_EMAIL_WRONG_ACCOUNT_TYPE == yad3CustomerPurchase(yad_3,
		"want@to.sell", "want@to.sell", "Pollos Hermanos", 222));
	ASSERT_TEST(YAD3_APARTMENT_SERVICE_DOES_NOT_EXIST ==
		yad3CustomerPurchase(yad_3, "want@to.buy",
			"want@to.sell", "No service", 222));
	ASSERT_TEST(YAD3_APARTMENT_DOES_NOT_EXIST == yad3CustomerPurchase(yad_3,
		"want@to.buy", "want@to.sell", "Pollos Hermanos", 777));
	ASSERT_TEST(YAD3_PURCHASE_WRONG_PROPERTIES == yad3CustomerPurchase(yad_3,
		"want@to.buy", "want@to.sell", "Pollos Hermanos", 1));	//too expensive
	ASSERT_TEST(YAD3_PURCHASE_WRONG_PROPERTIES == yad3CustomerPurchase(yad_3,
		"want@to.buy", "want@to.sell", "Pollos Hermanos", 200));	//less rooms
	ASSERT_TEST(YAD3_PURCHASE_WRONG_PROPERTIES == yad3CustomerPurchase(yad_3,
		"want@to.buy", "want@to.sell", "Pollos Hermanos", 100));	//less area
	ASSERT_TEST(YAD3_SUCCESS == yad3CustomerPurchase(yad_3, "want@to.buy",
		"want@to.sell", "Pollos Hermanos", 222));
#ifndef NDEBUG
	Customer customer = DEBUG_getCustomerByEmail(yad_3, "want@to.buy");
	ASSERT_TEST(550 == customerGetMoneyPaid(customer));
#endif
	ASSERT_TEST(YAD3_APARTMENT_DOES_NOT_EXIST == yad3CustomerPurchase(yad_3,
		"want@to.buy", "want@to.sell", "Pollos Hermanos", 222));
	yad3ServiceDestroy(yad_3);
	return true;
}

bool yad3_respond_to_offer_test() {
	Yad3Service yad_3 = yad3ServiceCreate();
	addRealtorCustomerAndApartments(yad_3);
	yad3CustomerMakeOffer(yad_3, "want@to.buy", "want@to.sell",
		"Pollos Hermanos", 1, 1000);
	ASSERT_TEST(YAD3_NULL_ARG == yad3RealtorRespondToOffer(NULL,
		"want@to.buy", "want@to.sell", DECLINE));
	ASSERT_TEST(YAD3_NULL_ARG == yad3RealtorRespondToOffer(yad_3,
		NULL, "want@to.sell", DECLINE));
	ASSERT_TEST(YAD3_NULL_ARG == yad3RealtorRespondToOffer(yad_3,
		"want@to.buy", NULL, DECLINE));
	ASSERT_TEST(YAD3_INVALID_PARAMETERS == yad3RealtorRespondToOffer(yad_3,
		"wrongEmail", "want@to.sell", DECLINE));
	ASSERT_TEST(YAD3_INVALID_PARAMETERS == yad3RealtorRespondToOffer(yad_3,
		"want@to.buy", "wrongEmail", DECLINE));
	ASSERT_TEST(YAD3_EMAIL_DOES_NOT_EXIST == yad3RealtorRespondToOffer(yad_3,
		"some_other@user", "want@to.sell", DECLINE));
	ASSERT_TEST(YAD3_EMAIL_DOES_NOT_EXIST == yad3RealtorRespondToOffer(yad_3,
		"want@to.buy", "some_other@user", DECLINE));
	ASSERT_TEST(YAD3_EMAIL_WRONG_ACCOUNT_TYPE ==
		yad3RealtorRespondToOffer(yad_3, "want@to.buy", "want@to.buy",DECLINE));
	ASSERT_TEST(YAD3_EMAIL_WRONG_ACCOUNT_TYPE ==
		yad3RealtorRespondToOffer(yad_3,"want@to.sell","want@to.sell",DECLINE));
	ASSERT_TEST(YAD3_SUCCESS == yad3RealtorRespondToOffer(yad_3,
		"want@to.buy", "want@to.sell", DECLINE));
#ifndef NDEBUG
	Customer customer = DEBUG_getCustomerByEmail(yad_3, "want@to.buy");
	ASSERT_TEST(0 == customerGetMoneyPaid(customer));
#endif
	ASSERT_TEST(YAD3_NOT_REQUESTED == yad3RealtorRespondToOffer(yad_3,
		"want@to.buy", "want@to.sell", DECLINE));
	yad3CustomerMakeOffer(yad_3, "want@to.buy", "want@to.sell",
		"Pollos Hermanos", 1, 1000);
	ASSERT_TEST(YAD3_SUCCESS == yad3RealtorRespondToOffer(yad_3,
		"want@to.buy", "want@to.sell", ACCEPT));
#ifndef NDEBUG
	customer = DEBUG_getCustomerByEmail(yad_3, "want@to.buy");
	ASSERT_TEST(1000 == customerGetMoneyPaid(customer));
#endif
	yad3ServiceDestroy(yad_3);
	return true;
}

bool yad3_report_relevant_realtors_test() {
	Yad3Service yad_3 = yad3ServiceCreate();
	yad3RealtorAdd(yad_3, "realtor@1", "Vamonos pest", 17);
	yad3RealtorAdd(yad_3, "realtor@2", "Vamonos pest", 80);
	yad3RealtorAddApartmentService(yad_3, "realtor@1", "service 1", 99);
	yad3RealtorAddApartment(yad_3, "realtor@1", "service 1",
		1001, 2, 3, 100, "eweewe"); //area 4
	yad3RealtorAddApartment(yad_3, "realtor@1", "service 1",
		1002, 3, 4, 700, "eeweeeweeewe"); //area 9
	yad3RealtorAddApartment(yad_3, "realtor@1", "service 1",
		1003, 4, 5, 5300, "eeweeeeweewwweeeeeee"); //area 15
	yad3RealtorAddApartment(yad_3, "realtor@1", "service 1",
		1004, 2, 2, 8900, "eeee"); //area 4
	//Realtor 2
	yad3RealtorAddApartmentService(yad_3, "realtor@2", "service 2", 99);
	yad3RealtorAddApartment(yad_3, "realtor@2", "service 2",
		1001, 3, 4, 100, "eeweeeweeewe"); //area 9
	yad3RealtorAddApartment(yad_3, "realtor@2", "service 2", 1002, 8, 9, 900,
	"eeeeweeeeeeeweweeeeeewwweeeeeeweweeeeeeweweeeewwwewwweweeeweeeweeeeeeeee");
	//area 53
	yad3RealtorAddApartment(yad_3, "realtor@2", "service 2", 1003, 4, 5, 35000,
		"eeweeeeweewwweeeeeee"); //area 15
	yad3CustomerAdd(yad_3, "big@area", 53, 1, 100000);
	yad3CustomerAdd(yad_3, "many@rooms", 1, 4, 100000);
	yad3CustomerAdd(yad_3, "low@price", 1, 1, 150);
	yad3CustomerAdd(yad_3, "everything@now", 100, 10, 50);
	yad3CustomerAdd(yad_3, "rich@and.modest", 1, 1, 1000000);
	ASSERT_TEST(YAD3_NULL_ARG ==
		yad3ReportRelevantRealtors(NULL, "big@area", stdout));
	ASSERT_TEST(YAD3_NULL_ARG ==
		yad3ReportRelevantRealtors(yad_3, NULL, stdout));
	ASSERT_TEST(YAD3_INVALID_PARAMETERS ==
		yad3ReportRelevantRealtors(yad_3, "WrongEmail", stdout));
	ASSERT_TEST(YAD3_EMAIL_DOES_NOT_EXIST ==
		yad3ReportRelevantRealtors(yad_3, "no@such.email", stdout));
	ASSERT_TEST(YAD3_EMAIL_WRONG_ACCOUNT_TYPE ==
		yad3ReportRelevantRealtors(yad_3, "realtor@1", stdout));
#ifndef NDEBUG
	//After debugging print functions can be checked with yad3 textual interface
	printf("\n");
	ASSERT_TEST(YAD3_SUCCESS ==
		yad3ReportRelevantRealtors(yad_3, "big@area", stdout)); //realtor 2
	printf("\n");
	ASSERT_TEST(YAD3_SUCCESS ==
		yad3ReportRelevantRealtors(yad_3, "many@rooms", stdout)); //realtor 2
	printf("\n");
	ASSERT_TEST(YAD3_SUCCESS ==
		yad3ReportRelevantRealtors(yad_3, "low@price", stdout)); //realtor 1
	printf("\n");
	ASSERT_TEST(YAD3_SUCCESS ==
		yad3ReportRelevantRealtors(yad_3, "everything@now", stdout)); //none
	printf("\n");
	ASSERT_TEST(YAD3_SUCCESS ==
		yad3ReportRelevantRealtors(yad_3, "rich@and.modest", stdout));
		//realtor 1 and 2
	printf("\n");
#endif
	yad3ServiceDestroy(yad_3);
	return true;
}

bool yad3_report_most_paying_customers_test() { //TODO finish
	Yad3Service yad_3 = yad3ServiceCreate();
	yad3CustomerAdd(yad_3, "paid@990", 1, 1, 1000);
	yad3CustomerAdd(yad_3, "paid@8800", 1, 1, 10000);
	yad3CustomerAdd(yad_3, "paid@a.220000", 1, 1, 500000);
	yad3CustomerAdd(yad_3, "paid@b.220000", 1, 1, 300000);
	yad3CustomerAdd(yad_3, "paid@c.220000", 1, 1, 1000000);
	yad3CustomerAdd(yad_3, "paid@385000", 1, 1, 1000000);
	yad3CustomerAdd(yad_3, "paid@0", 1, 1, 1000000);
	yad3RealtorAdd(yad_3,"re@ltor","company",10);
	yad3RealtorAddApartmentService(yad_3,"re@ltor","service",6);
	yad3RealtorAddApartment(yad_3,"re@ltor","service",1,1,1,900,"e");
	yad3RealtorAddApartment(yad_3,"re@ltor","service",2,1,1,8000,"e");
	yad3RealtorAddApartment(yad_3,"re@ltor","service",3,1,1,200000,"e");
	yad3RealtorAddApartment(yad_3,"re@ltor","service",4,1,1,200000,"e");
	yad3RealtorAddApartment(yad_3,"re@ltor","service",5,1,1,200000,"e");
	yad3RealtorAddApartment(yad_3,"re@ltor","service",6,1,1,350000,"e");
	ASSERT_TEST(YAD3_SUCCESS ==
		yad3CustomerPurchase(yad_3,"paid@8800","re@ltor","service",2));
	ASSERT_TEST(YAD3_SUCCESS ==
		yad3CustomerPurchase(yad_3,"paid@990","re@ltor","service",1));
	ASSERT_TEST(YAD3_SUCCESS ==
		yad3CustomerPurchase(yad_3,"paid@a.220000","re@ltor","service",4));
	ASSERT_TEST(YAD3_SUCCESS ==
		yad3CustomerPurchase(yad_3,"paid@b.220000","re@ltor","service",3));
	ASSERT_TEST(YAD3_SUCCESS ==
		yad3CustomerPurchase(yad_3,"paid@c.220000","re@ltor","service",5));
	ASSERT_TEST(YAD3_SUCCESS ==
			yad3CustomerPurchase(yad_3,"paid@385000","re@ltor","service",6));
	ASSERT_TEST(YAD3_NULL_ARG==yad3ReportMostPayingCustomers(NULL, 3, stdout));
	ASSERT_TEST(YAD3_INVALID_PARAMETERS ==
		yad3ReportMostPayingCustomers(yad_3, -4, stdout));
	ASSERT_TEST(YAD3_INVALID_PARAMETERS ==
		yad3ReportMostPayingCustomers(yad_3, 0, stdout));
	//After debugging print functions can be checked with yad3 textual interface
	printf("\n");
	ASSERT_TEST(YAD3_SUCCESS==yad3ReportMostPayingCustomers(yad_3, 3, stdout));
	printf("\n");
	ASSERT_TEST(YAD3_SUCCESS==yad3ReportMostPayingCustomers(yad_3, 8,stdout));
	printf("\n");
	yad3ServiceDestroy(yad_3);
	return true;
}

bool yad3_report_significant_realtors_test() {
	Yad3Service yad_3 = yad3ServiceCreate();
//Realtor 1
	yad3RealtorAdd(yad_3, "TywinL@nnister", "Casterly Rock", 17);
	yad3RealtorAddApartmentService(yad_3, "TywinL@nnister", "service 1", 99);
	yad3RealtorAddApartment(yad_3, "TywinL@nnister", "service 1",
		1001, 2, 3, 100, "eweewe"); //area 4
	yad3RealtorAddApartment(yad_3, "TywinL@nnister", "service 1",
		1002, 3, 4, 700, "eeweeeweeewe"); //area 9
	yad3RealtorAddApartment(yad_3, "TywinL@nnister", "service 1",
		1003, 4, 5, 5300, "eeweeeeweewwweeeeeee"); //area 15
	yad3RealtorAddApartment(yad_3, "TywinL@nnister", "service 1",
		1004, 2, 2, 8900, "eeee"); //area 4
//Realtor 2
	yad3RealtorAdd(yad_3, "D@enerys", "Meeryn", 80);
	yad3RealtorAddApartmentService(yad_3, "D@enerys", "service 2", 99);
	yad3RealtorAddApartment(yad_3, "D@enerys", "service 2", 1001, 3, 4, 100,
		"eeweeeweeewe"); //area 9
	yad3RealtorAddApartment(yad_3, "D@enerys", "service 2", 1002, 8, 9, 900,
	"eeeeweeeeeeeweweeeeeewwweeeeeeweweeeeeeweweeeewwwewwweweeeweeeweeeeeeeee");
	//area 53
	yad3RealtorAddApartment(yad_3, "D@enerys", "service 2", 1003, 4, 5, 35000,
		"eeweeeeweewwweeeeeee"); //area 15
//Others
	yad3RealtorAdd(yad_3, "John@Wall", "Night's watch", 80);
	yad3RealtorAddApartmentService(yad_3, "John@Wall", "service 2", 99);
	yad3RealtorAddApartment(yad_3, "John@Wall", "service 2", 1001, 3, 4, 100,
		"eeweeeweeewe"); //area 9

	yad3RealtorAdd(yad_3, "Stannis@grave", "Dragonstone", 80);
	yad3RealtorAddApartmentService(yad_3, "Stannis@grave", "service 2", 99);
	yad3RealtorAddApartment(yad_3, "Stannis@grave", "service 2", 1001, 3, 4,
		100, "eeweeeweeewe"); //area 9

	yad3RealtorAdd(yad_3, "Arya@Braavos", "Nobody", 80);
	yad3RealtorAddApartmentService(yad_3, "Arya@Braavos", "service 2", 99);
	yad3RealtorAddApartment(yad_3, "Arya@Braavos", "service 2", 1001, 3, 4, 100,
		"eeweeeweeewe"); //area 9
	ASSERT_TEST(YAD3_NULL_ARG==yad3ReportSignificantRealtors(NULL, 4, stdout));
	ASSERT_TEST(YAD3_INVALID_PARAMETERS ==
		yad3ReportSignificantRealtors(yad_3, -4, stdout));
	ASSERT_TEST(YAD3_INVALID_PARAMETERS ==
		yad3ReportSignificantRealtors(yad_3, 0, stdout));
#ifndef NDEBUG
	//After debugging print functions can be checked with yad3 textual interface
	printf("\n");
	ASSERT_TEST(YAD3_SUCCESS==yad3ReportSignificantRealtors(yad_3, 4, stdout));
	printf("\n");
	ASSERT_TEST(YAD3_SUCCESS==yad3ReportSignificantRealtors(yad_3, 8, stdout));
	printf("\n");
#endif
	yad3ServiceDestroy(yad_3);
	return true;
}

bool check_offers_are_deleted_combo_test() {
	Yad3Service yad_3 = yad3ServiceCreate();
	//Realtor 1
	yad3RealtorAdd(yad_3, "TywinL@nnister", "Casterly Rock", 17);
	yad3RealtorAddApartmentService(yad_3, "TywinL@nnister", "service 1", 99);
	yad3RealtorAddApartment(yad_3, "TywinL@nnister", "service 1",
		1001, 2, 3, 100, "eweewe"); //area 4
	yad3RealtorAddApartment(yad_3, "TywinL@nnister", "service 1",
		1002, 3, 4, 700, "eeweeeweeewe"); //area 9
	yad3RealtorAddApartment(yad_3, "TywinL@nnister", "service 1",
		1003, 4, 5, 5300, "eeweeeeweewwweeeeeee"); //area 15
	yad3RealtorAddApartment(yad_3, "TywinL@nnister", "service 1",
		1004, 2, 2, 8900, "eeee"); //area 4
	//Realtor 2
	yad3RealtorAdd(yad_3, "D@enerys", "Meeryn", 80);
	yad3RealtorAddApartmentService(yad_3, "D@enerys", "service 2", 99);
	yad3RealtorAddApartment(yad_3, "D@enerys", "service 2", 1001, 3, 4, 100,
		"eeweeeweeewe"); //area 9
	yad3RealtorAddApartment(yad_3, "D@enerys", "service 2", 1002, 8, 9, 900,
	"eeeeweeeeeeeweweeeeeewwweeeeeeweweeeeeeweweeeewwwewwweweeeweeeweeeeeeeee");
	yad3RealtorAddApartment(yad_3, "D@enerys", "service 2", 1003, 4, 5, 35000,
		"eeweeeeweewwweeeeeee"); //area 15
	//Customer
	yad3CustomerAdd(yad_3, "Petyr@Baelish", 1, 1, 1000000);
	yad3CustomerAdd(yad_3, "Eddard@Stark", 1, 1, 1000000);
	//remove customer
	ASSERT_TEST(YAD3_SUCCESS == yad3CustomerMakeOffer(yad_3, "Eddard@Stark",
		"TywinL@nnister", "service 1", 1001, 50));
	yad3CustomerRemove(yad_3, "Eddard@Stark");
	yad3CustomerAdd(yad_3, "Eddard@Stark", 1, 1, 1000000); //so email will exist
	ASSERT_TEST(YAD3_NOT_REQUESTED == yad3RealtorRespondToOffer(yad_3,
		"Eddard@Stark", "TywinL@nnister", ACCEPT));
	//remove apartment
	ASSERT_TEST(YAD3_SUCCESS == yad3CustomerMakeOffer(yad_3, "Petyr@Baelish",
		"D@enerys", "service 2", 1003, 25000));
	yad3RealtorRemoveApartment(yad_3, "D@enerys", "service 2", 1003);
	yad3RealtorAddApartment(yad_3, "D@enerys", "service 2", 1003, 4, 5, 35000,
		"eeweeeeweewwweeeeeee"); //area 15
	ASSERT_TEST(YAD3_NOT_REQUESTED == yad3RealtorRespondToOffer(yad_3,
		"Petyr@Baelish", "D@enerys", DECLINE));
	//remove service
	ASSERT_TEST(YAD3_SUCCESS == yad3CustomerMakeOffer(yad_3, "Petyr@Baelish",
		"D@enerys", "service 2", 1003, 20000));
	yad3RealtorRemoveApartmentService(yad_3, "D@enerys", "service 2");
	yad3RealtorAddApartmentService(yad_3, "D@enerys", "service 2", 99);
	yad3RealtorAddApartment(yad_3, "D@enerys", "service 2", 1003, 4, 5, 35000,
		"eeweeeeweewwweeeeeee"); //area 15
	ASSERT_TEST(YAD3_NOT_REQUESTED == yad3RealtorRespondToOffer(yad_3,
		"Petyr@Baelish", "D@enerys", DECLINE));
	//remove realtor
	ASSERT_TEST(YAD3_SUCCESS == yad3CustomerMakeOffer(yad_3, "Petyr@Baelish",
		"TywinL@nnister", "service 1", 1004, 4000));
	yad3RealtorRemove(yad_3, "TywinL@nnister");
	yad3RealtorAdd(yad_3, "TywinL@nnister", "Casterly Rock", 17);
	yad3RealtorAddApartmentService(yad_3, "TywinL@nnister", "service 1", 99);
	yad3RealtorAddApartment(yad_3, "TywinL@nnister", "service 1",
		1004, 2, 2, 8900, "eeee"); //area 4
	ASSERT_TEST(YAD3_NOT_REQUESTED == yad3RealtorRespondToOffer(yad_3,
		"Petyr@Baelish", "TywinL@nnister", DECLINE));
	//regular purchase
	ASSERT_TEST(YAD3_SUCCESS == yad3CustomerMakeOffer(yad_3, "Petyr@Baelish",
		"D@enerys", "service 2", 1003, 20000));
	yad3CustomerPurchase(yad_3, "Petyr@Baelish", "D@enerys", "service 2", 1003);
	yad3RealtorAddApartment(yad_3, "D@enerys", "service 2", 1003, 4, 5, 35000,
		"eeweeeeweewwweeeeeee"); //area 15
	ASSERT_TEST(YAD3_NOT_REQUESTED == yad3RealtorRespondToOffer(yad_3,
		"Petyr@Baelish", "D@enerys", DECLINE));
	//offer accepted
	ASSERT_TEST(YAD3_SUCCESS == yad3CustomerMakeOffer(yad_3, "Petyr@Baelish",
		"TywinL@nnister", "service 1", 1004, 4000));
	ASSERT_TEST(YAD3_SUCCESS == yad3RealtorRespondToOffer(yad_3,
		"Petyr@Baelish", "TywinL@nnister", ACCEPT));
	yad3RealtorAddApartment(yad_3, "TywinL@nnister", "service 1",
		1004, 2, 2, 8900, "eeee"); //area 4
	ASSERT_TEST(YAD3_NOT_REQUESTED == yad3RealtorRespondToOffer(yad_3,
		"Petyr@Baelish", "TywinL@nnister", ACCEPT));
	yad3ServiceDestroy(yad_3);
	return true;
}

int main() {
	RUN_TEST(yad3_create_destroy_test);
	RUN_TEST(yad3_realtor_add_remove_test);
	RUN_TEST(yad3_realtor_add_remove_service);
	RUN_TEST(yad3_realtor_add_remove_apartment);
	RUN_TEST(yad3_customer_add_remove_test);
	RUN_TEST(yad3_make_offer_test);
	RUN_TEST(yad3_purchase_test);
	RUN_TEST(yad3_respond_to_offer_test);
	RUN_TEST(yad3_report_relevant_realtors_test);
	RUN_TEST(yad3_report_most_paying_customers_test);
	RUN_TEST(yad3_report_significant_realtors_test);
	RUN_TEST(check_offers_are_deleted_combo_test);
	return 0;
}
