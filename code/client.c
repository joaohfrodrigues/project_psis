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
	int id=0;
	char *name=NULL;

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

	for(int i=0; i<4; i++){
		printf("ADDING PHOTOS\nfilename to add: ");
	  //fgets(m.buffer, MESSAGE_LEN, stdin);
		scanf("%s", buffer);
	  /* write message */
	  gallery_add_photo(peer_socket, buffer);
	}

	printf("GET PHOTO NAME\nid to search: ");
	//fgets(m.buffer, MESSAGE_LEN, stdin);
	scanf("%d", &id);
	gallery_get_photo_name(peer_socket, id, &name);

	printf("DELETING PHOTO\nid to search: ");
	//fgets(m.buffer, MESSAGE_LEN, stdin);
	scanf("%d", &id);
	gallery_delete_photo(peer_socket, id);


	for(int i=0; i<5; i++){
		printf("ADDING KEYWORDS\nphotoid(number >0): ");

		scanf("%d", &id);

		while(id<=0){
			printf("error: id not valid (number >0). Insert id: ");
			scanf("%d", &id);
			getchar();
		}

		printf("id=%d	keyword to add: ", id);
	  //fgets(m.buffer, MESSAGE_LEN, stdin);
		scanf("%s", buffer);
	  /* write message */
	  gallery_add_keyword(peer_socket, (uint32_t) id, buffer);
	}

	int nr_photos;
	uint32_t *id_vector = NULL;
	for(int i=0; i<3; i++){
		printf("SEARCHING PHOTOS\nkeyword: ");

		scanf("%s", buffer);

		while(buffer=="\0"){
			printf("error: keyword not valid. Insert keyword: ");
			scanf("%s", buffer);
			getchar();
		}

	  	nr_photos = gallery_search_photo(peer_socket, buffer, &id_vector);
	  	printf("%d photos found related to keyword %s\n", nr_photos, buffer);
			for(int i=0; i<nr_photos; i++){
				printf("id=%d\n", id_vector[i]);
			}
	}

  gallery_disconnect(peer_socket);
  exit(0);
}
