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
#include <string.h>

#include "image_server.h"
#include "list.h"

int getphotoid(char * name){
  int id=0;
  int i;

  for(i=0; i<=strlen(name);i++){
    id+=name[i];
  }
  return id;
}

void free_photo(Item foto)
{
  free((photo *) foto);
}

int main(){

LinkedList *server_list=initLinkedList();

photo *photo1, *photo1_processed;

photo1=(photo *) malloc(sizeof(photo));
photo1_processed=(photo *) malloc(sizeof(photo));

strcpy(photo1->name, "profile.png");
photo1->id=getphotoid(photo1->name);

server_list=insertUnsortedLinkedList(server_list,(Item) photo1);

photo1_processed=(photo*) getItemLinkedList(server_list);

printf("photo1_name=%s; photo_id=%d\n", photo1_processed->name, photo1_processed->id);

freeLinkedList(server_list, &free_photo);

return 0;
}
