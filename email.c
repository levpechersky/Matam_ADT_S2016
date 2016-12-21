/*
 * email.c
 *
 *  Created on: May 8, 2016
 *
 */

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "email.h"
#include "utilities.h"

bool emailIsValid(const Email email) {
	if (!email) {
		return false;
	}
	int i = 0, at_symbol_count = 0;
	while (email[i]) {
		at_symbol_count += (email[i++] == '@');
	}
	return (bool) (at_symbol_count == 1);
}

int emailCompare(const Email email_1, const Email email_2) {
	return strcmp(email_1, email_2);
}

const Email emailCopy(const Email source) {
	NULL_PTR_CHECK(source, NULL);
	Email destination = malloc(strlen(source) + 1);
	NULL_PTR_CHECK(destination, NULL);
	return strcpy((char*) destination, source);
}

void emailDestroy(Email email) {
	free(email);
}

