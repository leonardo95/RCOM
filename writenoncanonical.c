/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

#define FLAG 0x7E
#define C 0x07
#define A 0x03
#define BCC 0x03^0x07


typedef enum {
	START, FLAG_RCV, A_RCV, C_RCV, BCC_OK, STOP_SM
} State;

volatile int STOP=FALSE;

unsigned char* state_machine_UA(int fd){

	char ua[5];
	State state = START;
	int end = FALSE;

	while(!end){
		unsigned char c;
		
		if(state != STOP_SM){
			int res = read(fd, &c, 1);

			if(res != 1){
				printf("error reading: nothing received\n");
			}
		}

		switch(state){

		case START:
			if(c == FLAG){
				ua[0] = c;

				state = FLAG_RCV;

				
			}
			break;
		case FLAG_RCV:
			if(c == A){
				ua[1] = c;

				state = A_RCV;

				
			}else{
				memset(ua,0,strlen(ua));
				state = START;
			}
			break;
		case A_RCV:
			if(c == C){
				ua[2] = c;

				state = C_RCV;

				
			}else if(c == FLAG){
				state= FLAG_RCV;
			}else{
				memset(ua,0,strlen(ua));
				state = START;
			}
			break;
		case C_RCV:
			if(c == BCC){
				ua[3] = c;

				state = BCC_OK;

				
			}else if(c == FLAG){
				state= FLAG_RCV;
			}else{
				memset(ua,0,strlen(ua));
				state = START;
			}	
			break;

		case BCC_OK:
			if(c == FLAG){
				ua[5] = c;
				state = STOP_SM;

				
			}else{
				memset(ua,0,strlen(ua));
				state = START;
			}
			break;
		case STOP_SM:
			end=TRUE;
			break;
		}

	}
	int i=0;
	for(i; i< strlen(ua); i++){

		printf("%x\n", ua[i]);

	}
	
}

int main(int argc, char** argv)
{
    int fd,c, res;
    struct termios oldtio,newtio;
    char buf[255];
    int i, sum = 0, speed = 0;
    char set[5];
    set[0] = FLAG;
    set[1] = A;
    set[2] = C;
    set[3] = BCC;
    set[4] = FLAG;
    if ( (argc < 2) || 
  	     ((strcmp("/dev/ttyS0", argv[1])!=0) && 
  	      (strcmp("/dev/ttyS1", argv[1])!=0) )) {
      printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
      exit(1);
    }


  /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */


    fd = open(argv[1], O_RDWR | O_NOCTTY );
    if (fd <0) {perror(argv[1]); exit(-1); }

    if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
      perror("tcgetattr");
      exit(-1);
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 5;   /* blocking read until 5 chars received */



  /* 
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a 
    leitura do(s) próximo(s) caracter(es)
  */



    tcflush(fd, TCIOFLUSH);

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    printf("New termios structure set\n");



	/*
	printf("String: ");
	gets(buf);
	*/


  


  
    res = write(fd,set,strlen(set));   
    printf("%d bytes written\n", res);
 
	

  /* 
    O ciclo FOR e as instruções seguintes devem ser alterados de modo a respeitar 
    o indicado no guião 
  */	 
	/*
	read(fd,buf,strlen(buf)+1);
	printf("%s\n", buf); 
	*/
	char* ua = state_machine_UA(fd); 
	


   sleep(1);

    if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }




    close(fd);
    return 0;
}
