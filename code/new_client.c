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
	uint32_t id=0;
	char *name=(char *) malloc(MESSAGE_LEN*sizeof(char));
	int input;
	char file_name[MESSAGE_LEN];

	printf("******WELCOME TO THE PROPZ IMAGE REPOSITORY******\n");
	printf("connecting to server...\n");

	/*signal handling*/
  signal(SIGINT, terminate_ok);

	peer_socket=gallery_connect(argv[1], 3000);

	if(peer_socket==-1){
		printf("error connecting to the gateway\n");
		exit(0);
	}else if(peer_socket==0){
		printf("no peers available\n");
		exit(0);
	}else{
		printf("connected to a server\n");
	}

	printf("Actions available:\n");
	printf("Add photo - 1\n");
	printf("Add keyword - 2\n");
	printf("Search photo - 3\n");
	printf("Delete photo - 4\n");
	printf("Get photo name - 5\n");
	printf("Get photo - 6\n");
	printf("Choose your action: ");

	while(1){
		scanf("%d", &input);
		switch ( input ) {
      case 1: /*ADD_PHOTO*/
        printf( "You chose to add a photo!\n Please insert the file_name of your photo: " );
				scanf("%s", file_name);
				id
        break;
      case 2: /*ADD KEYWORD*/
        printf( "You chose to add a keyword!\n" );
        break;
      case 3: /*SEARCH PHOTO*/
        printf( "You chose to search a photo!\n" );
        break;
      case 4: /*DELETE PHOTO*/
        printf( "You chose to delete a photo!\n" );
        break;
			case 5: /*GET PHOTO NAME*/
	      printf( "You chose to get a photo name!\n" );
	      break;
			case 5: /*GET PHOTO*/
		    printf( "You chose to get (download) a photo!\n" );
		    break;
      default:
        printf( "Your input doesn't match the available options! Try again!\n" );
        break;
    }
		printf("Choose your action: ");
	}

  gallery_disconnect(peer_socket);
  exit(0);
}
