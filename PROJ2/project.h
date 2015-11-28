#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>
#include <string.h>

#define SERVER_PORT 6000
#define SERVER_ADDR "192.168.28.96"
#define FTP_SIZE 1024
#define STRING_SIZE 255
#define FTP_PORT 21
#define FAIL_LOGIN 530

void init_clientFTP(char ip, int port);
int socketConnection(char ip, int port);
int serverConnection(int socket);
int FTP_Login(int sockfd, char * user, char * password);
int FTP_disconnet(int sockfd_1, int sockfd_2);
int FTP_Mode_Passive(int sockfd);
int FTP_Retr(int sockfd, char * path);
int FTP_Download(int sockfd, char * path);
int Ftp_read(int socket, char * reply);
int Ftp_send(int socket, char *factor, char *type);
char getIP(char * host);
void Parse_Url(char * url, char * user, char * password, char * host, char * path);
