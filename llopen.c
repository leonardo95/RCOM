#include "project.h"

int llopen(int port_num, int flag)
{
  char port[255];
  struct termios oldtio,newtio;
  int fd, ret=0;
  char port_number[255];
   
  sprintf(port_number, "%d", port_num);
  strcpy(port, "/dev/ttyS");
  strcat(port, port_number);
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
    
    if(flag==0)
    {
      ret=llopen_transmitter(fd);
    }
    else if(flag==1)
    {
      ret=llopen_reciever(fd);
    }
    
    llclose(fd,flag);

    tcsetattr(fd,TCSANOW,&oldtio);
    sleep(1);
	 
    close(fd);
    
  return ret;
}

int llopen_reciever(int fd)
{	
    int res;
    char ua[5];
    char set[5];
    int try=0, connected = FALSE;

    ua_function(ua);

    state_machine_set(fd, set);
	
    while(!connected){
	if(try >= 3){
		signal_stop();
		printf("Maximum number of tries reached, aborting connection.\n");
		return 0;
	}
    	res=write(fd,ua,strlen(ua));
	if(res != 5){
	  if(try == 0){
	   signal_set();
	   try++;
	   continue;
	  }else{
	    try++;
	    continue;
	  }
	}else{
	
          connected= TRUE;
	  printf("Connection established succesfully.\n");
	}
    }
    printf("%d bytes written\n", res);
    sleep(1);

    return res;
}

int llopen_transmitter(int fd)
{
    int res;
    char set[5];
    char ua[5];
    int try=0, connected = FALSE;

    set_function(set);
    
    while(!connected){
	if(try >= 3){
		signal_stop();
		printf("Maximum number of tries reached, aborting connection.\n");
		return 0;
	}

    	res = write(fd,set,strlen(set));

	if(res != 5){
	  if(try == 0){
	   signal_set();
	   try++;
	   continue;
	  }else{
	    try++;
	    continue;
	  }
	}else{
	  connected = TRUE;
	  printf("Connection established succesfully.\n");
	}

    	printf("%d bytes written\n", res);
    }
    state_machine_ua(fd, ua); 
    sleep(1);

    return res;
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



void set_function(char *set)
{
	printf("set_funtion -> initializing\n");
    set[0] = FLAG;
    set[1] = A_SET;
    set[2] = C_SET;
    set[3] = BCC_SET;
    set[4] = FLAG;
    printf("set values: %x, %x, %x, %x, %x\n", set[0], set[1], set[2], set[3], set[4]);
    printf("set_function -> terminated\n");
}

void state_machine_ua(int fd, char* ua)
{
	printf("state_machine_ua -> initializing\n");
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
					ua[0] = c;
					state = FLAG_RCV;
				} break;
			case FLAG_RCV:
				printf("case: FLAG_RCV\n");
				if(c == A_UA)
				{
					printf("state: A\n");
					ua[1] = c;
					state = A_RCV;
				}
				else
				{
					memset(ua,0,strlen(ua));
					state = STATE_MACHINE_START;
				} break;
			case A_RCV:
				printf("case: A_RCV\n");
				if(c == C_UA)
				{
					printf("state: C\n");
					ua[2] = c;
					state = C_RCV;		
				}
				else if(c == FLAG)
				{
					printf("state: FLAG\n");
					state= FLAG_RCV;
				}
				else
				{
					memset(ua,0,strlen(ua));
					state = STATE_MACHINE_START;
				} break;
			case C_RCV:
				printf("case: C_RCV\n");
				if(c == BCC_UA)
				{
					printf("state: BBC\n");
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
				printf("case: BCC_OK\n");
				if(c == FLAG)
				{
					printf("state: FLAG\n");
					ua[4] = c;
					state = STATE_MACHINE_STOP;				
				}
				else
				{
					memset(ua,0,strlen(ua));
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
