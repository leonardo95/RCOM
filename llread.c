#include "project.h"

int llread(int fd, char* buffer, int flag,int number)
{
	int try=0, reading=TRUE, res=0, counter=0, flag=0, type=-1;
	char* frame;
	
	signal_set();
	while(reading==TRUE)
	{	
		if(try >= 3)
		{
		signal_stop();
		printf("Maximum number of tries reached, aborting read.\n");
		return -1;
		}

		unsigned int c;

		res = read(fd, &c, 1);
		if(res!=1)
		{
			printf("Error recieving\n");
		}

		if(counter == 3)
		{
			if(IS_I(c))
			{
				type=0;
			}
			else if(IS_RR(c))
			{
				type=1;
			}
			else if(IS_REJ(c))
			{
				type=2;
			}
		}
		frame[counter]=c;

		if(c==FLAG)
		{
			flag++;
		}

		if(flag==2)
		{
			reading=FALSE;
		}

		counter++;
		try++;
	}

	res=check_frame(frame, counter-1, flag, number);
	if(res==0)
	{
		if(type==0)
		{
			if(GET_C(frame[3])==number)
			{
				frame[3]=C_RR(frame[3]);
				buffer=frame;
				res=write(fd,frame,strlen(frame));
				return res;
			}
			else
			{
				buffer=frame;
				res=write(fd,frame,strlen(frame));
				return res;
			}
		}
		if(frame[3]==C_SET)
		{
			char ua[5];
			ua_function(ua);
			res=write(fd,ua,strlen(ua));
			return res;
		}
	}
	return -1;
}
