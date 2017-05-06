#define MESSAGE_LEN 100
#define MAX_KEYWORDS 10

#define GW_SERVER 0
#define CLIENT_GW 1
#define CLIENT_SERVER 2

typedef struct message{
    int type;
    char buffer[MESSAGE_LEN];
    int port;
} message;

typedef struct photo{
    char name[MESSAGE_LEN];
    uint32_t id;
    char keyword[MAX_KEYWORDS][MESSAGE_LEN];
} photo;

void error(const char *msg)
{
    perror(msg);
    exit(1);
}
