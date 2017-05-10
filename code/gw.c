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

int s;
LinkedList *server_list;

int compare_addr(Item addr1, Item addr2){
	if(((server_struct *)addr1)->addr.sin_port==((server_struct *)addr2)->addr.sin_port)
    return 1;
  else
    return 0;
}

void *sync_servers(void *arg){
	int total, alive;
	LinkedList *aux;
	server_struct *server=NULL;
	while(1){
		alive=0;
		total=0;
		sleep(10);
		for(aux=server_list; aux!=NULL ; aux=getNextNodeLinkedList(aux)){
			server=(server_struct*) getItemLinkedList(aux);
			sendto(s, &alive, sizeof(alive), 0,(struct sockaddr *) &(server->addr), (socklen_t) sizeof(server->addr));
			if(recv(s, &alive, sizeof(alive), 0)<=0){
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


void terminate_ok(int n){
	int in;
	//printf("received signal %d, do you want to exit? (0/1)", n);
	//scanf("%d", &in);
	//if(in==0){
	//	return;
	//}else{
    close(s);
		exit(-1);
	//}
}

int main(int argc, char *argv[]){

  message m;
  struct sockaddr_in gw_addr, src_addr;
  socklen_t gw_addr_size, src_addr_size;
	int server_port=0;
  struct sockaddr_in server_addr;
  socklen_t server_addr_size;
	pthread_t thrd_sync;
	int error;

	server_list=initLinkedList();
  /*signal handling*/
  signal(SIGINT, terminate_ok);


  /* create socket  */
  s= socket(AF_INET,SOCK_DGRAM,0);
  if(s == -1)
  {
    perror("Socket not created.Error:");
    return 1;
  }
  printf("Socket created\n");

  gw_addr.sin_family = AF_INET;
  gw_addr.sin_port = htons(3000); /*numero de porto*/
  gw_addr.sin_addr.s_addr = INADDR_ANY; /*IP*/

  /*bind the socket gatway*/
  if(bind(s,(const struct sockaddr*)&gw_addr,sizeof(gw_addr)) == -1)
  {
    perror("binding failed. Error:");
    return 1;
  }
  printf("Bind completed\n");

	/*error = pthread_create(&thrd_sync, NULL,sync_servers, NULL);
	if(error != 0){
		perror("pthread_create: ");
		exit(-1);
	}*/


	while(1) {
		recvfrom(s, &m, sizeof(m), 0,(struct sockaddr *) &src_addr, &src_addr_size);
		//recv(s, &m, sizeof(m), 0);
		printf("received\n");

		if(m.type==GW_SERVER){
			server_connecting(s, &server_list, m, src_addr);
		}else if(m.type==SERVER_DEATH){
			printf("gateway received death message\n");
		}else if(m.type==CLIENT_GW){
			client_connecting(s, server_list, src_addr);
		}
	}

  close(s);
  //unlink(client_addr_id);
  exit(0);

}
