#define MESSAGE_LEN 100
#define MAX_KEYWORDS 10
#define MAX_CLIENTS 10

#define CLIENT_GW 1
#define CLIENT_SERVER 2
#define CLIENT_DEATH 10
#define SERVER_DEATH 20

#define S_CONNECT 30
#define S_ADD_PHOTO 31
#define S_ADD_KEYWORD 32
#define S_DELETE_PHOTO 33

#define ADD_PHOTO 50
#define ADD_KEYWORD 51
#define SEARCH_PHOTO 52
#define SEARCH_KEYWORD 53
#define DELETE_PHOTO 54
#define SYNC_PHOTO_LIST 55

#include <sys/socket.h>

typedef struct message{
    int type;
    char buffer[MESSAGE_LEN];
    uint32_t port;
    int source;
} message;

typedef struct server_struct{
    //int health;
    struct sockaddr_in addr;
    struct sockaddr_in sgw_addr;
    int nclients;
} server_struct;

typedef struct photo_struct{
    char name[MESSAGE_LEN];
    uint32_t id;
    char keyword[MAX_KEYWORDS][MESSAGE_LEN];
    int nkey;
    int source;
} photo_struct;

/*void error(const char *msg)
{
    perror(msg);
    exit(1);
}*/
