/***************************************************************************
* PSis-project 2016-17
* by
* João Rodrigues and Sara Vieira
*
* list.c
* All functions that manage a list.
****************************************************************************/

#include<stdio.h>
#include<stdlib.h>

#include"LinkedList.h"

/*
 *  Data Type: LinkedListStruct
 *
 *  Description: Structure with:
 *                 1) Pointer to the item of the linked list node
 *                 2) Pointer to next node of the linked list.
 */
struct LinkedListStruct
{
  Item this;
  LinkedList * next;
};

/*
 *  Function:
 *    initLinkedList
 *
 *  Description:
 *    Initializes a new linked list.
 *
 *  Arguments:
 *    None
 *
 *  Return value:
 *    Returns the pointer to a new linked list.
 */
LinkedList * initLinkedList(void)
{
  return NULL;
}

/*
 *  Function:
 *    freeLinkedList
 *
 *  Description:
 *    Frees the memory allocated to a linked list.
 *
 *  Arguments:
 *    Pointer to the first element of a linked list:
 *      (LinkedList *) first
 *    Function to free the memory allocated to the items:
 *      void freeItem(Item)
 *
 *  Return value:
 *    None
 */
void freeLinkedList(LinkedList * first, void (* freeItemFnt)(Item item))
{
  LinkedList * next;
  LinkedList * aux;

  if(first == NULL)
    return;

  /* Cycle from the first to the last element                     */
  for(aux = first; aux != NULL; aux = next)
  {
    /* Keep trace of the next node                                */
    next = aux->next;

    /* Free current item                                          */
    freeItemFnt(aux->this);

    /* Free current node                                          */
    free(aux);
  }

  return;
}

/*
 *  Function:
 *    getNextNodeLinkedList
 *
 *  Description:
 *    Returns the next node of a linked list.
 *
 *  Arguments:
 *    Pointer to the current linked list node:
 *        (LinkedList *) node
 *
 *  Return value:
 *    Returns the pointer to the next node of a linked list. NULL
 *   is returned in case the current node is empty or there is no
 *   node following the current node.
 */
LinkedList * getNextNodeLinkedList(LinkedList * node)
{
  return ((node == NULL) ? NULL : node->next);
}

/*
 *  Function:
 *    getItemLinkedList
 *
 *  Description:
 *    Gets the item of a linked list node.
 *
 *  Arguments:
 *    Pointer to a linked list node:
 *        (LinkedList *) node
 *
 *  Return value:
 *    Returns the pointer to the item of a linked list node. NULL
 *   is returned if the node is NULL (or if the item is NULL).
 */
Item getItemLinkedList(LinkedList * node)
{
  /* Check if node is not empty                                   */
  if(node == NULL)
    return NULL;

  return node->this;
}

/*
 *  Function:
 *    findItemLinkedList
 *
 *  Description:
 *    Finds an Item in the LinkedList
 */
Item findItemLinkedList(LinkedList * head, Item item, int (* comparisonItemFnt) (Item item1, Item item2))
{
  LinkedList *aux;

  if(head == NULL)
    return item;

  for(aux = head; aux != NULL; aux = aux->next)
    if((* comparisonItemFnt)(aux->this, item))
      return (aux->this);

  return item;
}

/*
 *  Function:
 *    freeHeadLinkedList
 *
 *  Description:
 *    Frees the head of the LinkedList
 */
LinkedList *nextHeadLinkedList(LinkedList * head)
{
  LinkedList *aux;

  if(head == NULL)
    return NULL;
  aux = head;
  head = head->next;

  free(aux);

  return head;
}


/*
 *  Function:
 *    insertUnsortedLinkedList
 *
 *  Description:
 *    Creates a new linked list node.
 *
 *  Arguments:
 *    Item to associate to the new node:
 *      Item this
 *    Pointer to the next node:
 *      (LinkedList *) next
 *
 *  Return value:
 *    Returns the pointer to the node.
 */
LinkedList * insertUnsortedLinkedList(LinkedList * head, Item this)
{
  LinkedList * new;
  LinkedList *aux;

  /* Memory allocation                                            */
  new = (LinkedList *) malloc(sizeof(LinkedList));

  /* Check memory allocation errors                               */
  if(new == NULL)
    return NULL;

  /* Initialize new node                                          */
  new->this = this;
  new->next = NULL;

  if(head == NULL)
    return new;

  for(aux = head; aux->next != NULL; aux = aux->next);

  aux->next = new;

  return head;
}



/*
 *  Function:
 *    insertSortedLinkedList
 *
 *  Description:
 *    Inserts an item in order in an sorted linked list.
 *
 *  Arguments:
 *    Pointer to the first node of a sorted linked list:
 *        (LinkedList *) first
 *    Pointer to item to be inserted:
 *        Item item
 *    Pointer to function to compare two items:
 *        int comparisonItemFnt(void * item1, void * item2)
 *
 *        This function returns a value less, equal, or greater
 *       than zero if item1 compares less, equal, or greater than
 *       item2, respectively.
 *
 *    Pointer to integer to write error return value:
 *        (int *) err
 *
 *        0 upon sucess, 1 in case the item is NULL, and 2 in
 *   case of memory allocation failure.
 *
 *  Return value:
 *    Returns the pointer to the first node of the sorted linked list.
 */
LinkedList * insertSortedLinkedList(LinkedList * first, Item item, int (* comparisonItemFnt) (Item item1, Item item2))
{
  LinkedList *aux;
  LinkedList * new;

  /* Memory allocation                                            */
  new = (LinkedList *) malloc(sizeof(LinkedList));
  new->this = item;

  if(first == NULL)
  {
    new->next = NULL;
    return new;
  }
  for(aux = first; aux->next != NULL; aux = aux->next)
  {
      if((* comparisonItemFnt)(aux->next->this, item))
        break;
  }
  new->next = aux->next;
  aux->next = new;

  return first;
}
