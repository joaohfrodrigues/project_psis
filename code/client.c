/***************************************************************************
* PSis-project 2016-17
* by
* João Rodrigues and Sara Vieira
*
* client.c
* Example client
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
	char *name=NULL;
	int input;
	int id=0;
	char buffer[MESSAGE_LEN];
	uint32_t *id_photos=NULL;
	uint32_t ret_val=0;

	pthread_mutex_t ph;

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
        printf("\nYou chose to add a photo!\nPlease insert the file_name of your photo: " );
				scanf("%s", buffer);
				pthread_mutex_lock(&ph);
				ret_val = gallery_add_photo(peer_socket, buffer);
				pthread_mutex_unlock(&ph);
				if(ret_val > 0)
					printf("Your photo was added successfully. The id is %d\n", ret_val);
				else if(ret_val == 0)
					printf("Invalid file_name or problems in communication with the server\n");
        break;
      case 2: /*ADD KEYWORD*/
        printf("\nYou chose to add a keyword!\nPlease insert the id of the photo in which the keyword will be inserted: " );
				scanf("%d", &id);
				printf("Now insert the keyword too add: ");
				scanf("%s", buffer);
				gallery_add_keyword(peer_socket, id, buffer);
        break;
      case 3: /*SEARCH PHOTO*/
        printf("\nYou chose to search a photo!\nPlease insert the keyword to search for (0 to search for all the photos): " );
				scanf("%s", buffer);
				gallery_search_photo(peer_socket, buffer, &id_photos);
				break;
      case 4: /*DELETE PHOTO*/
        printf("\nYou chose to delete a photo!\nPlease insert the id of the photo to be deleted: " );
				scanf("%d", &id);
				gallery_delete_photo(peer_socket, id);
        break;
			case 5: /*GET PHOTO NAME*/
	      printf("\nYou chose to get a photo name!\nPlease insert the id of the photo to be searched: ");
				scanf("%d", &id);
				ret_val=gallery_get_photo_name(peer_socket, id, &name);
				if(ret_val!=0)
					printf("Filename found: %s\n", name);
	      break;
			case 6: /*GET PHOTO*/
		    printf("\nYou chose to get (download) a photo!\nPlease insert the id of the photo to be downloaded: ");
				scanf("%d", &id);
				gallery_get_photo(peer_socket, id, buffer);
				printf("Successfully downloaded photo %s\n", buffer);
		    break;
			case 7: /*EXIT REPOSITORY*/
				printf("Exiting...\n");
				gallery_disconnect(peer_socket);
				exit(0);
				break;
      default:
        printf("\nWe give you 7 numbers and you manage to miss! Try again\n" );
        break;
    }
	}

	pthread_mutex_destroy(&ph);
  gallery_disconnect(peer_socket);
  exit(0);
}
