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

int s, s_dgram;

int main(int argc, char *argv[]){

  message m, gateway_message;
  char client_addr_id[20];

  struct sockaddr_in server_addr, client_addr, gateway_addr;
  socklen_t server_addr_size, client_addr_size, gateway_addr_size;


  sprintf(client_addr_id, "sock_cli_%d", getpid());

  /* create socket with gateway */
  s_dgram= socket(AF_INET,SOCK_DGRAM,0);
  if(s_dgram == -1)
  {
    perror("Socket with gateway not created.Error:");
    return 1;
  }
  printf("Socket gateway created\n");

  gateway_addr.sin_family = AF_INET;
  gateway_addr.sin_port = htons(3000); /*numero de porto*/
  inet_aton(argv[1], &gateway_addr.sin_addr);

  gateway_message.type=CLIENT_GW;
  gateway_message.port=client_addr.sin_port;

  /*ask gateway for server to connect*/
  gateway_addr_size=sizeof(gateway_addr);
  printf("message type %d\n", gateway_message.type);
  int bits = sendto(s_dgram, (const void *) &gateway_message, (size_t) sizeof(gateway_message), 0,(const struct sockaddr *) &gateway_addr, (socklen_t) gateway_addr_size);

  printf("sent%d bits to gateway\n", bits);

  /*receive answer from gateway*/
  bits = recvfrom(s_dgram, &m, sizeof(m), 0,(struct sockaddr *) &gateway_addr, &gateway_addr_size);

  printf("received %d bits from gateway\n", bits);
	printf("ip: %s\n", m.buffer);

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

  printf("Socket created\n");

  if(connect(s, (struct sockaddr *) &server_addr, sizeof(server_addr))==-1){
    perror("connect ");
  }

  printf("message: ");
  fgets(m.buffer, MESSAGE_LEN, stdin);

  /* write message */
  send(s, &m, sizeof(m), 0);
  printf("OK\n");

  close(s);
  //unlink(client_addr_id);
  exit(0);

}
