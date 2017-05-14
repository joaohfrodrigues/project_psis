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

int s_gw, s_server;
struct sockaddr_in server_addr, gateway_addr, client_addr, sgw_addr;
LinkedList *photo_list;
server_struct gateway_message;
int n_clients=0;
int s_clients[MAX_CLIENTS];
int new_s;
//socklen_t gateway_addr_size;

/*void *sync_fnc(void *arg){
  while(1){
    int alive;
    recv(s_gw, &alive, sizeof(alive), 0);
    alive=1;
    sendto(s_gw, (const void *) &alive, (size_t) sizeof(alive), 0,(const struct sockaddr *) &gateway_addr, (socklen_t) sizeof(gateway_addr));
  }
}*/

void *handle_client(void *arg){

  int type=0;
  int handle_client_type=0;
  message m;
  photo_struct photo;
  //n_clients++; /*SELECT THE CORRECT SOCKET AND INCREMENT THE NUMBER OF CLIENTS*/
  printf("New thread ID %d %lu\n",getpid(), pthread_self()) ;
  while(1){
    /*receive message*/
    recv(new_s, &type, sizeof(type), 0);
    /* process message */
    if(type==ADD_PHOTO){ /*WHAT TO DO WHEN THE CLIENT WANTS TO ADD A PHOTO*/
      handle_client_type=S_ADD_PHOTO;
      recv(new_s, &photo, sizeof(photo), 0);
      sendto(s_gw, (const void *) &handle_client_type, (size_t) sizeof(handle_client_type), 0,(const struct sockaddr *) &gateway_addr, (socklen_t) sizeof(gateway_addr));
      sendto(s_gw, (const void *) &photo, (size_t) sizeof(photo), 0,(const struct sockaddr *) &gateway_addr, (socklen_t) sizeof(gateway_addr));
      sendto(s_gw, (const void *) &sgw_addr.sin_port, (size_t) sizeof(sgw_addr.sin_port), 0,(const struct sockaddr *) &gateway_addr, (socklen_t) sizeof(gateway_addr));
      //server_add_photo(new_s, &photo_list);
    }else if(type==ADD_KEYWORD){ /*WHAT TO DO WHEN THE CLIENT WANTS TO ADD A KEYWORD*/
      handle_client_type=S_ADD_KEYWORD;
      recv(new_s, &m, sizeof(m), 0);
      sendto(s_gw, (const void *) &handle_client_type, (size_t) sizeof(handle_client_type), 0,(const struct sockaddr *) &gateway_addr, sizeof(gateway_addr));
      sendto(s_gw, (const void *) &m, (size_t) sizeof(m), 0,(const struct sockaddr *) &gateway_addr, sizeof(gateway_addr));
      sendto(s_gw, (const void *) &sgw_addr.sin_port, (size_t) sizeof(sgw_addr.sin_port), 0,(const struct sockaddr *) &gateway_addr, (socklen_t) sizeof(gateway_addr));
      //server_add_keyword(new_s, photo_list);
    }else if(type==SEARCH_PHOTO){

    }else if(type==SEARCH_KEYWORD){

    }else if(type==DELETE_PHOTO){

    }else if(type==CLIENT_DEATH){
      //n_clients--;
      printf("closing thread %lu\n", pthread_self());
      pthread_exit(NULL);
    }

    //free(new_ph);
  }
}

/*THREAD USED TO COMMUNICATE WITH THE GATEWAY*/
void *gw_connection(void *arg){
  struct sockaddr_in src_addr;
  socklen_t src_addr_size;
  int gw_connection_type=0;
  int gw_m_type=S_CONNECT;
  char test[MESSAGE_LEN];
  LinkedList *aux=NULL;
  photo_struct photo;
  strcpy(test, inet_ntoa(gateway_message.addr.sin_addr));
  sendto(s_gw, (const void *) &gw_m_type, (size_t) sizeof(gw_m_type), 0,(const struct sockaddr *) &gateway_addr, (socklen_t) sizeof(gateway_addr));
  sendto(s_gw, (const void *) &(gateway_message), (size_t) sizeof(gateway_message), 0,(const struct sockaddr *) &gateway_addr, (socklen_t)sizeof(gateway_addr));
  while(1){
    recv(s_gw, &gw_connection_type, sizeof(gw_connection_type), 0);
    if(gw_connection_type==ADD_PHOTO){
      printf("gw->server:add photo\n");
      server_add_photo(s_gw, new_s, &photo_list);
    }else if(gw_connection_type==ADD_KEYWORD){
      server_add_keyword(s_gw, new_s, photo_list);
    }else if(gw_connection_type==SYNC_PHOTO_LIST){
      for(aux=photo_list; aux!=NULL; aux=getNextNodeLinkedList(aux)){
        photo= *((photo_struct *) getItemLinkedList(aux));
        sendto(s_gw, (const void *) &photo, (size_t) sizeof(photo), 0,(const struct sockaddr *) &gateway_addr, (socklen_t)sizeof(gateway_addr));
      }
    }
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
  close(s_server);
  for(int i=0; i<n_clients; i++)
    close(s_clients[n_clients]);
  //freeLinkedList(photo_list, &free_photo);
	exit(-1);
	//}
}

int main(int argc, char *argv[]){
  int error;
  pthread_t thrd_client[MAX_CLIENTS];
  pthread_t thrd_sync, thrd_gw;
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
  printf("Socket gateway created, number %d\n", s_gw);

  gateway_addr.sin_family = AF_INET;
  gateway_addr.sin_port = htons(3001); /*numero de porto*/
  inet_aton(argv[1], &gateway_addr.sin_addr);
  //gateway_addr.sin_addr.s_addr = INADDR_ANY; /*IP*/

  sgw_addr.sin_family = AF_INET;
  sgw_addr.sin_port = htons(3100+getpid());; /*numero de porto*/
  inet_aton(argv[1], &sgw_addr.sin_addr);

  if(bind(s_gw,(const struct sockaddr*)&sgw_addr,sizeof(sgw_addr)) == -1)
  {
    perror("binding failed. Error:");
    return 1;
  }


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

  /*gateway_message.type=S_CONNECT;
  gateway_message.port=server_addr.sin_port;
  strcpy(gateway_message.buffer, inet_ntoa(gateway_addr.sin_addr));*/

    /*bind the socket server-client*/
  if(bind(s_server,(const struct sockaddr*)&server_addr,sizeof(server_addr)) == -1)
  {
    perror("binding failed. Error:");
    return 1;
  }
  printf("Bind completed\n");

  gateway_message.addr=server_addr;
  gateway_message.sgw_addr=sgw_addr;
  inet_aton(argv[1], &gateway_message.addr.sin_addr);

  error = pthread_create(&thrd_gw, NULL,gw_connection, NULL);
  if(error != 0){
    perror("pthread_create: ");
    exit(-1);
  }

  if(listen(s_server, MAX_CLIENTS) == -1){
    perror("listen ");
    exit(-1);
  }

  while(1){
    new_s= accept(s_server,NULL, NULL);
    perror("accept");
    error = pthread_create(&thrd_client[n_clients], NULL,handle_client, NULL);
  	if(error != 0){
  		perror("pthread_create: ");
  		exit(-1);
  	}
  }
  printf("OK\n");
  exit(0);

}
