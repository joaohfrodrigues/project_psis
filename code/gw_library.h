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

void client_connecting(int s, LinkedList *server_list, struct sockaddr_in client_addr);

void server_connecting(int s, LinkedList **server_list, message m, struct sockaddr_in client_addr);
