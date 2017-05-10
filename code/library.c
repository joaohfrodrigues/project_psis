/***************************************************************************
* PSis-project 2016-17
* by
* João Rodrigues and Sara Vieira
*
* library.c
* Functions available for the client to use
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

#include "image_server.h"
#include "library.h"

int gallery_connect(char * host, in_port_t port){
  int s, s_dgram;
  message m, gateway_message;
  struct sockaddr_in server_addr, client_addr, gateway_addr;
  socklen_t server_addr_size, client_addr_size, gateway_addr_size;

  s_dgram= socket(AF_INET,SOCK_DGRAM,0);
  if(s_dgram == -1)
  {
    perror("Socket with gateway not created.Error:");
    return -1;
  }

  gateway_addr.sin_family = AF_INET;
  gateway_addr.sin_port = htons(port); /*numero de porto*/
  inet_aton(host, &gateway_addr.sin_addr);
  gateway_addr_size= sizeof(gateway_addr);

  gateway_message.type=CLIENT_GW;
  gateway_message.port=client_addr.sin_port;

  /*ask gateway for server to connect*/
  sendto(s_dgram, (const void *) &gateway_message, (size_t) sizeof(gateway_message), 0,(const struct sockaddr *) &gateway_addr, (socklen_t) sizeof(gateway_addr));
  /*receive answer from gateway*/
  recvfrom(s_dgram, &m, sizeof(m), 0,(struct sockaddr *) &gateway_addr, &gateway_addr_size);

  if(m.port==0){
    return 0;
  }

  printf("server_ip: %s\n", m.buffer);
  printf("server_port:%d\n", m.port);

  /*create socket with server*/
  s= socket(AF_INET,SOCK_STREAM,0);
  if(s == -1)
  {
    perror("Socket not created.Error:");
    return 1;
  }
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = m.port; /*numero de porto*/
  inet_aton(m.buffer, &server_addr.sin_addr); /*IP*/ /*fazer inet_ntoa na gateway do server e guardar no buffer!*/

  if(connect(s, (struct sockaddr *) &server_addr, sizeof(server_addr))==-1){
    perror("connect ");
  }

  return s;
}

uint32_t gallery_add_photo(int peer_socket, char *file_name){
  message m;
  int type=ADD_PHOTO;
  strcpy(m.buffer, file_name);
  m.port=0;
  
  send(peer_socket, &type, sizeof(type), 0);
  send(peer_socket, &m, sizeof(m), 0);
  recv(peer_socket, &m, sizeof(m), 0);

  printf("sucess\n");

  return m.port;
}

int gallery_add_keyword(int peer_socket, uint32_t id_photo, char *keyword){
  message m;
  int type=ADD_KEYWORD;
  strcpy(m.buffer, keyword);
  m.port=id_photo;

  send(peer_socket, &type, sizeof(type), 0);
  send(peer_socket, &m, sizeof(m), 0);
  recv(peer_socket, &m, sizeof(m), 0);

  if(m.port==1)
    printf("added keyword <%s>\n", m.buffer);
  else if(m.port==0)
    printf("keyword <%s> already in use\n", m.buffer);
  else if(m.port==-1)
    printf("no more space for new keywords\n");
  else if(m.port==-2)
    printf("id not recognized\n");

  printf("sucess\n");

  return m.port;
}

int gallery_search_photo(int peer_socket, char * keyword, uint32_t ** id_photos){
  message m;
  int type=SEARCH_PHOTO;
  strcpy(m.buffer, keyword);

}


int gallery_disconnect(int peer_socket){
  message m;
  int type=CLIENT_DEATH;

  send(peer_socket, &type, sizeof(type), 0);
  send(peer_socket, &m, sizeof(m), 0);
  close(peer_socket);
}
