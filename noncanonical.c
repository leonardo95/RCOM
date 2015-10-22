#include "project.h"

typedef enum {
  STATE_MACHINE_START, FLAG_RCV, A_RCV, C_RCV, BCC_OK, STATE_MACHINE_STOP
} State;

volatile int STOP=FALSE;

int main(int argc, char** argv)
{
    int fd, res;
    struct termios oldtio,newtio;
    char ua[5];
    char set[5];

    ua_function(ua);

    if ( (argc < 2) || ((strcmp("/dev/ttyS0", argv[1])!=0) && (strcmp("/dev/ttyS4", argv[1])!=0) )) 
    {
      printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
      exit(1);
    }
 
    fd = open(argv[1], O_RDWR | O_NOCTTY );
    
    if (fd <0) {perror(argv[1]); exit(-1); }

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
    newtio.c_cc[VMIN]     = 1;

    tcflush(fd, TCIOFLUSH);

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) 
    {
      perror("tcsetattr");
      exit(-1);
    }

    printf("New termios structure set\n");

    int i=0;
    while(i<5)
   {
     res=read(fd,set,1);
     if (res != 1)
     {
       printf("Error reading from the serial port.\n");
       break;
     }
     printf("%x\n", set[i]);
      i++;  
   }
	
	res=write(fd,ua,strlen(ua));
  printf("%d bytes written\n", res);
	
  state_machine_set(fd, set);
  sleep(1);

  tcsetattr(fd,TCSANOW,&oldtio);
  close(fd);
  return 0;
}

void ua_function(char *ua)
{
  printf("ua() -> initializing\n");
    ua[0] = FLAG;
    ua[1] = A_UA;
    ua[2] = C_UA;
    ua[3] = BCC_UA;
    ua[4] = FLAG;
    printf("ua() -> FLAG: %x\n", ua[0]);
    printf("ua() -> A: %x\n", ua[1]);
    printf("ua() -> C: %x\n", ua[2]);
    printf("ua() -> BCC: %x\n", ua[3]);
    printf("ua() -> FLAG: %x\n", ua[4]);
    printf("ua() -> terminated\n");
}

void state_machine_set(int fd, char* set)
{
  printf("state_machine_set() -> initializing\n");
  State state = STATE_MACHINE_START;
  int end = FALSE;

  while(!end)
  {
    unsigned char c;
    
    if(state != STATE_MACHINE_STOP)
    {
      int res = read(fd, &c, 1);

      if(res != 1)
      {
        printf("error reading: nothing received\n");
      }
    }

    switch(state)
    {
      case STATE_MACHINE_START:
        if(c == FLAG)
        {
          set[0] = c;
          state = FLAG_RCV;
        } break;
      case FLAG_RCV:
        if(c == A_UA)
        {
          set[1] = c;
          state = A_RCV;
        }
        else
        {
          memset(set,0,strlen(set));
          state = STATE_MACHINE_START;
        } break;
      case A_RCV:
        if(c == C_UA)
        {
          set[2] = c;
          state = C_RCV;    
        }
        else if(c == FLAG)
        {
          state= FLAG_RCV;
        }
        else
        {
          memset(set,0,strlen(set));
          state = STATE_MACHINE_START;
        } break;
      case C_RCV:
        if(c == BCC_UA)
        {
          set[3] = c;
          state = BCC_OK;
        }
        else if(c == FLAG)
        {
          state= FLAG_RCV;
        }
        else
        {
          memset(set,0,strlen(set));
          state = STATE_MACHINE_START;
        } break;
      case BCC_OK:
        if(c == FLAG)
        {
          set[4] = c;
          state = STATE_MACHINE_STOP;       
        }
        else
        {
          memset(set,0,strlen(set));
          state = STATE_MACHINE_START;
        } break;
      case STATE_MACHINE_STOP: end=TRUE; break;
    }
  }
  printf("state_machine_set() -> terminated\n");
}