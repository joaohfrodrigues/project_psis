/***************************************************************************
* PSis-project 2016-17
* by
* João Rodrigues and Sara Vieira
*
* image_server.h
* Header file with all the declarations and defines used in the project
****************************************************************************/

#define MESSAGE_LEN 100
#define MAX_KEYWORDS 10
#define MAX_CLIENTS 20
#define MAX_SERVERS 20

#define CLIENT_GW 1
#define CLIENT_SERVER 2
#define CLIENT_DEATH 10
#define SERVER_DEATH 20

#define S_CONNECT 30
#define S_ADD_PHOTO 31
#define S_ADD_KEYWORD 32
#define S_DELETE_PHOTO 33
#define S_GET_PHOTO_NAME 34
#define S_GET_PHOTO 35
#define S_SERVER_DEATH 36
#define S_SEND_PHOTO 37

#define ADD_PHOTO 50
#define ADD_KEYWORD 51
#define SEARCH_PHOTO 52
#define GET_PHOTO 53
#define GET_PHOTO_NAME 54
#define DELETE_PHOTO 56
#define SYNC_PHOTO_LIST 57

#include <sys/socket.h>
#include <stdint.h>

typedef struct message{
    int type;
    char buffer[MESSAGE_LEN];
    uint32_t port;
    int source;
    int s_client;
} message;

typedef struct server_struct{
    //int health;
    struct sockaddr_in addr;
    struct sockaddr_in sgw_addr;
    int nclients;
    int s_server;
} server_struct;

typedef struct photo_struct{
    char name[MESSAGE_LEN];
    uint32_t id;
    char keyword[MAX_KEYWORDS][MESSAGE_LEN];
    int nkey;
    int source;
    int s_client;
    int size;
} photo_struct;

/*void error(const char *msg)
{
    perror(msg);
    exit(1);
}*/
