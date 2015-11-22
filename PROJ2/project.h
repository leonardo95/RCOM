#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>
#include <strings.h>

#define SERVER_PORT 6000
#define SERVER_ADDR "192.168.28.96"
#define FTP_SIZE 1024

typedef struct URL {
	char* username;
	char* password;
	char* hostname;
	char* ip;
	char* path;
	char* filename;
} url;

typedef struct FTP
{
    int socket;
} ftp;

int init_clientFTP(ftp* ftp, char* ip, char* port);
int socketConnection(char* ip, char* port);
int serverConnection(int socket);
int getFTP(ftp* ftp);