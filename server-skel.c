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

#include "image_server.h"

int s_gw, new_s, s_server;


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
		exit(-1);
	//}
}

int main(int argc, char *argv[]){

  message m, gateway_message;
  struct sockaddr_in server_addr, client_addr, gateway_addr;
  socklen_t server_addr_size, client_addr_size, gateway_addr_size;

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

  gateway_message.type=GW_SERVER;
  gateway_message.port=server_addr.sin_port;

  gateway_addr_size=sizeof(gateway_addr);
  printf("message type %d\n", gateway_message.type);
  int bits=sendto(s_gw, (const void *) &gateway_message, (size_t) sizeof(gateway_message), 0,(const struct sockaddr *) &gateway_addr, (socklen_t) gateway_addr_size);

  printf("sent%d bits\n", bits);


    /* create socket client*/
    s_server= socket(AF_INET,SOCK_STREAM,0);
    if(s_client == -1)
    {
      perror("Socket client not created.Error:");
      return 1;
    }
    printf("Socket client created\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(3000+getpid()); /*numero de porto*/
    server_addr.sin_addr.s_addr = INADDR_ANY; /*IP*/

    /*bind the socket server-client*/
    if(bind(s_server,(const struct sockaddr*)&server_addr,sizeof(server_addr)) == -1)
    {
      perror("binding failed. Error:");
      return 1;
    }
    printf("Bind completed\n");


    if(listen(s_server, 10) == -1){
      perror("listen ");
      exit(-1);
    }

    while(1){
      new_s= accept(s_server,NULL, NULL);
      perror("accept");
      /*receive message*/
      recv(new_s, &m, sizeof(m), 0);
      /* process message */
      printf("%s\n", m.type);
    }
    printf("OK\n");
    exit(0);

}
