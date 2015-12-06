#include "project.h"

char* getIP(char *hostname){

	struct hostent *h;

	if((h = gethostbyname(hostname)) == NULL)
	{
		herror("gethostbyname");
		exit(2);
	}

	char *ip = inet_ntoa(*((struct in_addr *)h->h_addr));
	
	return ip;
}

void Parse_Url(char * url, char * user, char * password, char * host, char * path)
{
	if (sscanf(url, "ftp://%[^:]:%[^@]@%[^/]/%s", user, password, host, path) != 4)
	{
		fprintf(stderr, "Invalid URL! Usage: ftp ftp://<user>:<password>@<host>/<url-path>\n");
		exit(1);
	}

}
