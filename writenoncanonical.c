#include "project.h"

typedef enum {
	STATE_MACHINE_START, FLAG_RCV, A_RCV, C_RCV, BCC_OK, STATE_MACHINE_STOP
} State;

volatile int STOP=FALSE;

int main(int argc, char** argv)
{
    int fd,c, res;
    struct termios oldtio,newtio;
    char buf[255];
    int i, sum = 0, speed = 0;
    char* set;

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
 
	char* ua = state_machine_UA(fd); 
	sleep(1);

    if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) 
    {
      perror("tcsetattr");
      exit(-1);
    }

    close(fd);
    return 0;
}

char* set()
{
	char set[5];
    set[0] = FLAG;
    set[1] = A;
    set[2] = C;
    set[3] = BCC;
    set[4] = FLAG;
    return set;
}

unsigned char* state_machine_UA(int fd)
{
	char ua[5];
	State state = START;
	int end = FALSE;

	while(!end)
	{
		unsigned char c;
		
		if(state != STOP_SM)
		{
			int res = read(fd, &c, 1);

			if(res != 1)
			{
				printf("error reading: nothing received\n");
			}
		}

		switch(state)
		{
			case START:
				if(c == FLAG)
				{
					ua[0] = c;
					state = FLAG_RCV;
				} break;
			case FLAG_RCV:
				if(c == A)
				{
					ua[1] = c;
					state = A_RCV;
				}
				else
				{
					memset(ua,0,strlen(ua));
					state = START;
				} break;
			case A_RCV:
				if(c == C)
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
					state = START;
				} break;
			case C_RCV:
				if(c == BCC)
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
					state = START;
				} break;
			case BCC_OK:
				if(c == FLAG)
				{
					ua[5] = c;
					state = STOP_SM;				
				}
				else
				{
					memset(ua,0,strlen(ua));
					state = START;
				} break;
			case STOP_SM: end=TRUE; break;
		}
	}

	print_array(ua);
	return ua;
}

void print_array(unsigned char* array)
{
	int i=0;
	for(i; i< strlen(array); i++){

		printf("%x\n", array[i]);

	}
}
