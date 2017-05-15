/***************************************************************************
* PSis-project 2016-17
* by
* João Rodrigues and Sara Vieira
*
* server_library.c
* Functions available for the server to use
****************************************************************************/
#include "list.h"
#include "image_server.h"

#ifndef SERVER_LIB
#define SERVER_LIB
int compare_keywords(Item foto1, Item foto2);
uint32_t getphotoid(char * name);
void free_photo(Item foto);

void server_add_photo(int, LinkedList ** photo_list, int server_port);
void server_add_keyword(int, LinkedList * photo_list, int server_port);
void server_search_photo(int , LinkedList * photo_list);
void server_delete_photo(int s_gw, LinkedList ** photo_list, int server_port);
void server_get_photo_name(int s_client, LinkedList * photo_list);

#endif
