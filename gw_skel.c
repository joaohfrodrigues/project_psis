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

  message m;
  char client_addr_id[20];
  struct sockaddr_in gw_addr, src_addr;
  socklen_t gw_addr_size, src_addr_size;

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
		}else if(m.type==CLIENT_GW){
			printf("client_connecting\n");
		}
	}

  close(s);
  //unlink(client_addr_id);
  exit(0);

}
