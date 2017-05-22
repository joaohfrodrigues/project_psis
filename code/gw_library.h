/***************************************************************************
* PSis-project 2016-17
* by
* João Rodrigues and Sara Vieira
*
* gw_library.h
* Functions available for the gateway to use
****************************************************************************/
#include "list.h"
#include "image_server.h"


int compare_addr(Item addr1, Item addr2);
int compare_sgw_addr(Item addr1, Item addr2);
void free_server(Item server);
void *thrd_server_fnc(void *arg);
void *thrd_client_fnc(void *arg);
void *thrd_sync_fnc(void *arg);

void client_connecting(int s, LinkedList **server_list, LinkedList **aux);
void server_connecting(int s, LinkedList **server_list);
void server_disconnecting(int s, LinkedList **server_list);

void gw_add_photo(int s, LinkedList **server_list);
void gw_add_keyword(int s, LinkedList **server_list);
void gw_delete_photo(int s, LinkedList **server_list);
void gw_send_photo(int s);
