#include "project.h"


//inicia o FTP
int init_clientFTP(ftp* ftp, char* ip, char* port)
{	
	int socket = socketConnection(ip, port);
	if(socket < 0)
	{
		printf("Error in function socketConnection\n");
		return -1;
	}

	ftp->socket=socket;
	
	//char ftp_string[FTP_SIZE];
	
	//ftp_string = getFTP(ftp* ftp);

	return 0;
}

//Faz a conecção ao socket
int socketConnection(char* ip, char* port)
{
	int res=socket(AF_INET, SOCK_STREAM, 0);
	if(res < 0)
	{
		printf("Error in function socket\n");
		return -1;
	}

	int temp=serverConnection(res);
	if(temp < 0)
	{
		printf("Error in function serverConnection\n");
		return -1;
	} 

	return res;
}

//Faz a conecção ao servidor
int serverConnection(int socket)
{
	struct sockaddr_in server_addr;
	
	bzero((char*)&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
	server_addr.sin_port = htons(SERVER_PORT);

	int res = connect(socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if(res < 0)
	{
		printf("Error in function connect\n");
		return -1;
	}
	return res;
}

// Lê as respostas dadas pelo FTP
int Ftp_read(int socket, char *reply)
{
	int read_bytes;

	memset(reply, 0, READ_SIZE);
	read_bytes = read(socket, reply, READ_SIZE);

	return read_bytes;
}

// Envia um tipo de comando (USER, PASS, PASV, RETR para o servidor
int Ftp_send(int socket, char *factor, char *type)
{
	char buff[50];
	int write_bytes;

	memset(buff, 0, 50);
	strcat(buff, type);
	strcat(buff, "-");
	strcat(buff, factor);
	strcat(buff, "\n");
	
	write_bytes = write(socket, buff, strlen(buff));

	return write_bytes;
}


/*
//Retorna a string FTP
int getFTP(ftp* ftp)
{
	char res[FTP_SIZE];

	return res;
}
*/
// abrir socket para connect ao FTP
	// login no FTP
	// esperar resposta (replyCode), checkar e atuar
	// se der erro, terminar socket, terminar programa
	
	// entrar em modo passivo, calcular os últimos 2 bytes (penultimo * 256 + ultimo)
	// os primeiros 4 bytes serão o ip do segundo socket, que servirá para fazer download
	// abrir socket e guardar
	// pedir ficheiro ao FTP atraves do primeiro socket
	// usar o segundo socket e ler o ficheiro atraves de packets (ler, escrever, ler, escrever)
	// fechar os 2 sockets (close do fd). antes, mandar comando quit para primeiro
	// terminar programa
