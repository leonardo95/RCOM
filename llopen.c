#include "project.h"

int main(int argc, char** argv)
{
  char port[255];
  struct termios oldtio,newtio;
  int fd, ret=0;
  
  if ((argc < 2) || (argv[1]<0) || ((strcmp("0", argv[2])!=0) && (strcmp("0", argv[2])!=1))) 
  {
      printf("Invalid arguments for function llopen\n");
      exit(1);
  }
  
  strcpy(port, "/dev/ttyS");
  strcat(port, argv[1]);
  printf("%s\n", port);
  
    fd = open(port, O_RDWR | O_NOCTTY );
    
    if (fd <0) { perror(port); exit(-1); }

    if ( tcgetattr(fd,&oldtio) == -1) 
    {
      perror("tcgetattr");
      exit(-1);
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    newtio.c_lflag = 0;
    newtio.c_cc[VTIME]    = 0;
    newtio.c_cc[VMIN]     = 5;
    
    tcflush(fd, TCIOFLUSH);

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) 
    {
      perror("tcsetattr");
      exit(-1);
    }

    printf("New termios structure set\n");
    
    if(strcmp("0", argv[2])==0)
    {
      ret=transmitter(fd);
    }
    else if(strcmp("1", argv[2])==0)
    {
      ret=reciever(fd);
    }
    
    tcsetattr(fd,TCSANOW,&oldtio);
    close(fd);
    
  return ret;
}