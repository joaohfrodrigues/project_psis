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

int s_server, s_client, s_sync;
LinkedList *server_list;

void terminate_ok(int n){
	int in;
  close(s_client);
	close(s_server);
	close(s_sync);
	exit(-1);
}

void *thrd_server_fnc(void *arg){
  struct sockaddr_in gw_addr, server_addr;
  socklen_t server_addr_size;
  message m;
	int type=0;

  s_server= socket(AF_INET,SOCK_DGRAM,0);
  if(s_server == -1)
    perror("s_server: Socket not created.Error:");

  printf("s_server: Socket created\n");

  gw_addr.sin_family = AF_INET;
  gw_addr.sin_port = htons(3001); /*numero de porto*/
  gw_addr.sin_addr.s_addr = INADDR_ANY; /*IP*/


  if(bind(s_server,(const struct sockaddr*)&gw_addr,sizeof(gw_addr)) == -1)
    perror("s_server: binding failed. Error:");

  printf("s_server: Bind completed\n");

  while(1){
    recvfrom(s_server, &type, sizeof(type), 0,(struct sockaddr *) &server_addr, &server_addr_size);
		printf("PORTO=%d\n", server_addr.sin_port);
		if(type==S_CONNECT){
			server_connecting(s_server, &server_list, server_addr);
		}else if(type==S_ADD_PHOTO){
			gw_add_photo(s_server, &server_list);
		}else if(type==S_ADD_KEYWORD){
			gw_add_keyword(s_server, &server_list);
		}
  }
}

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


void *thrd_sync_fnc(void *arg){
	int total, alive;
	LinkedList *aux;
	server_struct *server=NULL;
	struct sockaddr_in gw_addr;

	s_sync= socket(AF_INET,SOCK_DGRAM,0);
	if(s_sync == -1)
		perror("s_sync: Socket not created.Error:");

	printf("s_sync: Socket created\n");

	gw_addr.sin_family = AF_INET;
	gw_addr.sin_port = htons(3002); /*numero de porto*/
	gw_addr.sin_addr.s_addr = INADDR_ANY; /*IP*/


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
				deleteItemLinkedList(server_list, aux, compare_addr);
				total--;
				printf("deleting server...\n");
			}else{
				total++;
			}
		}
		printf("There are %d ALIVE servers, port=%d\n", alive, server->addr.sin_port);
	}
}

int main(int argc, char *argv[]){
	pthread_t thrd_sync, thrd_client, thrd_server;
	int error;

	server_list=initLinkedList();
  /*signal handling*/
  signal(SIGINT, terminate_ok);

	error = pthread_create(&thrd_server, NULL,thrd_server_fnc, NULL);
  if(error != 0){
    perror("pthread_create: ");
    exit(-1);
  }

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

	while(1) {
	}
  exit(0);

}
