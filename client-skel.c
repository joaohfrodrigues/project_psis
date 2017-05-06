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

int peer_socket;

/*HANDLING SIGNALS*/
void terminate_ok(int n){
	printf("terminating client...\n");
  close(peer_socket);
	exit(-1);
}

int main(int argc, char *argv[]){
	message m;

  /*signal handling*/
  signal(SIGINT, terminate_ok);

	peer_socket=gallery_connect(argv[1], 3000);

	if(peer_socket==-1){
		printf("error connecting to the gateway\n");
		exit(0);
	}else if(peer_socket==0){
		printf("no peers available\n");
		exit(0);
	}
	
	while(1){
		printf("message: ");
	  //fgets(m.buffer, MESSAGE_LEN, stdin);
		scanf("%s", m.buffer);
	  /* write message */
	  send(peer_socket, &m, sizeof(m), 0);
	}
  close(peer_socket);
  exit(0);
}
