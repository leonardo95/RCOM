#include "project.h"

int main(int argc, char** argv)
{
  int res=0;
  
  if (argc < 2) 
  {
      printf("Invalid arguments for function llclose\n");
      exit(1);
  }
  
  int fd=atoi(argv[1]);
    
  res=llclose_transmitter(fd); 
  
  return res;
}

int llclose_transmitter(int fd)
{
    int res;
    char disc[5];
    char ua[5];

    disc_function(disc);

    res = write(fd,disc,strlen(disc));   
    printf("%d bytes written\n", res);

    state_machine_disc(fd, disc);
    
    ua_function(ua);

    res += write(fd,ua,strlen(ua));   
    printf("%d bytes written\n", res);

    return res;
}

void disc_function(char *disc)
{
  printf("disc_function -> initializing\n");
    disc[0] = FLAG;
    disc[1] = A_DISC;
    disc[2] = C_DISC;
    disc[3] = BCC_DISC;
    disc[4] = FLAG;
    printf("disc values: %x, %x, %x, %x, %x\n", disc[0],disc[1],disc[2],disc[3],disc[4]);
    printf("disc_function -> terminated\n");
}

void state_machine_disc(int fd, char* disc)
{
   printf("state_machine_disc -> initializing\n");
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
          disc[0] = c;
          state = FLAG_RCV;
        } break;
      case FLAG_RCV:
        printf("case: FLAG_RCV\n");
        if(c == A_DISC)
        {
          printf("state: A\n");
          disc[1] = c;
          state = A_RCV;
        }
        else
        {
          memset(disc,0,strlen(disc));
          state = STATE_MACHINE_START;
        } break;
      case A_RCV:
        printf("case: A_RCV\n");
        if(c == C_DISC)
        {
          printf("state: C\n");
          disc[2] = c;
          state = C_RCV;    
        }
        else if(c == FLAG)
        {
          printf("state: FLAG\n");
          state= FLAG_RCV;
        }
        else
        {
          memset(disc,0,strlen(disc));
          state = STATE_MACHINE_START;
        } break;
      case C_RCV:
        printf("case: C_RCV\n");
        if(c == BCC_DISC)
        {
          printf("state: BBC\n");
          disc[3] = c;
          state = BCC_OK;
        }
        else if(c == FLAG)
        {
          printf("state: FLAG\n");
          state= FLAG_RCV;
        }
        else
        {
          memset(disc,0,strlen(disc));
          state = STATE_MACHINE_START;
        } break;
      case BCC_OK:
      printf("case: BCC_OK\n");
        if(c == FLAG)
        {
          printf("state: FLAG\n");
          disc[4] = c;
          state = STATE_MACHINE_STOP;       
        }
        else
        {
          memset(disc,0,strlen(disc));
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

int llclose_reciever(int fd)
{
    int res;
    char ua[5];
    char disc[5];

    ua_function(ua);

    state_machine_disc(fd, disc);

    res=write(fd,ua,strlen(ua));
    printf("%d bytes written\n", res);

    sleep(1);

    return res;
}