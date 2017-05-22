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
	char buffer[MESSAGE_LEN];
	uint32_t *id_photos=NULL;
	int n_photos=0;
	int i=0;


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

	while(1){
		printf("\nActions available:\n");
		printf("Add photo - 1\n");
		printf("Add keyword - 2\n");
		printf("Search photo - 3\n");
		printf("Delete photo - 4\n");
		printf("Get photo name - 5\n");
		printf("Get photo - 6\n");
		printf("Exit - 7\n");
		printf("Choose your action: ");
		scanf("%d", &input);
		switch ( input ) {
      case 1: /*ADD_PHOTO*/
        printf("\nYou chose to add a photo!\n\nPlease insert the file_name of your photo: " );
				scanf("%s", buffer);
				id = gallery_add_photo(peer_socket, buffer);
				if(id > 0)
					printf("Your photo was added successfully. The id is %d\n", id);
				else if(id == 0)
					printf("Invalid file_name or problems in communication with the server\n");
        break;
      case 2: /*ADD KEYWORD*/
        printf("\nYou chose to add a keyword!\n\nPlease insert the id of the photo in which the keyword will be inserted: " );
				scanf("%d", &id);
				printf("Now insert the keyword too add: ");
				scanf("%s", buffer);
				gallery_add_keyword(peer_socket, id, buffer);
				printf("Keyword added successfully!\n\n");
        break;
      case 3: /*SEARCH PHOTO*/
        printf("\nYou chose to search a photo!\n\nPlease insert the keyword to search for: " );
				scanf("%s", buffer);
				gallery_search_photo(peer_socket, buffer, &id_photos);
		break;
      case 4: /*DELETE PHOTO*/
        printf("\nYou chose to delete a photo!\n\nPlease insert the id of the photo to be deleted: " );
				scanf("%d", &id);
				gallery_delete_photo(peer_socket, id);
        break;
			case 5: /*GET PHOTO NAME*/
	      printf("\nYou chose to get a photo name!\n\nPlease insert the id of the photo to be searched: ");
				scanf("%d", &id);
				gallery_get_photo_name(peer_socket, id, &name);
				printf("Filename found: %s\n", name);
	      break;
			case 6: /*GET PHOTO*/
		    printf("\nYou chose to get (download) a photo!\n\nPlease insert the id of the photo to be downloaded: ");
				scanf("%d", &id);
				gallery_get_photo(peer_socket, id, buffer);
				printf("Successfully downloaded photo %s\n", buffer);
		    break;
			case 7:
				printf("Exiting...\n");
				gallery_disconnect(peer_socket);
				exit(0);
				break;
      default:
        printf("\nWe give 7 numbers and you manage to miss! Try again\n" );
        break;
    }
		printf("\nChoose your action: ");
	}

  gallery_disconnect(peer_socket);
  exit(0);
}
