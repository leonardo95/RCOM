#include "project.h"

int main(int argc, char** argv)
{
  int fd=0;
  int port=atoi(argv[1]);
  int flag=atoi(argv[2]);
  
  fd=llopen(port, flag);
  printf("file descriptor = %d\n", fd);
  if(flag==1)
  {
  	char buffer[100];
  	int i=0;
  	for(;i<100;i++)
  	{
  		buffer[i]='b';
  	}
    printf("initiating llwrite test\n");
  	llwrite(fd,buffer,100,0);
  }
  if(flag==0)
  {
  	char* buffer=malloc(100);
    printf("initiating llread test\n");
  	llread(fd, buffer, 1);
  	int i=0;
  	for(;i<100;i++)
  	{
  		printf("0x%02x ", buffer[i]);
  	}
    printf("\n");
  }

  return 0;
}