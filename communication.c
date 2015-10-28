#include "project.h"

int main(int argc, char** argv)
{
  int fd=0;
  int port=atoi(argv[1]);
  int flag=atoi(argv[2]);
  
  printf("TESTE 1\n");
  fd=llopen(port, flag);
  printf("TESTE 2\n");
  if(flag==0)
  {
  	 printf("TESTE 3.1\n");
  	char buffer[255];
  	int i=0;
  	for(;i<255;i++)
  	{
  		buffer[i]=i;
  	}
  	 printf("TESTE 4\n");
  	llwrite(fd,buffer,255,0);
  }
  if(flag==1)
  {
  	char buffer[255];
  	llread(fd, buffer, 1);
  	int i=0;
  	for(;i<255;i++)
  	{
  		printf("0x%02x ", buffer[i]);
  	}
  }

  return 0;
}