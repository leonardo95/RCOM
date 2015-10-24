#include "project.h"

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
	
  state_machine_set(fd, set);

	res=write(fd,ua,strlen(ua));
  printf("%d bytes written\n", res);
	
  sleep(1);

  tcsetattr(fd,TCSANOW,&oldtio);
  close(fd);
  return 0;
}

void ua_function(char *ua)
{
  printf("ua_function -> initializing\n");
    ua[0] = FLAG;
    ua[1] = A_UA;
    ua[2] = C_UA;
    ua[3] = BCC_UA;
    ua[4] = FLAG;
    printf("ua values: %x, %x, %x, %x, %x\n", ua[0],ua[1],ua[2],ua[3],ua[4]);
    printf("ua_function -> terminated\n");
}

void state_machine_set(int fd, char* set)
{
  printf("state_machine_set -> initializing\n");
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
        printf("case: STATE_MACHINE_START\n");
        if(c == FLAG)
        {
          printf("state: FLAG\n");
          set[0] = c;
          state = FLAG_RCV;
        } break;
      case FLAG_RCV:
        printf("case: FLAG_RCV\n");
        if(c == A_SET)
        {
          printf("state: A\n");
          set[1] = c;
          state = A_RCV;
        }
        else
        {
          memset(set,0,strlen(set));
          state = STATE_MACHINE_START;
        } break;
      case A_RCV:
        printf("case: A_RCV\n");
        if(c == C_SET)
        {
          printf("state: C\n");
          set[2] = c;
          state = C_RCV;    
        }
        else if(c == FLAG)
        {
          printf("state: FLAG\n");
          state= FLAG_RCV;
        }
        else
        {
          memset(set,0,strlen(set));
          state = STATE_MACHINE_START;
        } break;
      case C_RCV:
        printf("case: C_RCV\n");
        if(c == BCC_SET)
        {
          printf("state: BBC\n");
          set[3] = c;
          state = BCC_OK;
        }
        else if(c == FLAG)
        {
          printf("state: FLAG\n");
          state= FLAG_RCV;
        }
        else
        {
          memset(set,0,strlen(set));
          state = STATE_MACHINE_START;
        } break;
      case BCC_OK:
      printf("case: BCC_OK\n");
        if(c == FLAG)
        {
          printf("state: FLAG\n");
          set[4] = c;
          state = STATE_MACHINE_STOP;       
        }
        else
        {
          memset(set,0,strlen(set));
          state = STATE_MACHINE_START;
        } break;
      case STATE_MACHINE_STOP: 
      end=TRUE; 
       printf("state: STATE_MACHINE_STOP\n");
      break;
    }
  }
  printf("state_machine -> terminated\n");
}