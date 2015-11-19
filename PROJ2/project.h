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


typedef struct FTP
{
    int socket;
} ftp;

int init_clientFTP(ftp* ftp, char* ip, char* port);