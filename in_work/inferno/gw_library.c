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

int id=1;

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

  m_client.type = CLIENT_GW;
  m_client.port = client_server->addr.sin_port;
  strcpy(m_client.buffer, inet_ntoa(client_server->addr.sin_addr));
  printf("client connecting	server_port=%d	server_ip=%s\n", m_client.port, m_client.buffer);
  sendto(s, (const void *) &m_client, (size_t) sizeof(m_client), 0,(const struct sockaddr *) &client_addr, (socklen_t) sizeof(client_addr));
}

void server_connecting(int s, LinkedList **server_list, struct sockaddr_in server_addr){
  server_struct *new_server=(server_struct *) malloc(sizeof(server_struct));
  message m;

  recv(s, &m, sizeof(m), 0);
  new_server->addr.sin_family = AF_INET;
  new_server->addr.sin_port = m.port;
  inet_aton(m.buffer, &(new_server->addr.sin_addr));
  //new_server->addr.sin_addr = server_addr.sin_addr;

  (*server_list)=insertUnsortedLinkedList((*server_list), (Item) new_server);

  printf("server connecting	server_port=%d	server_ip=%s\n", m.port, m.buffer);
}

void gw_add_photo(int s, LinkedList **server_list){
  LinkedList *aux;
  message m;
	struct sockaddr_in server_addr;
	socklen_t server_addr_size;
  int type=ADD_PHOTO;
  /*struct sockaddr_in aux_addr;*/
  recvfrom(s, &m, sizeof(m), 0, (struct sockaddr *) &server_addr, &server_addr_size);
	m.port=id++;
  for(aux=(*server_list) ; aux!=NULL ; aux=getNextNodeLinkedList(aux)){
    sendto(s, (const void *) &type, (size_t) sizeof(type), 0,(const struct sockaddr *) &server_addr, (socklen_t) sizeof(server_addr));
    sendto(s, (const void *) &m, (size_t) sizeof(m), 0,(const struct sockaddr *) &server_addr, (socklen_t) sizeof(server_addr));
  }
}

void gw_add_keyword(int s, LinkedList **server_list){
  LinkedList *aux;
  message m;
	struct sockaddr_in server_addr;
	socklen_t server_addr_size;
  int type=ADD_KEYWORD;
  struct sockaddr_in aux_addr;
  recvfrom(s, &m, sizeof(m), 0, (struct sockaddr *) &server_addr, &server_addr_size);
  for(aux=(*server_list) ; aux!=NULL ; aux=getNextNodeLinkedList(aux)){
		sendto(s, (const void *) &type, (size_t) sizeof(type), 0,(const struct sockaddr *) &server_addr, (socklen_t) sizeof(server_addr));
    sendto(s, (const void *) &m, (size_t) sizeof(m), 0,(const struct sockaddr *) &server_addr, (socklen_t) sizeof(server_addr));
  }
}
