#include "project.h"

typedef enum {
	STATE_MACHINE_START, FLAG_RCV, A_RCV, C_RCV, BCC_OK, STATE_MACHINE_STOP
} State;

volatile int STOP=FALSE;

int main(int argc, char** argv)
{
    int fd, res;
    struct termios oldtio,newtio;
    char set[5];
    char ua[5];

    set_function(set); //A funcionar corretamente
  
    if ((argc < 2) || ((strcmp("/dev/ttyS0", argv[1])!=0) && (strcmp("/dev/ttyS1", argv[1])!=0) )) 
    {
      printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
      exit(1);
    }

    fd = open(argv[1], O_RDWR | O_NOCTTY );
    
    if (fd <0) { perror(argv[1]); exit(-1); }

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

    res = write(fd,set,strlen(set));   
    printf("%d bytes written\n", res);
 
    int i=0;
    while(i<5)
   {
     res=read(fd,ua,1);
     if (res != 1)
     {
       printf("Error reading from the serial port.\n");
       break;
     }
     printf("%x\n", ua[i]);
      i++;  
   }

	state_machine_ua(fd, ua); 
	sleep(1);

    if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) 
    {
      perror("tcsetattr");
      exit(-1);
    }

    close(fd);
    return 0;
}

void set_function(char *set)
{
	printf("set() -> initializing\n");
    set[0] = FLAG;
    set[1] = A_SET;
    set[2] = C_SET;
    set[3] = BCC_SET;
    set[4] = FLAG;
    printf("set() -> FLAG: %x\n", set[0]);
    printf("set() -> A: %x\n", set[1]);
    printf("set() -> C: %x\n", set[2]);
    printf("set() -> BCC: %x\n", set[3]);
    printf("set() -> FLAG: %x\n", set[4]);
    printf("set() -> terminated\n");
}

void state_machine_ua(int fd, char* ua)
{
	printf("state_machine_ua() -> initializing\n");
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
					ua[0] = c;
					state = FLAG_RCV;
				} break;
			case FLAG_RCV:
				if(c == A_SET)
				{
					ua[1] = c;
					state = A_RCV;
				}
				else
				{
					memset(ua,0,strlen(ua));
					state = STATE_MACHINE_START;
				} break;
			case A_RCV:
				if(c == C_SET)
				{
					ua[2] = c;
					state = C_RCV;		
				}
				else if(c == FLAG)
				{
					state= FLAG_RCV;
				}
				else
				{
					memset(ua,0,strlen(ua));
					state = STATE_MACHINE_START;
				} break;
			case C_RCV:
				if(c == BCC_SET)
				{
					ua[3] = c;
					state = BCC_OK;
				}
				else if(c == FLAG)
				{
					state= FLAG_RCV;
				}
				else
				{
					memset(ua,0,strlen(ua));
					state = STATE_MACHINE_START;
				} break;
			case BCC_OK:
				if(c == FLAG)
				{
					ua[4] = c;
					state = STATE_MACHINE_STOP;				
				}
				else
				{
					memset(ua,0,strlen(ua));
					state = STATE_MACHINE_START;
				} break;
			case STATE_MACHINE_STOP: end=TRUE; break;
		}
	}
	printf("state_machine_UA() -> terminated\n");
}
