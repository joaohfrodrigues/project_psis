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

int s, s_dgram;

int main(int argc, char *argv[]){

  message m, gateway_message;
  char client_addr_id[20];

  struct sockaddr_in server_addr, client_addr, gateway_addr;
  socklen_t server_addr_size, client_addr_size, gateway_addr_size;


  sprintf(client_addr_id, "sock_cli_%d", getpid());

  s=gallery_connect(argv[1], 3000);

  printf("message: ");
  fgets(m.buffer, MESSAGE_LEN, stdin);

  /* write message */
  send(s, &m, sizeof(m), 0);
  printf("OK\n");

  close(s);
  //unlink(client_addr_id);
  exit(0);

}
