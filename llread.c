#include "project.h"

#define START 0
#define FASE_1 1
#define FASE_2 2
#define FASE_3 3
#define FINISH 4

int llread(int fd, unsigned char* buffer)
{
	int j = 0, res;
	char state=START;

	while(state!=FINISH)
	{
		res = read(fd, buffer + j, sizeof(char));

		if (res != 1)
		{
			printf("Error Reading\n");
			return -1;
		}

		switch(state)
		{
			case START:
			if(buffer[j]==FLAG)
			{
				state = FASE_1;
				j++;
			}
			break;

			case FASE_1:
			if(buffer[j]==FLAG)
			{
				state = FASE_1;
			}
			else
			{
				j++;
				state = FASE_2;
			}
			break;

			case FASE_2:
			if(buffer[j] == FASE_1)
			{
				state = FASE_1;
				j=1;
			}
			else
			{
				state =FASE_3;
				j++;
			}
			break;

			case FASE_3:
			while(buffer[j] != FLAG)
			{
				j++;
				res = read(fd, buffer + j, 1); 

				if (res != 1)
				{
					printf("Error Reading\n");
					return -1;
				}
			}

			state=FINISH;
			break;
		}
	}
	state=START;
	
	return j;
}
