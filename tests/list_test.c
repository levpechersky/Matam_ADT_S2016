/*
 * list_test.c
 *
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "test_utilities.h"
#include "../list_mtm/list_mtm.h"

int longerThan(ListElement element1, ListElement element2) {
	if (strlen(element2) > strlen(element1)) {
		return 1;
	} else if (strlen(element2) < strlen(element1)) {
		return -1;
	}
	return 0;
}

ListElement copyString(ListElement str) {
	assert(str);
	char* copy = malloc(strlen(str) + 1);
	return copy ? strcpy(copy, str) : NULL;
}

void DestroyString(ListElement str) {
	free(str);
}

bool isLongerThan(ListElement string, ListFilterKey key) {
	return strlen(string) > strlen(key);
}

static bool testListCreate() {
	ASSERT_TEST(listCreate(NULL,NULL) == NULL);
	ASSERT_TEST(listCreate(NULL,NULL) == NULL);
	ASSERT_TEST(listCreate(copyString,NULL) == NULL);
	ASSERT_TEST(listCreate(NULL,DestroyString) == NULL);
	return true;
}

static bool testListCopy() {
	List list = listCreate(copyString, DestroyString);
	ASSERT_TEST(listCopy(NULL)==NULL);
	char *str = "yolo", *str2 = "mtm", *str3 = "b_y_e";
	listInsertFirst(list, str);
	listInsertFirst(list, str2);
	listInsertLast(list, str3);
	List new_list = listCopy(list);
	ASSERT_TEST(new_list);
	ASSERT_TEST(strcmp(listGetFirst(list), listGetFirst(new_list)) == 0);
	ASSERT_TEST(strcmp(listGetNext(list), listGetNext(new_list)) == 0);
	ASSERT_TEST(strcmp(listGetNext(list), listGetNext(new_list)) == 0);
	listDestroy(new_list);
	listDestroy(list);
	return true;
}

static bool testListGetSize() {
	List list = listCreate(copyString, DestroyString);
	ASSERT_TEST(listGetSize(NULL)==-1);
	ASSERT_TEST(listGetSize(list) == 0);
	char *str = "yolo", *str2 = "mtm", *str3 = "b_y_e";
	listInsertFirst(list, str);
	ASSERT_TEST(listGetSize(list) == 1);
	listInsertFirst(list, str2);
	ASSERT_TEST(listGetSize(list) == 2);
	listInsertFirst(list, str3);
	ASSERT_TEST(listGetSize(list) == 3);
	listDestroy(list);
	return true;
}

static bool testListGetFirst() {
	List list = listCreate(copyString, DestroyString);
	ASSERT_TEST(listGetFirst(list)==NULL);
	ASSERT_TEST(listGetFirst(NULL)==NULL);
	char *str = "hello", *str2 = "mm";
	listInsertFirst(list, str);
	ASSERT_TEST(strcmp(listGetFirst(list), str) == 0);
	listInsertFirst(list, str2);
	ASSERT_TEST(strcmp(listGetFirst(list), str2) == 0);
	ASSERT_TEST(strcmp(listGetFirst(list), str) != 0);
	listDestroy(list);

	return true;
}

static bool testListGetNext() {
	List list = listCreate(copyString, DestroyString);
	ASSERT_TEST(listGetNext(list)==NULL);
	char* str = "hello";
	listInsertFirst(list, str);
	listGetFirst(list);
	ASSERT_TEST(listGetNext(NULL)==NULL);
	ASSERT_TEST(listGetNext(list)==NULL);
	char* str2 = "matam";
	listInsertFirst(list, str2);
	listGetFirst(list);
	listDestroy(list);

	return true;
}

static bool testListInsertFirst() {
	List list = listCreate(copyString, DestroyString);
	char *str = "hello", *str2 = "bye";
	ASSERT_TEST(listInsertFirst(NULL,str)==LIST_NULL_ARGUMENT);
	ASSERT_TEST(listInsertFirst(list, str) == LIST_SUCCESS);
	ASSERT_TEST(listInsertFirst(list, str2) == LIST_SUCCESS);
	ASSERT_TEST(listInsertFirst(NULL,str2)==LIST_NULL_ARGUMENT);
	listDestroy(list);
	return true;
}

static bool testListInsertLast() {
	char *str = "hi", *str2 = "hey", *str3 = "ciao";
	List list = listCreate(copyString, DestroyString);
	ASSERT_TEST(listInsertLast(NULL,str)==LIST_NULL_ARGUMENT);
	ASSERT_TEST(listInsertLast(list, str) == LIST_SUCCESS);
	listInsertFirst(list, str);
	listInsertFirst(list, str2);
	ASSERT_TEST(listInsertLast(list, str3) == LIST_SUCCESS);
	listDestroy(list);
	return true;
}

static bool testListInsertBeforeCurrent() {
	char *str = "hi", *str2 = "hey", *str3 = "ciao";
	List list = listCreate(copyString, DestroyString);
	assert(list);
	ASSERT_TEST(listInsertBeforeCurrent(NULL,str2)==LIST_NULL_ARGUMENT);
	ASSERT_TEST(listInsertBeforeCurrent(list, str2) == LIST_INVALID_CURRENT);
	listInsertFirst(list, str);
	listGetFirst(list);
	ASSERT_TEST(listInsertBeforeCurrent(list, str) == LIST_SUCCESS);
	ASSERT_TEST(listInsertLast(list, str) == LIST_SUCCESS);
	listInsertFirst(list, str2);
	listInsertLast(list, str3);
	listGetFirst(list);
	listGetNext(list);
	ASSERT_TEST(listInsertBeforeCurrent(list, str3) == LIST_SUCCESS);
	listDestroy(list);
	return true;
}

static bool testListInsertAfterCurrent() {
	char *str = "hi", *str2 = "hey", *str3 = "ciao", *str4 = "bobo";
	List list = listCreate(copyString, DestroyString);
	assert(list);
	ASSERT_TEST(listInsertAfterCurrent(NULL,str2)==LIST_NULL_ARGUMENT);
	ASSERT_TEST(listInsertAfterCurrent(list, str2) == LIST_INVALID_CURRENT);
	listInsertFirst(list, str);
	listInsertLast(list, str2);
	listGetFirst(list);
	ASSERT_TEST(strcmp(listGetCurrent(list), str) == 0);
	ASSERT_TEST(listInsertAfterCurrent(list, str3) == LIST_SUCCESS);
	listGetNext(list);
	ASSERT_TEST(strcmp(listGetNext(list), str2) == 0);
	ASSERT_TEST(listInsertAfterCurrent(list, str) == LIST_SUCCESS);
	listInsertAfterCurrent(list, str3);
	ASSERT_TEST(strcmp(listGetNext(list), str3) == 0);
	listInsertFirst(list, str4);
	listGetFirst(list);
	ASSERT_TEST(listInsertAfterCurrent(list, str2) == LIST_SUCCESS);
	listDestroy(list);
	return true;
}

static bool testListRemoveCurrent() {
	List list = listCreate(copyString, DestroyString);
	ASSERT_TEST(listRemoveCurrent(NULL)==LIST_NULL_ARGUMENT);
	ASSERT_TEST(listRemoveCurrent(list) == LIST_INVALID_CURRENT);
	char *str = "hi", *str2 = "hey", *str4 = "bobooooo";
	listInsertFirst(list, str);
	listInsertLast(list, str2);
	listInsertFirst(list, str4);
	listGetFirst(list);
	ASSERT_TEST(listRemoveCurrent(list) == LIST_SUCCESS);
	listGetFirst(list);
	ASSERT_TEST(strcmp(listGetFirst(list), str) == 0);
	ASSERT_TEST(listRemoveCurrent(list) == LIST_SUCCESS);
	listGetFirst(list);
	ASSERT_TEST(listGetSize(list) == 1);
	ASSERT_TEST(strcmp(listGetFirst(list), str2) == 0);
	ASSERT_TEST(listRemoveCurrent(list) == LIST_SUCCESS);
	ASSERT_TEST(listGetSize(list) == 0);
	ASSERT_TEST(listGetFirst(list)==NULL);
	ASSERT_TEST(listRemoveCurrent(list) == LIST_INVALID_CURRENT);
	listDestroy(list);
	return true;
}

static bool testListSort() {
	char *str1 = "1", *str2 = "22", *str3 = "333", *str4 = "4444",
		*str5 =	"55555";
	List list = listCreate(copyString, DestroyString);
	ASSERT_TEST(listSort(NULL,NULL)==LIST_NULL_ARGUMENT);
	ASSERT_TEST(listSort(list,NULL)==LIST_NULL_ARGUMENT);
	ASSERT_TEST(listSort(list, longerThan) == LIST_SUCCESS);
	listInsertLast(list, str4);
	listInsertLast(list, str5);
	listInsertLast(list, str1);
	listInsertLast(list, str3);
	listInsertLast(list, str2);
	ASSERT_TEST(longerThan(str4, str2) < 0);
	listGetFirst(list);
	listGetNext(list);//iterator at str 5, "list[1]"
	ASSERT_TEST(listSort(list, longerThan) == LIST_SUCCESS);
	//check iterator, should be at str2, "list[1]"
	ASSERT_TEST(strcmp(listGetCurrent(list), str2) == 0);
	ASSERT_TEST(strcmp(listGetFirst(list), str1) == 0);
	ASSERT_TEST(strcmp(listGetNext(list), str2) == 0);
	ASSERT_TEST(strcmp(listGetNext(list), str3) == 0);
	ASSERT_TEST(strcmp(listGetNext(list), str4) == 0);
	ASSERT_TEST(strcmp(listGetNext(list), str5) == 0);
	listDestroy(list);
	return true;
}

static bool testListClear() {
	List list = listCreate(copyString, DestroyString);
	ASSERT_TEST(listClear(NULL)==LIST_NULL_ARGUMENT);
	ASSERT_TEST(listClear(list) == LIST_SUCCESS);
	char *str = "hi", *str2 = "hey", *str3 = "ciao";
	listInsertFirst(list, str);
	listInsertFirst(list, str2);
	listInsertLast(list, str2);
	listInsertFirst(list, str3);
	ASSERT_TEST(listClear(list) == LIST_SUCCESS);
	listDestroy(list);
	return true;
}

static bool testListDestroy() {
	List list = listCreate(copyString, DestroyString);
	char *str = "hi", *str2 = "hey", *str3 = "ciao";
	listInsertFirst(list, str);
	listInsertFirst(list, str2);
	listInsertLast(list, str2);
	listInsertFirst(list, str3);
	listDestroy(list);
	return true;
}

static bool testListFilter() {
	char* key = "mooo";
	ASSERT_TEST(listFilter(NULL,isLongerThan,&key)==NULL);
	List list = listCreate(copyString, DestroyString);
	char *str = "hiii44i", *str2 = "h", *str3 = "ciaoooo", *str4 = "b";
	listInsertFirst(list, str);
	listInsertFirst(list, str2);
	listInsertFirst(list, str3);
	listInsertFirst(list, str4);
	List filtered = listFilter(list, isLongerThan, &key);
	ASSERT_TEST(filtered);
	ASSERT_TEST(listGetSize(filtered) == 2);
	ASSERT_TEST(strcmp(listGetFirst(filtered), str3) == 0);
	ASSERT_TEST(strcmp(listGetNext(filtered), str) == 0);
	listDestroy(list);
	listDestroy(filtered);
	return true;
}

int main() {
	RUN_TEST(testListCreate);
	RUN_TEST(testListCopy);
	RUN_TEST(testListGetSize);
	RUN_TEST(testListGetFirst);
	RUN_TEST(testListGetNext);
	RUN_TEST(testListInsertFirst);
	RUN_TEST(testListInsertLast);
	RUN_TEST(testListInsertBeforeCurrent);
	RUN_TEST(testListInsertAfterCurrent);
	RUN_TEST(testListRemoveCurrent);
	RUN_TEST(testListSort);
	RUN_TEST(testListClear);
	RUN_TEST(testListFilter);
	RUN_TEST(testListDestroy);
	return 0;
}

