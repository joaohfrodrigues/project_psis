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

void server_add_photo(int s_gw, LinkedList ** photo_list, int server_port){
  photo_struct photo;
  FILE *dest_file;
  char c;
  int i=0;
  char name[MESSAGE_LEN];

  recv(s_gw, &photo, sizeof(photo), 0);
  photo_struct *new_ph=(photo_struct *) malloc(sizeof(photo_struct));
  printf("adding photo, filename=%s;  id=%d;\n", photo.name, photo.id);

  strcpy(new_ph->name, photo.name);
  new_ph->id=photo.id;
  new_ph->nkey=photo.nkey;

  for(int i=0; i<photo.nkey; i++){
    strcpy(new_ph->keyword[i], photo.keyword[i]);
  }


  (*photo_list)=insertUnsortedLinkedList((*photo_list), (Item) new_ph);

  if(photo.source==server_port){
    send(photo.s_client, &photo, sizeof(photo), 0);
  }

  //itoa(photo.id, name, 10);
  sprintf(name,"%d",photo.id); /*converts to decimal base*/
  strcat(name, photo.name);
  printf("photo_name in the database = %s\n", name);
  dest_file=fopen(name, "wb");
  for(i=0; i< photo.size; i++){
    recv(s_gw, &c, sizeof(c), 0);
    fputc(c, dest_file);
  }
  fclose(dest_file);

}

void server_delete_photo(int s_gw, LinkedList ** photo_list, int server_port){
  message m;
  photo_struct photo;
  recv(s_gw, &m, sizeof(m), 0);

  photo.id=m.port;
  printf("deleting photo\n", m.port);
  m.port=1;
  (*photo_list)=deleteItemLinkedList((*photo_list), (Item) &photo, &m.port, &compare_id, &free_photo);

  if(m.source==server_port){
    send(m.s_client, &m.port, sizeof(m.port), 0);
  }
}

void server_add_keyword(int s_gw, LinkedList * photo_list, int server_port){
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
      //free(new_key);
    }
  printf("id=%d name=%s nkey=%d keyword=%s\n",new_key->id, new_key->name, new_key->nkey, new_key->keyword[new_key->nkey-1]);
  }
  if(m.source==server_port){
    send(m.s_client, &m, sizeof(m), 0);
  }
}

void server_get_photo_name(int s_client, LinkedList * photo_list){
  message m;
  recv(s_client, &m, sizeof(m), 0);
  photo_struct photo;
  photo_struct *found_photo;

  photo.id=m.port;
  found_photo= (photo_struct*) findItemLinkedList(photo_list, (Item) &photo, &compare_id);

  if(found_photo==NULL){
    m.port=-2;
    send(s_client, &m, sizeof(m), 0);
    return;
  }

  strcpy(m.buffer, found_photo->name);
  send(s_client, &m, sizeof(m), 0);
}

void server_get_photo(int s_client, LinkedList * photo_list){
  photo_struct photo;
  photo_struct *found_photo;
  FILE *src_file;
  struct stat file_stat;
  char name[MESSAGE_LEN];
  char c;
  int i;

  recv(s_client, &photo, sizeof(photo), 0);

  found_photo= (photo_struct*) findItemLinkedList(photo_list, (Item) &photo, &compare_id);

  if(found_photo==NULL){
    photo.id=-2;
    send(s_client, &photo, sizeof(photo), 0);
    return;
  }
  sprintf(name,"%d",found_photo->id); /*converts to decimal base*/
  strcat(name, found_photo->name);

  src_file=fopen(name, "rb");

  printf("File Size: \n%d bytes\n", found_photo->size);

  send(s_client, found_photo, sizeof(*found_photo), 0);

  for(i=0; i<found_photo->size; i++){
    c=fgetc(src_file);
    send(s_client, &c, sizeof(c), 0);
  }
  fclose(src_file);
}

void server_search_photo(int s, LinkedList * photo_list){
  message m;
  photo_struct photo;
  recv(s, &m, sizeof(m), 0);
  int count=0;
  int i=0;
  uint32_t *photos_id = NULL;

  strcpy(photo.keyword[0], m.buffer);
  photo_struct ** vector= (photo_struct **) findItemVectorLinkedList(photo_list, (Item) &photo, &compare_keywords, &count);
  printf("found %d matches for keyword %s\n", count, photo.keyword[0]);

  send(s, &count, sizeof(count), 0);

  if(count != 0)
    photos_id = (uint32_t*) calloc(count, sizeof(uint32_t));

  if(vector==NULL){
    printf("nothing found for keyword %s\n", m.buffer);
  }else{
    for(i=0; i<count; i++){
      printf("photo_name=%s; photo_id=%d, i=%d\n", vector[i]->name, vector[i]->id, i);
      photos_id[i]= (uint32_t) vector[i]->id;
      send(s, &(photos_id[i]), sizeof(photos_id[i]), 0);
    }
    free(vector);
  }
  free(photos_id);
}
