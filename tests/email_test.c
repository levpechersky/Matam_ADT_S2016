/*
 * email_test.c
 *
 *  Created on: May 8, 2016
 *
 */
#include <stdlib.h>
#include "../email.h"
#include "test_utilities.h"

bool email_is_valid_test() {
	ASSERT_TEST(false == emailIsValid("really.bad-mail"));
	ASSERT_TEST(true == emailIsValid("my@mail.is.ok"));
	ASSERT_TEST(false == emailIsValid("my@mail@is@not@good"));
	ASSERT_TEST(true == emailIsValid("@"));
	ASSERT_TEST(false == emailIsValid(""));
	ASSERT_TEST(false == emailIsValid(NULL));
	return true;
}

bool email_compare_test() {
	ASSERT_TEST(0 < emailCompare("really.bad-mail", "another_mail"));
	ASSERT_TEST(0 == emailCompare("bad@mail", "bad@mail"));
	ASSERT_TEST(0 > emailCompare("asd@asr", "cdad@fdg"));
	return true;
}

bool email_copy_destroy_test() {
	Email tmp = emailCopy("my@mail.is.ok");
	ASSERT_TEST(0 == emailCompare("my@mail.is.ok", tmp));
	emailDestroy(tmp);
	tmp = emailCopy(NULL);
	ASSERT_TEST(NULL == tmp);
	return true;
}

int main() {
	RUN_TEST(email_is_valid_test);
	RUN_TEST(email_compare_test);
	RUN_TEST(email_copy_destroy_test);
	return 0;
}
