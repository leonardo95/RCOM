#include "project.h"

int getIP(url* url)
{
	struct hostent *h;

	h=gethostbyname(url->hostname);
	if(h == NULL)
	{
		printf("Error in function gethostbyname\n");
		return -1;
	}

	url->ip = inet_ntoa(*((struct in_addr *)h->h_addr));

	return 0;
}