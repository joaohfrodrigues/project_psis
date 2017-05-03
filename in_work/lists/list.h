/***************************************************************************
* PSis-project 2016-17
* by
* João Rodrigues and Sara Vieira
*
* list.h
* All functions that manage a list.
****************************************************************************/

#ifndef LinkedListHeader
#define LinkedListHeader



#include "image_server.h"
#include "item.h"

typedef struct LinkedListStruct LinkedList;


LinkedList * initLinkedList(void);
void freeLinkedList(LinkedList * first, void (* freeItemFnt)(Item));
LinkedList *nextHeadLinkedList(LinkedList * head);
LinkedList * getNextNodeLinkedList(LinkedList * node);
Item getItemLinkedList(LinkedList * node);
Item findItemLinkedList(LinkedList * head, Item item, int (* comparisonItemFnt) (Item item1, Item item2));
LinkedList * insertUnsortedLinkedList(LinkedList * next, Item this);
LinkedList * insertSortedLinkedList(LinkedList * first, Item item, int (* comparisonItemFnt) (Item item1, Item item2));


#endif
