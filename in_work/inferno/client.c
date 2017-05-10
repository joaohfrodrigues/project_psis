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
  gallery_disconnect(peer_socket);
	exit(-1);
}

int main(int argc, char *argv[]){
	char buffer[MESSAGE_LEN];
	int id;

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

	for(int i=0; i<5; i++){
		printf("ADDING PHOTOS\nfilename to add: ");
	  //fgets(m.buffer, MESSAGE_LEN, stdin);
		scanf("%s", buffer);
	  /* write message */
	  gallery_add_photo(peer_socket, buffer);
	}
	for(int i=0; i<3; i++){
		printf("ADDING KEYWORDS\nphotoid: ");
		scanf("%d", &id);
		printf("id=%d	keyword to add: ", id);
	  //fgets(m.buffer, MESSAGE_LEN, stdin);
		scanf("%s", buffer);
	  /* write message */
	  gallery_add_keyword(peer_socket, (uint32_t) id, buffer);
	}
  gallery_disconnect(peer_socket);
  exit(0);
}
