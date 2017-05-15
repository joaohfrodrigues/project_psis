/***************************************************************************
* PSis-project 2016-17
* by
* João Rodrigues and Sara Vieira
*
* gw_library.c
* Functions available for the gateway to use
****************************************************************************/
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <time.h>


#include "gw_library.h"

int id=0; /*always increments*/
int count_photos=0; /*decrements when a photo is deleted*/

int compare_addr(Item addr1, Item addr2){
	if(((server_struct *)addr1)->addr.sin_port==((server_struct *)addr2)->addr.sin_port)
    return 1;
  else
    return 0;
}

void client_connecting(int s, LinkedList **server_list, LinkedList **aux){
  struct sockaddr_in client_addr;
  socklen_t client_addr_size;
  message m_client;

  recvfrom(s, &m_client, sizeof(m_client), 0,(struct sockaddr *) &client_addr, &client_addr_size);

  if((*aux)==NULL || getNextNodeLinkedList(*aux)==NULL){
    (*aux)=(*server_list);
  }else{
    (*aux)=getNextNodeLinkedList(*aux);
  }

  server_struct* client_server=(server_struct*) getItemLinkedList(*aux);

	if(client_server==NULL){
		m_client.port=0;
		sendto(s, (const void *) &m_client, (size_t) sizeof(m_client), 0,(const struct sockaddr *) &client_addr, (socklen_t) sizeof(client_addr));
		return;
	}

  m_client.type = CLIENT_GW;
  m_client.port = client_server->addr.sin_port;
  strcpy(m_client.buffer, inet_ntoa(client_server->addr.sin_addr));
  printf("client connecting	server_port=%d	server_ip=%s\n", m_client.port, m_client.buffer);
  sendto(s, (const void *) &m_client, (size_t) sizeof(m_client), 0,(const struct sockaddr *) &client_addr, (socklen_t) sizeof(client_addr));
}

void server_connecting(int s, LinkedList **server_list){
  server_struct *new_server=(server_struct *) malloc(sizeof(server_struct));
	struct sockaddr_in server_addr;
	socklen_t server_addr_size;
	photo_struct photo;
	message m;
	/*RECEBE O ENDEREÇO DO NOVO SERVIDOR*/
  recvfrom(s, new_server, sizeof(*new_server), 0,(struct sockaddr *) &server_addr, &server_addr_size);
	server_struct *aux_server;
	int type;
  //new_server->addr.sin_addr = server_addr.sin_addr;
	char test[MESSAGE_LEN];
	char test2[MESSAGE_LEN];
	int i;

/*WHEN SERVER_LIST IS NOT NULL, THERE IS ANOTHER SERVER, THEREFORE THE NEED TO REPLICATE THE LINKED LIST*/
	if((*server_list)!=NULL){
		type=SYNC_PHOTO_LIST;
		aux_server= (server_struct*) getItemLinkedList(*server_list);
		sendto(s, (const void *) &type, sizeof(type), 0,(struct sockaddr *) &aux_server->sgw_addr, sizeof(aux_server->sgw_addr));

		for(i=0; i<count_photos; i++){
			recv(s, &photo, sizeof(photo), 0);
			type=ADD_PHOTO;
			sendto(s, (const void *) &type, sizeof(type), 0,(struct sockaddr *) &server_addr, server_addr_size);
			sendto(s, (const void *) &photo, sizeof(photo), 0,(struct sockaddr *) &server_addr, server_addr_size);
		}
	}

  (*server_list)=insertUnsortedLinkedList((*server_list), (Item) new_server);
	strcpy(test, inet_ntoa(new_server->addr.sin_addr));
	strcpy(test2, inet_ntoa(new_server->sgw_addr.sin_addr));

  printf("server connecting server_port=%d, server ip=%s\n", new_server->addr.sin_port, test);
	printf("sgw server_port=%d, server ip=%s\n", new_server->sgw_addr.sin_port, test2);
}

void gw_add_photo(int s, LinkedList **server_list){
  LinkedList *aux;
	photo_struct photo;
  int type=ADD_PHOTO;
	server_struct *server;
	id++;
	count_photos++;
  /*struct sockaddr_in aux_addr;*/
  recv(s, &photo, sizeof(photo), 0);
	printf("src_port=%d\n", photo.source);
	photo.id=id;
	photo.nkey=0;
  for(aux=(*server_list) ; aux!=NULL ; aux=getNextNodeLinkedList(aux)){
		server= (server_struct*) getItemLinkedList(aux);
    sendto(s, (const void *) &type, (size_t) sizeof(type), 0,(const struct sockaddr *) &(server->sgw_addr), (socklen_t) sizeof(server->sgw_addr));
    sendto(s, (const void *) &photo, (size_t) sizeof(photo), 0,(const struct sockaddr *) &(server->sgw_addr), (socklen_t) sizeof(server->sgw_addr));
  }
}

void gw_add_keyword(int s, LinkedList **server_list){
  LinkedList *aux;
  message m;
  int type=ADD_KEYWORD;
  server_struct *server;

  recv(s, &m, sizeof(m), 0);
	printf("src_port=%d\n", m.source);
  for(aux=(*server_list) ; aux!=NULL ; aux=getNextNodeLinkedList(aux)){
		server= (server_struct*) getItemLinkedList(aux);
    sendto(s, (const void *) &type, (size_t) sizeof(type), 0,(const struct sockaddr *) &(server->sgw_addr), (socklen_t) sizeof(server->sgw_addr));
    sendto(s, (const void *) &m, (size_t) sizeof(m), 0,(const struct sockaddr *) &(server->sgw_addr), (socklen_t) sizeof(server->sgw_addr));
  }
}

void gw_delete_photo(int s, LinkedList **server_list){
  LinkedList *aux;
  message m;
  int type=DELETE_PHOTO;
  server_struct *server;

  recv(s, &m, sizeof(m), 0);
	printf("src_port=%d\n", m.source);
  for(aux=(*server_list) ; aux!=NULL ; aux=getNextNodeLinkedList(aux)){
		server= (server_struct*) getItemLinkedList(aux);
    sendto(s, (const void *) &type, (size_t) sizeof(type), 0,(const struct sockaddr *) &(server->sgw_addr), (socklen_t) sizeof(server->sgw_addr));
    sendto(s, (const void *) &m, (size_t) sizeof(m), 0,(const struct sockaddr *) &(server->sgw_addr), (socklen_t) sizeof(server->sgw_addr));
  }
}
