#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <pthread.h>

//#include "image_server.h"
#include "server_library.h"

int s_gw, new_s, s_server;
int ret;
message gateway_message;
struct sockaddr_in gateway_addr, client_addr;
LinkedList *photo_list;
//socklen_t gateway_addr_size;

void *handle_client(void *arg){

  message m;
  printf("New thread ID %d %lu\n",getpid(), pthread_self()) ;
  while(1){
    /*receive message*/
    recv(new_s, &m, sizeof(m), 0);
    /* process message */
    if(m.type==ADD_PHOTO){ /*WHAT TO DO WHEN THE CLIENT WANTS TO ADD A PHOTO*/
      server_add_photo(new_s, m, &photo_list);
    }else if(m.type==ADD_KEYWORD){ /*WHAT TO DO WHEN THE CLIENT WANTS TO ADD A KEYWORD*/
      server_add_keyword(new_s, m, photo_list);
    }else if(m.type==SEARCH_PHOTO){

    }else if(m.type==SEARCH_KEYWORD){

    }else if(m.type==DELETE_PHOTO){

    }else if(m.type==CLIENT_DEATH){
      printf("closing thread %lu\n", pthread_self());
      pthread_exit(NULL);
    }

    //free(new_ph);
  }
}

/*HANDLING SIGNALS*/
void terminate_ok(int n){
	//int in;
	//printf("received signal %d, do you want to exit? (0/1)", n);
	//scanf("%d", &in);
	//if(in==0){
	//	return;
	//}else{
  close(s_gw);
  close(new_s);
  close(s_server);
  //freeLinkedList(photo_list, &free_photo);
	exit(-1);
	//}
}

int main(int argc, char *argv[]){
  message gateway_message;
  struct sockaddr_in server_addr, gateway_addr;
  socklen_t server_addr_size, client_addr_size, gateway_addr_size;
  int error;
  pthread_t thrd_client[MAX_CLIENTS];
  int i=0;

  photo_list=initLinkedList();

  /*signal handling*/
  signal(SIGINT, terminate_ok);

  /* create socket gateway*/
  s_gw = socket(AF_INET,SOCK_DGRAM,0);
  if(s_gw == -1)
  {
    perror("Socket gateway not created.Error:");
    return 1;
  }
  printf("Socket gateway created\n");

  gateway_addr.sin_family = AF_INET;
  gateway_addr.sin_port = htons(3000); /*numero de porto*/
  inet_aton(argv[1], &gateway_addr.sin_addr);
  //gateway_addr.sin_addr.s_addr = INADDR_ANY; /*IP*/

  /* create socket client*/
  s_server= socket(AF_INET,SOCK_STREAM,0);
  if(s_server == -1)
  {
    perror("Socket client not created.Error:");
    return 1;
  }
  printf("Socket client created\n");

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(3000+getpid()); /*numero de porto*/
  server_addr.sin_addr.s_addr = INADDR_ANY; /*IP*/

  gateway_message.type=GW_SERVER;
  gateway_message.port=server_addr.sin_port;

  gateway_addr_size=sizeof(gateway_addr);
  printf("server_port=%d\n", server_addr.sin_port);
  sendto(s_gw, (const void *) &gateway_message, (size_t) sizeof(gateway_message), 0,(const struct sockaddr *) &gateway_addr, (socklen_t) gateway_addr_size);

    /*bind the socket server-client*/
  if(bind(s_server,(const struct sockaddr*)&server_addr,sizeof(server_addr)) == -1)
  {
    perror("binding failed. Error:");
    return 1;
  }
  printf("Bind completed\n");

  if(listen(s_server, MAX_CLIENTS) == -1){
    perror("listen ");
    exit(-1);
  }

  i=0;
  while(1){
    new_s= accept(s_server,NULL, NULL);
    perror("accept");
    error = pthread_create(&thrd_client[i], NULL,handle_client, NULL);
  	if(error != 0){
  		perror("pthread_create: ");
  		exit(-1);
  	}
    i++;
  }
  printf("OK\n");
  exit(0);

}
