/*
 * email.h
 *
 *  Created on: May 8, 2016
 *
 */

#ifndef EMAIL_H_
#define EMAIL_H_

#include <stdbool.h>

typedef char* Email;

/*
 * @Return:
 * false if email is NULL or contains more or less than one "@"
 * true if email contains exactly one "@"
 */
bool emailIsValid(const Email email);

/*
 * IMPORTANT: check emails for NULL before calling this function
 * @Return:
 * 0 if both emails are equal
 * positive value if email_1 is "greater" than email_2
 * negative otherwise */
int emailCompare(const Email email_1, const Email email_2);

/* Allocates memory and copies source email
 * @Return:
 * NULL if source is NULL or in case of memory allocation error
 * pointer to new allocated copy otherwise
 * */
const Email emailCopy(const Email source);

/* Frees all allocated for email memory, can receive NULL */
void emailDestroy(Email email);


#endif /* EMAIL_H_ */
