#include "project.h"

int start_applicationlayer(int port, int role, char* filename)
{
	int fd;
	fd=llopen(port, role);
	if(fd==-1)
	{
		printf("ERROR: in function llopen()\n");
		return -1;
	}
	if(role==0)
	{
		packageControl_send(fd, role, filename);

	}
	else if(role==1)
	{
		packageControl_recieve(fd, role, filename);
	}

	return 0;
}

int packageControl_send(int fd, int role, char* filename)
{
	FILE* file = fopen(filename, "rb");
	fseek(file, 0, SEEK_END);
	int size = ftell(file);
	fseek(file, 0, SEEK_SET);
	char filesize[255];
	sprintf(filesize, "%d", size);
	printf("%s\n" ,filesize);

	int counter=0, i=0;
	int packSize = 5 + strlen(filesize) + strlen(filename);

	char packControl[packSize];
	packControl[counter]=C_SET;
	counter++;
	packControl[counter]=filesize;
	counter++;
	packControl[counter]=strlen(filesize);
	counter++;

	for(;i<strlen(filename);i++)
	{
		packControl[counter]=filename[i];
		counter++;
	}

	llwrite(fd, packControl, packSize, role);
	return 0;
}

int packageControl_recieve(int fd, int role, char* filename)
{
	int res=0, i=0, counter=1, num=0;
	FILE* file = fopen(filename, "rb");
	fseek(file, 0, SEEK_END);
	int size = ftell(file);
	fseek(file, 0, SEEK_SET);
	char filesize[255];
	sprintf(filesize, "%d", size);
	printf("%s\n" ,filesize);

	char* pack;

	res=llread(fd, pack, role, number); //ONDE ARRANJO O NUMBER?

	for(;i<2;i++)
	{
		int type=pack[pos];
		counter++;

		if(i==0)
		{
			num=pack[counter];
			counter++;
			char* temp=malloc(num);
			memcpy(temp,pack[counter],num);	
		}
		else if(i==1)
		{
			num=(char) pack[counter];
			counter++;
			memcpy(filename, pack[counter], num);
		}
	}
	return 0;
}
