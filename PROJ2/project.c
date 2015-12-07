#include "project.h"

int main(int argc, char** argv)
{
	if(argc != 2)
	{
		printf("Wrong number os arguments in function main\n");
		return -1;
	}

	char host[STRING_SIZE];
	char user[STRING_SIZE];
	char password[STRING_SIZE];
	char path[STRING_SIZE];

	// Get all the info about URL
	Parse_Url(argv[1], user, password, host, path);

	printf("\nInfo Aplication Download\n\n");
	printf("User: %s\n", user);
	printf("Password: %s\n", password);
	printf("Host: %s\n", host);
	printf("Path to File: %s\n", path);


	// Get IP from host
	char* Server_Address = getIP(host);

	// Prepare Connection	
	printf("Start Connection...\n\n");
	printf("IP Address: %s\n\n", Server_Address);

	/// Start Connection
	int sockfd;
	sockfd = connect_server(FTP_PORT, Server_Address);
	
	printf("Login...\n\n");
	
	char reply[STRING_SIZE];

	Ftp_read(sockfd, reply);
	//Login
	FTP_Login(sockfd, user, password);
	
	printf("Enter Pasv mode...\n\n");
	//Pasv mode
	int sockfd_2 = FTP_Mode_Passive(sockfd);

	//Retr mode
	FTP_Retr(sockfd, path);

	//Download File
	FTP_Download(sockfd_2, path);

	//Disconnects from server
	FTP_disconnet(sockfd, sockfd_2);
	
	return 0;
}
