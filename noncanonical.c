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
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

#define FLAG 0x7E
#define A 0x01
#define C 0x03
#define BCC 0x01^0x03

volatile int STOP=FALSE;

int main(int argc, char** argv)
{
    int fd, res;
    struct termios oldtio,newtio;
    //char buf;
    char *word;
    char UA[5];
    char SET;

	UA[0]=FLAG;
	UA[1]=A;
	UA[2]=C;
	UA[3]=BCC;
	UA[4]=FLAG;
	word= (char *)malloc(255);
	strcpy(word,"");
    if ( (argc < 2) || 
  	     ((strcmp("/dev/ttyS0", argv[1])!=0) && 
  	      (strcmp("/dev/ttyS4", argv[1])!=0) )) {
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
    newtio.c_cc[VMIN]     = 1;   /* blocking read until 1 chars received */



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
     while (STOP==FALSE) 
	{       // loop for input 
      		res = read(fd,&buf,1);   // returns after 5 chars have been input 
	
 	     	if (res != 1)
		{
			printf("Error reading from the serial port.\n");
			break;
		}
		if (buf=='\0') 
		{
		STOP=TRUE;
		}
		
		char newstr[2]; newstr[1] = '\0'; newstr[0] = buf;
		strcat(word, newstr);
				
	}

	sleep(1);	
	printf("%s\n", word);

	write(fd,word,strlen(word));
	sleep(1);

*/
	int i=0;
	while(i<5)
	{
		res=read(fd,&SET,1);
		if (res != 1)
		{
			printf("Error reading from the serial port.\n");
			break;
		}
		printf("%x\n", SET);
	i++;	
	}
	
	write(fd,UA,strlen(UA));
	sleep(1);

	
	

  /* 
    O ciclo WHILE deve ser alterado de modo a respeitar o indicado no guião 
  */



    tcsetattr(fd,TCSANOW,&oldtio);
    close(fd);
    return 0;
}
