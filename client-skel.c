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

void terminate_ok(int n){
	int in;
	printf("received signal %d, do you want to exit? (0/1)", n);
	scanf("%d", &in);
	if(in==0){
		return;
	}else{
    close(s);
		exit(-1);
	}
}

int main(){

  message m;
  char client_addr_id[20];
  char * story;
  story = strdup("");
  int s;
  struct sockaddr_in server_addr, client_addr;
  socklen_t server_addr_size, client_addr_size;

  /*signal handling*/
  signal(SIGINT, terminate_ok);

  sprintf(client_addr_id, "sock_cli_%d", getpid());
  /* create socket  */
  s= socket(AF_INET,SOCK_STREAM,0);
  if(s == -1)
  {
    perror("Socket not created.Error:");
    return 1;
  }
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(3000); /*numero de porto*/
  server_addr.sin_addr.s_addr = INADDR_ANY; /*IP*/

  printf("Socket created\n");

  if(connect(s, (struct sockaddr *) &server_addr, sizeof(server_addr))==-1){
    perror("connect ");
  }

  printf("message: ");
  fgets(m.buffer, MESSAGE_LEN, stdin);

  /* write message */
  send(s, &m, sizeof(m), 0);
  printf("OK\n");
  /* receive story */
  recv(s, story, 1000000, 0);

  printf("%s\n", story);

  close(s);
  //unlink(client_addr_id);
  exit(0);

}
