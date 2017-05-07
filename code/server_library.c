/***************************************************************************
* PSis-project 2016-17
* by
* João Rodrigues and Sara Vieira
*
* server_library.c
* Functions available for the server to use
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
#include <time.h>

#include "image_server.h"
#include "server_library.h"

/*COMPARES THE ID OF PHOTO 1 WITH THE DETERMINED ID OF PHOTO 2*/
int compare_id(Item foto1, Item foto2){
  if(((photo *)foto1)->id==((photo *)foto2)->id)
    return 1;
  else
    return 0;
}

/*COMPARES THE KEYWORDS OF PHOTO 1 WITH A SINGLE KEYWORD OF PHOTO 2*/
int compare_keywords(Item foto1, Item foto2){
  for(int i=0; i<MAX_KEYWORDS; i++){
    if(strcmp(((photo *)foto1)->keyword[i],((photo *)foto2)->keyword[0])==0)
      return 1;
  }
  return 0;
}

/*DETERMINES THE ID OF A PHOTO*/ /*NEEDS IMPROVEMENT*/
uint32_t getphotoid(char * name){
  uint32_t id=0;
  int i;
  srand(time(NULL));   // should only be called once
  for(i=0; i<=strlen(name);i++){
    id+=name[i];
  }
  id=id*strlen(name)+rand();
  return id;
}


/*FREES EACH PHOTO*/
void free_photo(Item foto){
  free((photo *) foto);
}
