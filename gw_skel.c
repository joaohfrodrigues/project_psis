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

int s;

void terminate_ok(int n){
	int in;
	//printf("received signal %d, do you want to exit? (0/1)", n);
	//scanf("%d", &in);
	//if(in==0){
	//	return;
	//}else{
    close(s);
		exit(-1);
	//}
}

int main(int argc, char *argv[]){

  message m, m_client;
  char client_addr_id[20];
  struct sockaddr_in gw_addr, src_addr;
  socklen_t gw_addr_size, src_addr_size;

  struct sockaddr_in server_addr, client_addr;
  socklen_t server_addr_size, client_addr_size;

  /*signal handling*/
  signal(SIGINT, terminate_ok);


  /* create socket  */
  s= socket(AF_INET,SOCK_DGRAM,0);
  if(s == -1)
  {
    perror("Socket not created.Error:");
    return 1;
  }
  printf("Socket created\n");

  gw_addr.sin_family = AF_INET;
  gw_addr.sin_port = htons(3000); /*numero de porto*/
  gw_addr.sin_addr.s_addr = INADDR_ANY; /*IP*/

  /*bind the socket gatway*/
  if(bind(s,(const struct sockaddr*)&gw_addr,sizeof(gw_addr)) == -1)
  {
    perror("binding failed. Error:");
    return 1;
  }
  printf("Bind completed\n");

	while(1) {
		recvfrom(s, &m, sizeof(m), 0,(struct sockaddr *) &src_addr, &src_addr_size);
		//recv(s, &m, sizeof(m), 0);
		printf("received\n");

		if(m.type==GW_SERVER){
			printf("server connecting\n");
      server_addr.sin_family = AF_INET;
      server_addr.sin_port = src_addr.sin_port;
      server_addr.sin_addr = src_addr.sin_addr;
      server_addr_size = src_addr_size;

		}else if(m.type==CLIENT_GW){
			printf("client connecting\n");
      client_addr.sin_family = AF_INET;
      client_addr.sin_port = src_addr.sin_port;
      client_addr.sin_addr = src_addr.sin_addr;
      client_addr_size = src_addr_size;

      /*if(server_addr.sin_family!=AF_INET){/* ver outra condição de teste: ver se lista está vazia*/
          m_client.type = CLIENT_GW;
          m_client.port = server_addr.sin_port;
          m_client.address = inet_ntoa(server_addr.sin_addr);

          printf("message type %d\n", m_client.type);
          int bits=sendto(s, (const void *) &m_client, (size_t) sizeof(m_client), 0,(const struct sockaddr *) &client_addr, (socklen_t) client_addr_size);

          printf("sent%d bits to client\n", bits);
      //}

		}
	}

  close(s);
  //unlink(client_addr_id);
  exit(0);

}
