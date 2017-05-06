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
int getphotoid(char * name){
  int id=0;
  int i;

  for(i=0; i<=strlen(name);i++){
    id+=name[i];
  }

  id=id*strlen(name);

  return id;
}


/*FREES EACH PHOTO*/
void free_photo(Item foto){
  free((photo *) foto);
}

int main(){

LinkedList *server_list=initLinkedList();

photo *photo1, *photo_processed, *photo2, *photo3, *photo4, *photo5;
photo *keyword_item;

photo1=(photo *) malloc(sizeof(photo));
photo_processed=(photo *) malloc(sizeof(photo));
photo2=(photo *) malloc(sizeof(photo));
photo3=(photo *) malloc(sizeof(photo));
photo4=(photo *) malloc(sizeof(photo));
photo5=(photo *) malloc(sizeof(photo));

keyword_item=(photo *) malloc(sizeof(photo));
strcpy(keyword_item->keyword[0], "nature");

strcpy(photo1->name, "profile1.png");
photo1->id=getphotoid(photo1->name);
strcpy(photo1->keyword[0], "landscape");

printf("photo_name=%s; photo_id=%d\n", photo1->name, photo1->id);

strcpy(photo2->name, "profile2.png");
photo2->id=getphotoid(photo2->name);
strcpy(photo2->keyword[0], "portrait");

printf("photo_name=%s; photo_id=%d\n", photo2->name, photo2->id);

strcpy(photo3->name, "profile3.png");
photo3->id=getphotoid(photo3->name);
strcpy(photo3->keyword[0], "portrait");

printf("photo_name=%s; photo_id=%d\n", photo3->name, photo3->id);

strcpy(photo4->name, "profile4.png");
photo4->id=getphotoid(photo4->name);
strcpy(photo4->keyword[0], "nature");

printf("photo_name=%s; photo_id=%d\n", photo4->name, photo4->id);

strcpy(photo5->name, "profile5.png");
photo5->id=getphotoid(photo5->name);
strcpy(photo5->keyword[0], "beach");

printf("photo_name=%s; photo_id=%d\n", photo5->name, photo5->id);

server_list=insertUnsortedLinkedList(server_list,(Item) photo1);
server_list=insertUnsortedLinkedList(server_list,(Item) photo2);
server_list=insertUnsortedLinkedList(server_list,(Item) photo3);
server_list=insertUnsortedLinkedList(server_list,(Item) photo4);
server_list=insertUnsortedLinkedList(server_list,(Item) photo5);

photo_processed=(photo*) findItemLinkedList(server_list, (Item) photo3, &compare_id);
printf("\nphoto_name=%s; photo_id=%d\n", photo_processed->name, photo_processed->id);

photo ** vector= (photo **) findItemVectorLinkedList(server_list, (Item) keyword_item, &compare_keywords);

for(int i=0; i<(sizeof(vector)/sizeof(Item)); i++)
  printf("\nphoto_name=%s; photo_id=%d\n", vector[i]->name, vector[i]->id);



freeLinkedList(server_list, &free_photo);

return 0;
}
