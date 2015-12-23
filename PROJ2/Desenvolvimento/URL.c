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
	 if(sscanf(url, "ftp://%[^:]:%[^@]@%[^/]/%s", user, password, host, path) == 4)
	 {
		return;
	 }
	 if (sscanf(url, "ftp://%[^@]@%[^/]/%[^\n]", user, host, path) == 3) 
         {
		strcpy(password, "pass_temp");
		return;

         } 
	 else if (sscanf(url, "ftp://%[^:@/]/%[^\n]", host, path) == 2) 
	 {
		strcpy(user, "anonymous");
		strcpy(password, "pass_temp");
		return;
	 }
	 else
	 {
		fprintf(stderr, "Invalid URL! Usage: ftp ftp://<user>:<password>@<host>/<url-path>\n");
		exit(2);
	 }
}

void check_Pass_User(char * pass, char * user)
{
	if(strcmp (pass, "pass_temp") == 0)
	printf("\nBy default your password is pass_temp. Fill the password field next time.\n");

	if(strcmp (user, "anonymous") == 0)
	printf("\nYou have entered on anonymous mode.\n\n\n");
}
