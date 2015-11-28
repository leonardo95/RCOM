#include "project.h"

char getIP(char * host)
{
	struct hostent *h;
	
	char * buf = malloc(STRING_SIZE);
	h=gethostbyname(host);
	if(h == NULL)
	{
		fprintf(stderr,"Error in function gethostbyname\n");
		exit(1);
	}

	strcpy(buf, inet_ntoa(*((struct in_addr *)h->h_addr)));

	return *buf;
}

void Parse_Url(char * url, char * user, char * password, char * host, char * path)
{
	if (sscanf(url, "ftp://%[^:]:%[^@]@%[^/]/%s", user, password, host, path) != 4)
	{
		fprintf(stderr, "Invalid URL! Usage: ftp ftp://<user>:<password>@<host>/<url-path>\n");
		exit(1);
	}

}
