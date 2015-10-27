#include "project.h"

int llread(int fd, char* buffer, int flag_type,int number)
{
	int try=0, reading=TRUE, res=0, counter=0, flag=0, type=-1;
	char frame[(DATASIZE+1)*2 + 4];
	
	signal_set();
	while(reading==TRUE)
	{	

		unsigned int c;

		frame = receiveframe(fd);
	}

	res=check_frame(frame, counter-1, flag_type, number);
	if(res==0)
	{
		if(type==0)
		{
			if(GET_C(frame[3])==number)
			{
				//frame[3]=C_RR(frame[3]);
				buffer=frame;
				buffer[3] = C_RR(buffer[3]);
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
