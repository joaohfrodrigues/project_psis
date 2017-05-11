/***************************************************************************
* PSis-project 2016-17
* by
* João Rodrigues and Sara Vieira
*
* gw_library.c
* Functions available for the gateway to use
****************************************************************************/
#include "list.h"
#include "image_server.h"


int compare_addr(Item addr1, Item addr2);
void *thrd_server_fnc(void *arg);
void *thrd_client_fnc(void *arg);
void *thrd_sync_fnc(void *arg);

void client_connecting(int s, LinkedList **server_list, LinkedList **aux);
void server_connecting(int s, LinkedList **server_list, struct sockaddr_in client_addr);

void gw_add_photo(int s, LinkedList **server_list);
void gw_add_keyword(int s, LinkedList **server_list);
