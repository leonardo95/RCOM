#include "project.h"

int llclose(int fd, int type)
{
  int res=0;
  
  if(type==1)
  {
  res=llclose_transmitter(fd); 
  }
  else if(type==0)
  {
    res=llclose_reciever(fd);
  }
   close(fd);
  return res;
}

int llclose_transmitter(int fd)
{
    int res;
    char disc[5];
    char ua[5];
    int disconnected = FALSE, try=0;

    disc_function(disc);
    while(!disconnected){
	if(try >= 3){
		signal_stop();
		printf("Maximum number of tries reached.\n");
		return 0;
	}
	res = write(fd,disc,5);
	stat_send_disc++;
	if(res != 5){
	  if(try == 0){
	   signal_set();
	   try++;
	   continue;
	  }else{
	    try++;
	    continue;
	  }
	}

	state_machine_disc(fd, disc);
	stat_rec_disc++;
		    
	ua_function(ua);

	res = write(fd,ua,5);
sleep(1);

	stat_send_ua++;
	if(res != 5){
	  if(try == 0){
	   signal_set();
	   try++;
	   continue;
	  }else{
	    try++;
	    continue;
	  }
	}
	disconnected = TRUE;  
    }

    return res;
}

int llclose_reciever(int fd)
{
    int res;
    char ua[5];
    char disc[5];
    int disconnected = FALSE, try=0;
	while(!disconnected){
	if(try >= 3){
		signal_stop();
		printf("Maximum number of tries reached.\n");
		return 0;
	}
    	state_machine_disc(fd, disc);
	stat_rec_disc++;
	
	disc_function(disc);
    	res=write(fd,disc,5);
	stat_send_disc++;
	if(res != 5){
	  if(try == 0){
	   signal_set();
	   try++;
	   continue;
	  }else{
	    try++;
	    continue;
	  }
	}  
	disconnected= TRUE;

    	state_machine_ua(fd, ua);	
	stat_rec_ua++;
	}    

    sleep(1);

    return res;
}

void disc_function(char *disc)
{
    disc[0] = FLAG;
    disc[1] = A_DISC;
    disc[2] = C_DISC;
    disc[3] = BCC_DISC;
    disc[4] = FLAG;
}

void state_machine_disc(int fd, char* disc)
{
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
        //printf("case: STATE_MACHINE_START1\n");
        if(c == FLAG)
        {
          disc[0] = c;
          state = FLAG_RCV;
        } break;
      case FLAG_RCV:
        if(c == A_DISC)
        {
          disc[1] = c;
          state = A_RCV;
        }
        else
        {
          memset(disc,0,5);
          state = STATE_MACHINE_START;
        } break;
      case A_RCV:
        if(c == C_DISC)
        {
          disc[2] = c;
          state = C_RCV;    
        }
        else if(c == FLAG)
        {
          state= FLAG_RCV;
        }
        else
        {
          memset(disc,0,5);
          state = STATE_MACHINE_START;
        } break;
      case C_RCV:
        if(c == BCC_DISC)
        {
          disc[3] = c;
          state = BCC_OK;
        }
        else if(c == FLAG)
        {
          state= FLAG_RCV;
        }
        else
        {
          memset(disc,0,5);
          state = STATE_MACHINE_START;
        } break;
      case BCC_OK:
        if(c == FLAG)
        {
          disc[4] = c;
          state = STATE_MACHINE_STOP;       
        }
        else
        {
          memset(disc,0,5);
          state = STATE_MACHINE_START;
        } break;
      case STATE_MACHINE_STOP: 
      end=TRUE; 
      break;
    }
  }
}


