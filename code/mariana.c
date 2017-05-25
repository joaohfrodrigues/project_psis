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
	uint32_t id=0;
	char *name=NULL;
	int input;

	uint32_t *id_photos=NULL;
	int n_photos=0;
	int i=0;


	printf("******WELCOME TO THE PROPZ IMAGE REPOSITORY******\n");
	printf("connecting to server...\n");

	/*signal handling*/
  	signal(SIGINT, terminate_ok);

	peer_socket=gallery_connect(argv[1], 3000);


		id = gallery_add_photo(peer_socket, "sporting.png");


	exit(0);
}
