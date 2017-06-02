/***************************************************************************
* PSis-project 2016-17
* by
* João Rodrigues and Sara Vieira
*
* client_add_photo.c
* Example client thats adds 100 photos
****************************************************************************/

#include "library.h"

int peer_socket;

/*HANDLING SIGNALS*/
void terminate_ok(int n){
	printf("terminating client...\n");
  gallery_disconnect(peer_socket);
	exit(-1);
}

int main(int argc, char *argv[]){
	int i=0;

	/*signal handling*/
  signal(SIGINT, terminate_ok);

	peer_socket=gallery_connect(argv[1], 3000);

	if(peer_socket==-1){
		printf("error connecting to the gateway\n");
		exit(0);
	}else if(peer_socket==0){
		printf("no peers available\n");
		exit(0);
	}else
		printf("connected to a server\n");

	for(i=0; i<100;i++)
		gallery_add_photo(peer_socket, "aces.bmp");

  gallery_disconnect(peer_socket);
  exit(0);
}
