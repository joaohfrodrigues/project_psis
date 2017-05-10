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

void client_connecting(int s, LinkedList *server_list, struct sockaddr_in client_addr){
  LinkedList *aux=NULL;
  message m_client;
  if(aux==NULL || getNextNodeLinkedList(aux)==NULL){
    aux=server_list;
  }else{
    aux=getNextNodeLinkedList(aux);
  }

  server_struct* client_server=(server_struct*) getItemLinkedList(aux);

  m_client.type = CLIENT_GW;
  m_client.port = client_server->addr.sin_port;
  strcpy(m_client.buffer, inet_ntoa(client_server->addr.sin_addr));
  printf("client connecting	server_port=%d	server_ip=%s\n", m_client.port, m_client.buffer);
  sendto(s, (const void *) &m_client, (size_t) sizeof(m_client), 0,(const struct sockaddr *) &client_addr, (socklen_t) sizeof(client_addr));
}

void server_connecting(int s, LinkedList **server_list, message m, struct sockaddr_in server_addr){
  server_struct *new_server=(server_struct *) malloc(sizeof(server_struct));
  strcpy(m.buffer, inet_ntoa(server_addr.sin_addr));

  new_server->addr.sin_family = AF_INET;
  new_server->addr.sin_port = m.port;
  new_server->addr.sin_addr = server_addr.sin_addr;

  (*server_list)=insertUnsortedLinkedList((*server_list), (Item) new_server);

  printf("server connecting	server_port=%d	server_ip=%s\n", m.port, m.buffer);
}
