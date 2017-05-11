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


#include "server_library.h"

/*COMPARES THE ID OF PHOTO 1 WITH THE DETERMINED ID OF PHOTO 2*/
int compare_id(Item foto1, Item foto2){
  if(((photo_struct *)foto1)->id==((photo_struct *)foto2)->id)
    return 1;
  else
    return 0;
}

/*COMPARES THE KEYWORDS OF PHOTO 1 WITH A SINGLE KEYWORD OF PHOTO 2*/
int compare_keywords(Item foto1, Item foto2){
  for(int i=0; i<MAX_KEYWORDS; i++){
    if(strcmp(((photo_struct *)foto1)->keyword[i],((photo_struct *)foto2)->keyword[0])==0)
      return 1;
  }
  return 0;
}


/*FREES EACH PHOTO*/
void free_photo(Item foto){
  free((photo_struct *) foto);
}

void server_add_photo(int s_gw, int s_client, LinkedList ** photo_list){
  message m;
  recv(s_gw, &m, sizeof(m), 0);
  photo_struct *new_ph=(photo_struct *) malloc(sizeof(photo_struct));
  printf("adding photo, filename=%s;  id=%d;\n", m.buffer, m.port);

  strcpy(new_ph->name, m.buffer);
  new_ph->id=m.port;
  new_ph->nkey=0;
  (*photo_list)=insertUnsortedLinkedList((*photo_list), (Item) new_ph);
  send(s_client, &m, sizeof(m), 0);
}

void server_add_keyword(int s_gw, int s_client, LinkedList * photo_list){
  message m;
  recv(s_gw, &m, sizeof(m), 0);
  int i=0;
  int test_value=0;
  photo_struct *new_key=(photo_struct *) malloc(sizeof(photo_struct));
  new_key->id=m.port;
  new_key = (photo_struct *) findItemLinkedList(photo_list, (Item) new_key, &compare_id);

  if(new_key==NULL){
    m.port=-2;
  }else{
    printf("id=%d name=%s nkey=%d\n",new_key->id, new_key->name, new_key->nkey);
    if(new_key->nkey==0){
      strcpy(new_key->keyword[0], m.buffer);
      new_key->nkey=1;
      m.port=1;
    }else if(new_key->nkey==MAX_KEYWORDS){
      m.port=-1;
    }else{
      for(i=0;i<new_key->nkey; i++){
        if(strcmp(m.buffer, new_key->keyword[i])==0){
          m.port=0;
          test_value=1;
          break;
        }
      }
      if(test_value==0){
        strcpy(new_key->keyword[new_key->nkey], m.buffer);
        new_key->nkey++;
        m.port=1;
      }
    }
  }
  send(s_client, &m, sizeof(m), 0);
}
