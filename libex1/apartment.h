#ifndef APARTMENT_H
#define APARTMENT_H

#include <stdbool.h>

typedef struct apartment_t* Apartment;

typedef enum { EMPTY, WALL } SquareType;

typedef enum {
	APARTMENT_SUCCESS, APARTMENT_NULL_ARG, APARTMENT_OUT_OF_MEM,
	APARTMENT_OUT_OF_BOUNDS, APARTMENT_NO_ROOM,
	APARTMENT_OLD_VALUE, APARTMENT_BAD_SPLIT,
	APARTMENT_PRICE_NOT_IN_RANGE
} ApartmentResult;

/**
* Creates a new apartment of type image:
* The structure is given by squares, and the dimensions are width/height.
* The price of the apartment is given by price.
*
* @return
* 	A new allocated apartment.
* 	if width or height are not positive, price is negative, squares is NULL
* 	or in case of a memory allocation failure - return NULL.
**/
Apartment apartmentCreate(SquareType** squares, int length, int width, 
						  int price);
/**
* Frees all memory allocated for the given apartment.
* This function can receive NULL.
*/
void apartmentDestroy(Apartment apartment);

/**
* creates a newly allocated copy of a given apartment.
*
* @return
* A newly allocated copy of the original apartment
* NULL if the apartment is NULL or in any case of memory allocation failure
*/
Apartment apartmentCopy(Apartment apartment);

/**
* checks whether two squares belong to the same room. 
* Two squares are considered to be in the same room if there exists a path 
* between them, which consists only of EMPTY squares.
* result should be stored in outResult
* row1, col1, row2 and col2 are the coordinates for the two squares.
*
* @return
* 	APARTMENT_NULL_ARG if apartment is NULL or outResult is NULL
* 	APARTMENT_OUT_OF_BOUNDS if one the coordinates is illegal.
*	APARTMENT_NO_ROOM if one if the requested squares is a wall.
* 	MESSAGE_SUCCESS otherwise
*
*/
ApartmentResult apartmentIsSameRoom(Apartment apartment, int row1, int col1,
									int row2, int col2, bool* outResult);

/**
* @return
* 	the number of empty sqaures in the apartment.
*	This function asserts (apartment != NULL).
*/
int apartmentTotalArea(Apartment apartment);

/**
* This function receives an apartment and coordinates for a sqyare in the 
* apartment, and calculates the number of squares in the same room with the
* given square. 
* Two squares are considered to be in the same room if there exists a path
* between them, which consists only of EMPTY squares.
* result should be stored in outArea
* row and colare the coordinates for the square.
*
* @return
* 	APARTMENT_NULL_ARG if apartment is NULL or outArea is NULL
* 	APARTMENT_OUT_OF_BOUNDS if the coordinates are illegal.
*	APARTMENT_NO_ROOM if one if the requested squares is a wall.
* 	MESSAGE_SUCCESS otherwise
*
*/
ApartmentResult apartmentRoomArea(Apartment apartment, int row, int col,
								   int* outArea);

/**
* This function receives an apartment and creates two newly allocated 
* apartments, which result from splitting the original aparmtnet.
* The split is done either horizontally or vertically.
* if splitByRow is true, the first apartment will consist of the rows with
* lower indices, and the second apartment will consists of the rows with
* greater indices.
* apartment, and calculates the number of squares in the same room with the
* given square.
* The new price will be calculated by:
* old_price * (#rows in new apartment + 1) / #rows in original apartment.
* Note that the sum of prices of the two new apartments is higher than the
* original price.
* if splitByRow is false, a similiar split is done w.r.t the columnd at the
* given index.
* If split is only allowed to be made if the splitting row or column consisits
* entirely of WALL squares.
* Also, the two new apartments must not be empty (i.e. both dimensions must be
* positive).
*
* @return
* 	APARTMENT_NULL_ARG if apartment, first or second are NULL.
* 	APARTMENT_OUT_OF_BOUNDS if the given row or colomn index exceeds the bounds
*	of the apartment. 
*	APARTMENT_BAD_SPLIT if the requested split will result with at least one
*	epmty apartment, or the splitting row or column contains an empty sqaure.
* 	MESSAGE_SUCCESS otherwise
*
*/
ApartmentResult apartmentSplit(Apartment apartment, bool splitByRow,
								int index, Apartment* first, 
								Apartment* second);

/**
* @return
* 	the number of different rooms in the apartment.
*	This function asserts (apartment != NULL).
*/
int apartmentNumOfRooms(Apartment apartment);

/**
* sets the value of a specific sqaure, given by a row and columns indices  (0-based)
* to the given value.
*
* @return
* 	APARTMENT_NULL_ARG if apartment is NULL
* 	APARTMENT_OUT_OF_BOUNDS if the row or column are greater than or equal to
*	the length and width (respectively)
*	APARTMENT_OLD_VALUE if the square already holds the requested value.
* 	APARTMENT_SUCCESS otherwise
*/
ApartmentResult apartmentSetSquare(Apartment apartment, int row, int col,
									SquareType value);

/**
* gets the value of a specific sqaure, given by a row and columns indices  (0-based)
* result should be stored in outValue
*
* @return
* 	APARTMENT_NULL_ARG if apartment is NULL
* 	APARTMENT_OUT_OF_BOUNDS if the row or column are greater than or equal to 
*	the length and width (respectively)
* 	APARTMENT_SUCCESS otherwise
*/
ApartmentResult apartmentGetSquare(Apartment apartment, int row, int col,
									SquareType* outValue);

/**
* This function modifies the apartment price based on the given percentage.
* If it is positive, the apartment price increases. If it is negative - it
* decreases.
* Positive values are unbounded, but negative must not be lower than -100.
*
* @return
* 	APARTMENT_NULL_ARG if apartment is NULL
* 	APARTMENT_PRICE_NOT_IN_RANGE if percent < -100.
* 	APARTMENT_SUCCESS otherwise
*/
ApartmentResult apartmentChangePrice(Apartment apartment, int percent);


/**
* @return
* 	the price of the apartment.
*	This function asserts (apartment != NULL).
*/
int apartmentGetPrice(Apartment apartment);

/**
* @return
* 	the length of the apartment.
*	This function asserts (apartment != NULL).
*/
int apartmentGetLength(Apartment apartment);

/**
* @return
* 	the width of the apartment.
*	This function asserts (apartment != NULL).
*/
int apartmentGetWidth(Apartment apartment);

/**
* Checks whether two apartments are identical.
* If both are NULL, this function returns True.
* If exactly one of them is NULL, it returns False.
* Otherwise, it returns True iff all the parameters are identical, i.e:
* The width is the same, the length is the same, the price is the same,
* And each two squares in the same positions are the same.
* @return
* 	A flag indicating whether the apartments are identical or not.
*/
bool apartmentIsIdentical(Apartment apartment1, Apartment apartment2);

#endif /* APARTMENT_H */
