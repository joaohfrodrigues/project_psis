#define MESSAGE_LEN 100

#define GW_SERVER 0
#define CLIENT_GW 1
#define CLIENT_SERVER 2

typedef struct message{
    int type;
    char buffer[MESSAGE_LEN];
    int port;
} message;

void error(const char *msg)
{
    perror(msg);
    exit(1);
}
