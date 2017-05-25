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
#include <pthread.h>


#include "gw_library.h"

int id=0; /*always increments*/
int count_photos=0; /*decrements when a photo is deleted*/

int compare_addr(Item addr1, Item addr2){
	if(((server_struct *)addr1)->addr.sin_port==((server_struct *)addr2)->addr.sin_port)
    return 1;
  else
    return 0;
}

int compare_sgw_addr(Item addr1, Item addr2){
	if(((server_struct *)addr1)->sgw_addr.sin_port==((server_struct *)addr2)->sgw_addr.sin_port)
    return 1;
  else
    return 0;
}

/*FREES EACH PHOTO*/
void free_server(Item server){
  free((server_struct *) server);
}


void client_connecting(int s, LinkedList **server_list, LinkedList **aux){
  struct sockaddr_in client_addr;
  socklen_t client_addr_size;
  message m_client;

  pthread_rwlock_t s_list;
  pthread_rwlock_init(&s_list, NULL);

  recvfrom(s, &m_client, sizeof(m_client), 0,(struct sockaddr *) &client_addr, &client_addr_size);


  pthread_rwlock_wrlock(&s_list);
  if((*aux)==NULL || getNextNodeLinkedList(*aux)==NULL){
    (*aux)=(*server_list);
  }else{
    (*aux)=getNextNodeLinkedList(*aux);
  }

  server_struct* client_server=(server_struct*) getItemLinkedList(*aux);

  pthread_rwlock_unlock(&s_list);
  pthread_rwlock_destroy(&s_list);

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
	/*RECEBE O ENDEREÇO DO NOVO SERVIDOR*/
  	recv(s, new_server, sizeof(*new_server), 0);
	server_struct *aux_server;
	int type;
  //new_server->addr.sin_addr = server_addr.sin_addr;
	char test[MESSAGE_LEN];
	char test2[MESSAGE_LEN];

	new_server->s_server=s;

	strcpy(test, inet_ntoa(new_server->addr.sin_addr));
	strcpy(test2, inet_ntoa(new_server->sgw_addr.sin_addr));
	printf("server connecting server_port=%d, server ip=%s\n", new_server->addr.sin_port, test);

/*WHEN SERVER_LIST IS NOT NULL, THERE IS ANOTHER SERVER, THEREFORE THE NEED TO REPLICATE THE LINKED LIST*/
	if((*server_list)!=NULL){
		type=SYNC_PHOTO_LIST;
		aux_server= (server_struct*) getItemLinkedList(*server_list);
		send(aux_server->s_server, &type, sizeof(type), 0);
		send(aux_server->s_server, &s, sizeof(s), 0);
	}

	pthread_rwlock_t s_list;
  	pthread_rwlock_init(&s_list, NULL);

  	pthread_rwlock_rdlock(&s_list);

  	(*server_list)=insertUnsortedLinkedList((*server_list), (Item) new_server);

  	pthread_rwlock_unlock(&s_list);
  	pthread_rwlock_destroy(&s_list);
}

void gw_send_photo(int s){
	photo_struct photo;
	FILE *src_file;
	int i, k;
	char c;
	int dest_s=0;
	int type=0;

	recv(s, &dest_s, sizeof(dest_s), 0);
	for(i=0; i<count_photos; i++){
		recv(s, &photo, sizeof(photo), 0);
		type=ADD_PHOTO;
		send(dest_s, (const void *) &type, sizeof(type), 0);
		send(dest_s, (const void *) &photo, sizeof(photo), 0);

		for(k=0; i< photo.size; i++){
			recv(s, &c, sizeof(c), 0);
			send(dest_s, &c, sizeof(c), 0);
		}
	}
}

void server_disconnecting(int s, LinkedList **server_list){
	photo_struct photo;
	int port, ret_value=0;
	/*RECEBE O ENDEREÇO DO NOVO SERVIDOR*/
  recv(s, &port, sizeof(port), 0);
	server_struct server;
	server.addr.sin_port=port;

	pthread_rwlock_t s_list;
  	pthread_rwlock_init(&s_list, NULL);

  	pthread_rwlock_rdlock(&s_list);

	(*server_list)= deleteItemLinkedList((*server_list), (Item) &server, &ret_value, &compare_addr, &free_server);

	pthread_rwlock_unlock(&s_list);
  	pthread_rwlock_destroy(&s_list);

	printf("deleted server with port %d\n", server.addr.sin_port);
}

void gw_add_photo(int s, LinkedList **server_list){
  LinkedList *aux;
	photo_struct photo;
  int type=ADD_PHOTO;
	char c;
	int i=0;
	server_struct *server;
	id++;
	count_photos++;
  server_struct aux_server;
	server_struct *aux2_server;
  recv(s, &photo, sizeof(photo), 0);
	//printf("src_port=%d\n", photo.source);
	photo.id=id;
	photo.nkey=0;

	pthread_rwlock_t s_list;
  	pthread_rwlock_init(&s_list, NULL);

	char *file=(char *)malloc(photo.size*sizeof(char));
	printf("file has size of %d\n", photo.size);
	//char file[photo.size];

	aux_server.sgw_addr.sin_port=photo.source;

	pthread_rwlock_wrlock(&s_list);
	aux2_server= (server_struct*) findItemLinkedList(*server_list,(Item) &aux_server, &compare_sgw_addr);
	pthread_rwlock_unlock(&s_list);
	pthread_rwlock_destroy(&s_list);

	for(i=0; i< photo.size; i++){
		recv(s, &c, sizeof(c), 0);
		file[i]=c;
	}

  for(aux=(*server_list) ; aux!=NULL ; aux=getNextNodeLinkedList(aux)){
		server= (server_struct*) getItemLinkedList(aux);
    send(server->s_server, (const void *) &type, (size_t) sizeof(type), 0);
    send(server->s_server, (const void *) &photo, (size_t) sizeof(photo), 0);
		for(i=0; i< photo.size; i++)
			send(server->s_server, (const void *) &file[i], (size_t) sizeof(file[i]), 0);
  }
}

void gw_add_keyword(int s, LinkedList **server_list){
  LinkedList *aux;
  message m;
  int type=ADD_KEYWORD;
  server_struct *server;

  recv(s, &m, sizeof(m), 0);
	//printf("src_port=%d\n", m.source);
  for(aux=(*server_list) ; aux!=NULL ; aux=getNextNodeLinkedList(aux)){
		server= (server_struct*) getItemLinkedList(aux);
    send(server->s_server, (const void *) &type, (size_t) sizeof(type), 0);
    send(server->s_server, (const void *) &m, (size_t) sizeof(m), 0);
  }
}

void gw_delete_photo(int s, LinkedList **server_list){
  LinkedList *aux;
  message m;
  int type=DELETE_PHOTO;
  server_struct *server;

  recv(s, &m, sizeof(m), 0);
	//printf("src_port=%d\n", m.source);
  for(aux=(*server_list) ; aux!=NULL ; aux=getNextNodeLinkedList(aux)){
		server= (server_struct*) getItemLinkedList(aux);
    send(server->s_server, (const void *) &type, (size_t) sizeof(type), 0);
    send(server->s_server, (const void *) &m, (size_t) sizeof(m), 0);
  }

	count_photos--;
}
