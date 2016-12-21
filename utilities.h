/*
 * utilities.h
 *
 *  Created on: Apr 11, 2016
 *
 *  You can add here any macros or constants, defines,
 *  enums for public use
 */

#ifndef UTILITIES_H_
#define UTILITIES_H_

#include <stdlib.h>
#include <stdbool.h>

#define NULL_PTR_CHECK(ptr, error)  if(!(ptr)){\
										return (error);\
									}

#define POSITIVE_CHECK(value, error)  if((value)<=0){\
										return (error);\
									}

#define NON_NEGATIVE_CHECK(value, error)  if((value)<0){\
										return (error);\
									}

/*compare 1 and 2, return 3*/
#define IF_EQUAL_RETURN(value_1, value_2, return_value)  \
		if((value_1)==(value_2)){\
			return (return_value);\
			}

/*compare 1 and 2, return 3*/
#define IF_NOT_EQUAL_RETURN(value_1, value_2, return_value)  \
		if((value_1)!=(value_2)){\
			return (return_value);\
			}

//For debug purposes
//Prints new line in a way "variable = value"
#define PRINTN(n) printf("\n%s = %d", #n, (n))

#define INT_ERROR -1


#endif /* UTILITIES_H_ */
