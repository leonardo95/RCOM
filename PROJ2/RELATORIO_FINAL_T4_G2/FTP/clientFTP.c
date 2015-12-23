#include "project.h"

// Connects to Server
int connect_server(int port, char* addr){

	int sockfd;
	struct	sockaddr_in server_addr;

	/*server handler address*/
	bzero((char*)&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(addr);	
	server_addr.sin_port = htons(port);		

	if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0) {
		perror("socket()");
		exit(0);
	}
	/*connect to the server*/
	if(connect(sockfd,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0){
			perror("connect()");
			exit(0);
	}

	return sockfd;
}
// Coloca o servidor em modo passivo
int FTP_Mode_Passive(int sockfd)
{
	char reply[STRING_SIZE];
	int pasv_1, pasv_2, pasv_3, pasv_4, pasv_5, pasv_6;

	//Sends pasv to server
	if(Ftp_send(sockfd, "pasv", "PASV") < 0)
	{
		printf("Error sending pasv to server\n");
		return -1;
	}
	
	//Reads from the server
	if(Ftp_read(sockfd, reply) < 0)
	{
		printf("Error while receiving answer from pasv information \n");
		return -1;
	}
	sscanf(reply, "%*[^(](%d,%d,%d,%d,%d,%d)\n", &pasv_1, &pasv_2, &pasv_3, &pasv_4, &pasv_5, &pasv_6);

	char ip[256];
	int port = pasv_5*256 + pasv_6;
	
	sprintf(ip,"%d.%d.%d.%d", pasv_1, pasv_2, pasv_3, pasv_4);
	
	// Get IP from host
	printf("New IP Address: %s\n\n", ip);
	char* Server_Address = getIP(ip);

	printf("\nStarts a new Connection...\n\n");
	printf("New IP Address: %s\n\n",  Server_Address);

	// Connects with the new ip
	int sockfd_2;
	sockfd_2 = connect_server(port, Server_Address);
	if(sockfd_2 < 0)
	{
		printf("Error connecting to the new socket\n");
		return -1;
	}
	return sockfd_2;
}

// Faz download do ficheiro descrito no path
int FTP_Download(int sockfd, char * path)
{
	char buf[STRING_SIZE];
	char file_name[450];

	int i, j;
	i = 0, j = strlen(path) - 1;

	while(j > 0)
	{
		if(path[j] == '/')
		{
			j++;
			break;
		}
		j--;
	}

	while(j < strlen(path))
	{
		file_name[i]=path[j];
		i++;
		j++;
	}
	file_name[i]='\0';
	
	printf("Filename described by path: %s\n",file_name);

	int w;
	FILE *file_download = fopen(file_name, "w");

	while((w=read(sockfd, buf, STRING_SIZE)) > 0)
	{
		fwrite(buf,sizeof(char),w,file_download);
	}

	fclose(file_download);
	
	printf("New File received successfuly!!!\n\n");

	return 0;

}

int FTP_disconnet(int sockfd_1, int sockfd_2)
{
	char reply[STRING_SIZE];

	if(Ftp_read(sockfd_1, reply) < 0)
	{
		printf("Error after receiveing file\n");
		return -1;
	}

	if(Ftp_send(sockfd_1, "quit", "QUIT") < 0)
	{
		printf("Error in sending quit command\n");
		return -1;
	}

	if(Ftp_read(sockfd_1, reply) < 0)
	{
		printf("Error on disconnecing from server\n");
		return -1;
	}


	if(close(sockfd_1) < 0)
	{
		printf("Failed to close ftp socket 1.\n");
		return -1;
	}

	if(close(sockfd_2) < 0)
	{
		printf("Failed to close ftp socket 2.\n");
		return -1;
	}

	printf("Disconnected from server.\n");

	return 0;
}
	


// Envia mensagem ao servidor que pretende descarregar um ficheiro
int FTP_Retr(int sockfd, char * path)
{
	char reply[STRING_SIZE];

	//Sends retr to server
	if(Ftp_send(sockfd, path, "RETR") < 0)
	{
		printf("Error sending retr with file path to server\n");
		return -1;
	}
	//Reads from the server
	if(Ftp_read(sockfd, reply) < 0)
	{
		printf("Error while receiving answer from retr information \n");
		return -1;
	}

	return 0;
}

// Faz login no servidor
int FTP_Login(int sockfd, char * user, char * password)
{
	//Checks user
	char reply[STRING_SIZE];
	if(Ftp_send(sockfd, user, "USER") < 0)
	{
		printf("Error sending user information\n");
		return -1;
	}

	if(Ftp_read(sockfd, reply) < 0)
	{
		printf("Error while receiving answer from user information \n");
		return -1;
	}
	
	//Checks password
	if(Ftp_send(sockfd, password, "PASS") < 0)
	{
		printf("Error sending password information\n");
		return -1;
	}

	if(Ftp_read(sockfd, reply) < 0)
	{
		printf("Error while receiving answer from password information \n");
		return -1;
	}

	int code;
	sscanf(reply, "%d", &code);
	if(code == FAIL_LOGIN)
	{ 
		printf("Failed to login. Username or password don't exist.\n");
		exit(2);
	}

	return 0;
}

// Lê as respostas dadas pelo FTP
int Ftp_read(int socket, char *reply)
{
	int read_bytes;

	memset(reply, 0, STRING_SIZE);
	read_bytes = read(socket, reply, STRING_SIZE);
	printf("Reply: %s\n", reply);

	return read_bytes;
}

// Envia um tipo de comando (USER, PASS, PASV, RETR para o servidor
int Ftp_send(int socket, char *factor, char *type)
{
	char buff[50];
	int write_bytes;

	memset(buff, 0, 50);
	strcat(buff, type);
	strcat(buff, " ");
	strcat(buff, factor);
	strcat(buff, "\r\n");

	printf("Element sent: %s\n", buff);
	
	write_bytes = write(socket, buff, strlen(buff));

	return write_bytes;
}

