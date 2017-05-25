/***************************************************************************
* PSis-project 2016-17
* by
* João Rodrigues and Sara Vieira
*
* gw.c
* Gateway process with 1 thread per server and 1 thread to communicate with the clients
****************************************************************************/

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
#include <pthread.h>

#include "gw_library.h"

int s_gw, s_client, s_sync;
LinkedList *server_list;
//int n_servers=-1;
int n_servers=0;

void terminate_ok(int n){
	int in;
  close(s_client);
	close(s_gw);
	close(s_sync);
	exit(-1);
}

/*THIS THREAD HANDLES THE SERVERS*/
void *thrd_server_fnc(void *socket){
	int type=0;
	int s_server=*(int *)(socket);

  while(1){
    recv(s_server, &type, sizeof(type), 0);
		switch(type){
			case S_CONNECT:
				server_connecting(s_server, &server_list);
				break;
			case S_ADD_PHOTO:
				gw_add_photo(s_server, &server_list);
				break;
			case S_ADD_KEYWORD:
				gw_add_keyword(s_server, &server_list);
				break;
			case S_DELETE_PHOTO:
				gw_delete_photo(s_server, &server_list);
				break;
			case S_SERVER_DEATH:
				server_disconnecting(s_server, &server_list);
				close(s_server);
				n_servers--;
				pthread_exit(NULL);
				break;
			case S_SEND_PHOTO:
				gw_send_photo(s_server);
				break;
			default:
				break;
		}
  }
}
/*THIS THREAD HANDLES THE CLIENTS*/
void *thrd_client_fnc(void *arg){
  struct sockaddr_in gw_addr, client_addr;
  socklen_t client_addr_size;
  message m;
	LinkedList *aux=NULL;

  s_client= socket(AF_INET,SOCK_DGRAM,0);
  if(s_client == -1)
    perror("s_client: Socket not created.Error:");

  printf("s_client: Socket created\n");

  gw_addr.sin_family = AF_INET;
  gw_addr.sin_port = htons(3000); /*numero de porto*/
  gw_addr.sin_addr.s_addr = INADDR_ANY; /*IP*/


  if(bind(s_client,(const struct sockaddr*)&gw_addr,sizeof(gw_addr)) == -1)
    perror("s_client: binding failed. Error:");

  printf("s_client: Bind completed\n");

  while(1){
    client_connecting(s_client, &server_list, &aux);
  }
}


/*void *thrd_sync_fnc(void *arg){
	int total, alive;
	LinkedList *aux;
	server_struct *server=NULL;
	struct sockaddr_in gw_addr;

	s_sync= socket(AF_INET,SOCK_DGRAM,0);
	if(s_sync == -1)
		perror("s_sync: Socket not created.Error:");

	printf("s_sync: Socket created\n");

	gw_addr.sin_family = AF_INET;
	gw_addr.sin_port = htons(3003);
	gw_addr.sin_addr.s_addr = INADDR_ANY;

	if(bind(s_sync,(const struct sockaddr*)&gw_addr,sizeof(gw_addr)) == -1)
		perror("s_sync: binding failed. Error:");

	printf("s_sync: Bind completed\n");

	while(1){
		alive=0;
		total=0;
		sleep(10);
		for(aux=server_list; aux!=NULL ; aux=getNextNodeLinkedList(aux)){
			server=(server_struct*) getItemLinkedList(aux);
			sendto(s_sync, &alive, sizeof(alive), 0,(struct sockaddr *) &(server->addr), (socklen_t) sizeof(server->addr));
			sleep(0.5);
			if(read(s_sync, &alive, sizeof(alive))<=0){
				deleteItemLinkedList(server_list, aux,NULL, compare_addr);
				total--;
				printf("deleting server...\n");
			}else{
				total++;
			}
		}
		printf("There are %d ALIVE servers, port=%d\n", alive, server->addr.sin_port);
	}
}*/

int main(int argc, char *argv[]){
	pthread_t thrd_sync, thrd_client;
	int error;
	struct sockaddr_in gw_addr, client_addr;
	pthread_t thrd_servers;
	server_list=initLinkedList();
  /*signal handling*/
  signal(SIGINT, terminate_ok);
	int new_s;
	/*error = pthread_create(&thrd_sync, NULL,thrd_sync_fnc, NULL);
	if(error != 0){
		perror("pthread_create: ");
		exit(-1);
	}*/

	error = pthread_create(&thrd_client, NULL,thrd_client_fnc, NULL);
	if(error != 0){
		perror("pthread_create: ");
		exit(-1);
	}

	int reuse=1;
	s_gw= socket(AF_INET,SOCK_STREAM,0);
	setsockopt(s_gw, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));
	if(s_gw == -1)
		perror("s_gw: Socket not created.Error:");

	printf("s_gw: Socket created\n");

	gw_addr.sin_family = AF_INET;
	gw_addr.sin_port = htons(3001); /*numero de porto*/
	gw_addr.sin_addr.s_addr = INADDR_ANY; /*IP*/


	if(bind(s_gw,(const struct sockaddr*)&gw_addr,sizeof(gw_addr)) == -1)
		perror("s_gw: binding failed. Error");
	printf("s_gw: Bind completed\n");

	if(listen(s_gw, MAX_SERVERS) == -1){
		perror("listen ");
		exit(-1);
	}

	while(1){
		new_s= accept(s_gw,NULL, NULL);
		n_servers++;
		printf("n_servers=%d new_s=%d\n",n_servers, new_s);
		perror("accept");
		error = pthread_create(&thrd_servers, NULL,thrd_server_fnc, (void *) &new_s);
		if(error != 0){
			perror("pthread_create: ");
			exit(-1);
		}
	}
  exit(0);

}
