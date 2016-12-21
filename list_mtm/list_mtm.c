#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include "list_mtm.h"

#define NULL_PTR_CHECK(ptr, error)  if(!(ptr)){ \
										return (error); }\

#define RETURN_CURRENT_AND_ADVANCE(node) do{\
	Node result = *node;\
	*node = (*node)->next;\
	return result;\
	}while(0);

typedef struct node_t *Node;
struct node_t {
	ListElement element;
	struct node_t* next;
};

struct List_t {
	Node iterator;
	Node head;
	int elements_number;
	FreeListElement freeElement;
	CopyListElement copyElement;
};

//==============================================================================
/*Allocates memory and creates a new Node to insert to list.
 * Does not inserts the created node.
 * @Return:
 * NULL in case of malloc errors
 * new node otherwisec */
static Node createNode(List list, ListElement element) {
	assert(list && element);
	Node node = malloc(sizeof(*node));
	NULL_PTR_CHECK(node, NULL);
	node->element = list->copyElement(element);
	if(!node->element){
		free(node);
		return NULL;
	}
	node->next = NULL;
	(list->elements_number)++;
	return node;
}

/* Frees allocated for node memory. Asserts list isn't NULL (because list holds
 * destroy function for objects inside the list), node can be NULL */
static void destroyNode(List list, Node node) {
	assert(list);
	if (node) {
		list->freeElement(node->element);
		free(node);
		(list->elements_number)--;
	}
}

/* Returns a node, which considered minimal (defined by compare function)
 * between first nodes in right and left lists.
 * That list (which contained minimal node) will advance to the next member.
 * Lists can be empty, but pointers (right and left) must be valid.
 * If one of lists is empty - returns first node of another list. */
static Node getMinNode(Node* right, Node* left, CompareListElements compare) {
	assert(left && right && compare);
	Node result = NULL;
	if (*left && *right) {
		if (compare((*left)->element, (*right)->element) > 0) {
			RETURN_CURRENT_AND_ADVANCE(left);
		} else {
			RETURN_CURRENT_AND_ADVANCE(right);
		}
	} else if (*left) {
		RETURN_CURRENT_AND_ADVANCE(left);
	} else if (*right) {
		RETURN_CURRENT_AND_ADVANCE(right);
	}
	return result;
}

/* Splits a list into 2 (without copying, just breaking the linkage) at the
 * middle. Output is stored at left and right Nodes. */
static void splitList(int size, Node source, Node *left, Node *right) {
	int size_l = size / 2;
	*left = source;
	*right = source;
	int count = 0;
	while (count < size_l - 1) {
		*right = (*right)->next;
		count++;
	}
	Node tmp = *right;
	*right = tmp->next;
	tmp->next = NULL;
}

/* Gets 2 sorted lists and merges to one sorted list (without copying, just by
 * changing the linkage between nodes). Returns first node in resulting list. */
static Node merge(CompareListElements compare, Node right, Node left) {
	if (right == NULL) {
		return left;

	} else if (left == NULL) {
		return right;
	}
	Node result = getMinNode(&right, &left, compare);
	Node current = result;
	while (left || right) {
		current->next = getMinNode(&right, &left, compare);
		current = current->next;
	}
	return result;
}

/* Recursive mergeSort for linked lists. No additional memory allocated (except
 * for recursion). Returns first node in sorted list.
 * @param compare - compare function, look description in header file
 * @param size - number of elements in list
 * @param temp_head - first node in original list */
static Node mergeSort(CompareListElements compare, int size, Node temp_head) {
	if (size <= 1) {
		return temp_head;
	}
	Node left = NULL, right = NULL;
	splitList(size, temp_head, &left, &right);
	left = mergeSort(compare, size / 2, left);
	right = mergeSort(compare, size - size / 2, right);
	return merge(compare, right, left);

}

/* Returns index of element, pointed by iterator.
 * @Return
 * -1 if list is undefined or iterator is undefined, or list is empty.
 * index of element (starting from 0) otherwise */
static int getIteratorIndex(List list) {
	if (!list || !list->iterator) {
		return -1;
	}
	int index = 0;
	Node current = list->head;
	assert(current); //if iterator isn't NULL, list should not be empty
	while (current != list->iterator) {
		current = current->next;
		index++;
	}
	return index;
}

/* Sets iterator to element with index.
 * IMPORTANT: index must be checked to be valid (less than number of elements),
 * 		before calling this function. */
static void setIteratorIndex(List list, int index) {
	list->iterator = list->head;
	while (index > 0) {
		list->iterator = list->iterator->next;
		index--;
	}
}
//====Interface functions=======================================================
List listCreate(CopyListElement copyElement, FreeListElement freeElement) {
	NULL_PTR_CHECK(copyElement, NULL);
	NULL_PTR_CHECK(freeElement, NULL);
	List list = malloc(sizeof(*list));
	NULL_PTR_CHECK(list, NULL);
	list->head = NULL;
	list->iterator = NULL;
	list->freeElement = freeElement;
	list->copyElement = copyElement;
	list->elements_number = 0;
	return list;
}

ListElement listGetFirst(List list) {
	NULL_PTR_CHECK(list, NULL);
	if (list->elements_number == 0) {
		return NULL;
	}
	list->iterator = list->head;
	return list->iterator->element;

}

ListElement listGetNext(List list) {
	NULL_PTR_CHECK(list, NULL);
	NULL_PTR_CHECK(list->iterator, NULL);
	NULL_PTR_CHECK(list->iterator->next, NULL);
	list->iterator = list->iterator->next;
	return (list->iterator->element);
}

ListElement listGetCurrent(List list) {
	NULL_PTR_CHECK(list, NULL);
	NULL_PTR_CHECK(list->iterator, NULL);
	return list->iterator->element;
}

ListResult listInsertFirst(List list, ListElement element) {
	NULL_PTR_CHECK(list, LIST_NULL_ARGUMENT);
	Node new_node = createNode(list, element);
	NULL_PTR_CHECK(new_node, LIST_OUT_OF_MEMORY);
	new_node->next = list->head;
	list->head = new_node;
	return LIST_SUCCESS;

}

ListResult listInsertLast(List list, ListElement element) {
	NULL_PTR_CHECK(list, LIST_NULL_ARGUMENT);
	Node new_node = createNode(list, element);
	NULL_PTR_CHECK(new_node, LIST_OUT_OF_MEMORY);
	if (!list->head) {
		list->head = new_node;
		new_node->next = NULL;
		return LIST_SUCCESS;
	}
	Node temp = list->head;
	while (temp->next) {
		temp = temp->next;
	}
	temp->next = new_node;
	new_node->next = NULL;
	return LIST_SUCCESS;
}

int listGetSize(List list) {
	NULL_PTR_CHECK(list, -1);
	return list->elements_number;
}

List listCopy(List list) {
	NULL_PTR_CHECK(list, NULL);
	List new_list = listCreate(list->copyElement, list->freeElement);
	if (list->head == NULL) {
		return new_list;
	}
	NULL_PTR_CHECK(new_list, NULL);
	new_list->iterator = list->iterator;
	new_list->elements_number = list->elements_number;
	Node temp = list->head;
	while (temp) {
		listInsertLast(new_list, temp->element);
		temp = temp->next;
	}
	return new_list;
}

ListResult listInsertBeforeCurrent(List list, ListElement element) {
	NULL_PTR_CHECK(list, LIST_NULL_ARGUMENT);
	NULL_PTR_CHECK(list->iterator, LIST_INVALID_CURRENT);
	if (list->iterator == list->head || list->head == NULL) {
		return listInsertFirst(list, element);
	}
	Node new_node = createNode(list, element);
	NULL_PTR_CHECK(new_node, LIST_OUT_OF_MEMORY);
	Node temp = list->head;
	while (temp) {
		if (temp->next == list->iterator) {
			break;
		}
		temp = temp->next;
	}
	new_node->next = temp->next;
	temp->next = new_node;
	return LIST_SUCCESS;
}

ListResult listInsertAfterCurrent(List list, ListElement element) {
	NULL_PTR_CHECK(list, LIST_NULL_ARGUMENT);
	NULL_PTR_CHECK(list->iterator, LIST_INVALID_CURRENT);
	Node new_node = createNode(list, element);
	NULL_PTR_CHECK(new_node, LIST_OUT_OF_MEMORY);
	new_node->next = list->iterator->next;
	list->iterator->next = new_node;
	return LIST_SUCCESS;
}

ListResult listRemoveCurrent(List list) {
	NULL_PTR_CHECK(list, LIST_NULL_ARGUMENT);
	NULL_PTR_CHECK(list->iterator, LIST_INVALID_CURRENT);
	if (list->iterator == list->head) {
		list->head = list->iterator->next;
		destroyNode(list, list->iterator);
		list->iterator = NULL;
		return LIST_SUCCESS;
	}
	Node temp = list->head;
	while (temp->next != list->iterator) {
		temp = temp->next;
	}
	temp->next = list->iterator->next;
	destroyNode(list, list->iterator);
	list->iterator = NULL;
	return LIST_SUCCESS;
}

ListResult listClear(List list) {
	NULL_PTR_CHECK(list, LIST_NULL_ARGUMENT);
	if (list->head) {
		Node temp = list->head, current = list->head;
		while (temp) {
			temp = current->next;
			destroyNode(list, current);
			current = temp;
		}
		list->head = NULL;
		list->iterator = NULL;
		list->elements_number = 0;
	}
	return LIST_SUCCESS;
}

void listDestroy(List list) {
	if (list) {
		listClear(list);
		free(list);
	}
}

ListResult listSort(List list, CompareListElements compareElement) {
	NULL_PTR_CHECK(list, LIST_NULL_ARGUMENT);
	NULL_PTR_CHECK(compareElement, LIST_NULL_ARGUMENT);
	int index = getIteratorIndex(list);
	list->head = mergeSort(compareElement, list->elements_number, list->head);
	if (index >= 0) {
		setIteratorIndex(list, index);
	}
	return LIST_SUCCESS;
}

List listFilter(List list, FilterListElement filterElement, ListFilterKey key) {
	NULL_PTR_CHECK(list, NULL);
	NULL_PTR_CHECK(filterElement, NULL);
	NULL_PTR_CHECK(key, NULL);
	List filtered_list = listCreate(list->copyElement, list->freeElement);
	NULL_PTR_CHECK(filtered_list, NULL);
	LIST_FOREACH(ListElement,temp_element,list)
	{
		if (filterElement(temp_element, key)) {
			if(listInsertLast(filtered_list, temp_element)!=LIST_SUCCESS){
				listDestroy(filtered_list);
				return NULL;
			}
		}
	}
	filtered_list->iterator = filtered_list->head;
	return filtered_list;
}
