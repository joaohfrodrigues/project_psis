/***************************************************************************
* PSis-project 2016-17
* by
* João Rodrigues and Sara Vieira
*
* library.h
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
    return 1;
  }
  printf("Socket gateway created\n");

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
