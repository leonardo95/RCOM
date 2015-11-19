#include "project.h"

int main(int argc, char** argv)
{
	if(argc != 3)
	{
		printf("Wrong number os arguments in function main\n");
		return 1;
	}

	char* ip = argv[1];
	char* port = argv[2];

	printf("IP: %s\n", ip);
	printf("PORT: %s\n", port);

	ftp ftp_struct;

	int res = init_clientFTP(&ftp_struct, ip, port);
	if(res != 0)
	{
		printf("Error in funciton clientFTP: return value 1\n");
		return 1;
	}
	return 0;
}
