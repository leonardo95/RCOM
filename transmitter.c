#include "project.h"

int transmitter(int fd)
{
    int res;
    char set[5];
    char ua[5];

    set_function(set);

    res = write(fd,set,strlen(set));   
    printf("%d bytes written\n", res);

    state_machine_ua(fd, ua); 
    sleep(1);

    return 0;
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
