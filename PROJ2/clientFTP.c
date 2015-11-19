#include "project.h"


//inicia o FTP
int init_clientFTP(ftp* ftp, char* ip, char* port)
{
	printf("socket file descriptor: %d\n", ftp->socket);
	printf("IP: %s\n", ip);
	printf("PORT: %s\n", port);
	
	int socket;
//	char ftp_string[FTP_SIZE];

	socket = socketConnection(ip, port);
//	ftp_string = getFTP(ftp* ftp);

	return 0;
}

//Faz a conecção ao socket - ver clientTCP.c
int socketConnection(char* ip, char* port)
{
	int socket;

	return socket;
}
/*
//Retorna a string FTP
int getFTP(ftp* ftp)
{
	char res[FTP_SIZE];

	return res;
}

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
*/