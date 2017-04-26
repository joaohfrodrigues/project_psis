#define SOCKET_GW "gateway"
#define SOCKET_CS "c_s"
#define MESSAGE_LEN 100

typedef struct message{
    char buffer[MESSAGE_LEN];
} message;

typedef struct message_gw{
	int type;
    char address[20];
    int port;
} message_gw;