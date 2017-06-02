/***************************************************************************
* PSis-project 2016-17
* by
* João Rodrigues and Sara Vieira
*
* library.c
* Functions available for the client to use
****************************************************************************/

#include "library.h"

int gallery_connect(char * host, in_port_t port){
  int s, s_dgram;
  message m, gateway_message;
  struct sockaddr_in server_addr, gateway_addr;
  s_dgram= socket(AF_INET,SOCK_DGRAM,0);
  if(s_dgram == -1)
  {
    perror("Socket with gateway not created.Error:");
    return -1;
  }

  gateway_addr.sin_family = AF_INET;
  gateway_addr.sin_port = htons(port);
  inet_aton(host, &gateway_addr.sin_addr);

  gateway_message.type=CLIENT_GW;
  strcpy(gateway_message.buffer, inet_ntoa(gateway_addr.sin_addr));
  sendto(s_dgram, (const void *) &gateway_message, (size_t) sizeof(gateway_message), 0,(const struct sockaddr *) &gateway_addr, (socklen_t) sizeof(gateway_addr));
  recv(s_dgram, &m, sizeof(m), 0);

  if(m.port==0){
    return 0;
  }

  printf("server_ip: %s\n", m.buffer);
  printf("server_port:%d\n", m.port);

  s= socket(AF_INET,SOCK_STREAM,0);
  if(s == -1)
  {
    perror("Socket not created.Error:");
    return -1;
  }
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = m.port;
  inet_aton(m.buffer, &server_addr.sin_addr);

  if(connect(s, (struct sockaddr *) &server_addr, sizeof(server_addr))==-1){
    perror("connect ");
  }

  return s;
}

uint32_t gallery_add_photo(int peer_socket, char *file_name){
  photo_struct photo;
  int type=ADD_PHOTO;
  FILE *photo_file;
  strcpy(photo.name, file_name);
  photo.id=0;
  char c;
  int i=0;
  struct stat file_stat;

  photo_file=fopen(file_name, "rb");

  if(photo_file==NULL){
    fprintf(stderr, "Error opening file --> %s\n", strerror(errno));
    return 0;
  }
  int fd=fileno(photo_file);
  if (fd == -1){
    fprintf(stderr, "Error opening file --> %s\n", strerror(errno));
    fclose(photo_file);
    return 0;
  }else{
    if (fstat(fd, &file_stat) < 0){
      fprintf(stderr, "Error fstat --> %s", strerror(errno));
       exit(EXIT_FAILURE);
    }
    photo.size=file_stat.st_size;

    send(peer_socket, &type, sizeof(type), 0);
    send(peer_socket, &photo, sizeof(photo), 0);

    for(i=0; i<file_stat.st_size; i++){
      c=fgetc(photo_file);
      send(peer_socket, &c, sizeof(c), 0);
    }
  }
  fclose(photo_file);

  recv(peer_socket, &photo, sizeof(photo), 0);
  return photo.id;
}

int gallery_add_keyword(int peer_socket, uint32_t id_photo, char *keyword){
  message m;
  int type=ADD_KEYWORD;
  strcpy(m.buffer, keyword);
  m.port=id_photo;

  if(id_photo <= 0){
    printf("not a valid id (number > 0)\n");
    return -1;
  }

  send(peer_socket, &type, sizeof(type), 0);
  send(peer_socket, &m, sizeof(m), 0);
  recv(peer_socket, &m, sizeof(m), 0);

  if(m.port==1)
    printf("added keyword <%s>\n", m.buffer);
  else if(m.port==0)
    printf("keyword <%s> already in use\n", m.buffer);
  else if(m.port==-1)
    printf("no more space for new keywords\n");
  else if(m.port==-2)
    printf("id not recognized\n");

  return m.port;
}

int gallery_get_photo_name(int peer_socket, uint32_t id_photo, char **photo_name){
  message m;
  int type=GET_PHOTO_NAME;

  m.port=id_photo;

  if(id_photo <= 0){
    printf("not a valid id (number > 0)\n");
    return -1;
  }

  send(peer_socket, &type, sizeof(type), 0);
  send(peer_socket, &m, sizeof(m), 0);
  recv(peer_socket, &m, sizeof(m), 0);

  if(m.port==-2){
    printf("id not recognized or not present\n");
    return 0;
  }
  (*photo_name)=(char *) malloc(strlen(m.buffer));
  strcpy((*photo_name), m.buffer);

  return m.port;
}

int gallery_get_photo(int peer_socket, uint32_t id_photo, char *file_name){
  photo_struct photo;
  int type=GET_PHOTO;
  FILE *dest_file;
  char c;
  int i;

  photo.id=id_photo;

  if(id_photo <= 0){
    printf("not a valid id (number > 0)\n");
    return -1;
  }

  send(peer_socket, &type, sizeof(type), 0);
  send(peer_socket, &photo, sizeof(photo), 0);
  recv(peer_socket, &photo, sizeof(photo), 0);

  if(photo.id==-2){
    printf("id not recognized or not present\n");
    return 0;
  }

  printf("got %s\n", photo.name);
  strcpy(file_name, photo.name);

  dest_file=fopen(file_name, "wb");
  for(i=0; i< photo.size; i++){
    recv(peer_socket, &c, sizeof(c), 0);
    fputc(c, dest_file);
  }
  fclose(dest_file);


  return 1;
}

int gallery_delete_photo(int peer_socket, uint32_t id_photo){
  message m;
  int type=DELETE_PHOTO;
  int ret_val=0;

  if(id_photo <= 0){
    printf("not a valid id (number > 0)\n");
    return -1;
  }

  m.port=id_photo;
  printf("deleting photo id: %d\n", m.port);
  send(peer_socket, &type, sizeof(type), 0);
  send(peer_socket, &m, sizeof(m), 0);
  recv(peer_socket, &ret_val, sizeof(ret_val), 0);

  if(ret_val==1){
    printf("deleted photo with id=%d\n", id_photo);
  }else{
    printf("id not recognized\n");
  }

  return ret_val;
}


int gallery_search_photo(int peer_socket, char * keyword, uint32_t ** id_photos){
  message m;
  int type=SEARCH_PHOTO;
  strcpy(m.buffer, keyword);
  int n_photos=0;
  uint32_t id;
  int i=0;
  photo_struct photo;

  send(peer_socket, &type, sizeof(type), 0);
  send(peer_socket, &m, sizeof(m), 0);
  recv(peer_socket, &n_photos, sizeof(n_photos), 0);
  (*id_photos) = (uint32_t*) calloc(n_photos, sizeof(uint32_t));

  if(strcmp("0",keyword)!=0)
    printf("Found %d matches for keyword %s\n", n_photos, keyword);
  else
    printf("There are %d photos in the repository:\n", n_photos);

  for(i=0; i<n_photos; i++){
    recv(peer_socket, &(photo), sizeof(photo), 0);
    (*id_photos)[i]=photo.id;
    printf("photo_id=%d\tphoto_name=%s\n", (*id_photos)[i], photo.name);
  }

  return n_photos;
}

int gallery_disconnect(int peer_socket){
  message m;
  int type=CLIENT_DEATH;

  send(peer_socket, &type, sizeof(type), 0);
  send(peer_socket, &m, sizeof(m), 0);
  return 0;
}
