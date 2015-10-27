#include "project.h"

int llread(int fd, char* buffer, int flag_type,int number)
{
	int try=0, reading=TRUE, res=0, counter=0, flag=0, type=-1;
	char frame[(DATASIZE+1)*2 + 4];
	
	signal_set();

	while(reading==TRUE)
	{	

		

		int frame_size = receiveframe(fd, frame);

		res=check_frame(frame, counter-1, flag_type, number);
		if(res==0)
		{
			if(type==0)
			{
				if(GET_C(frame[2]) == number)
				{
					frame_data=frame;
					int frame_data_size = retrievedata(frame_data, frame_size);
					memcpy(buffer, frame_data, frame_data_size);
					char buf[6];
					char nulo[1];
					nulo[0] = '\0';
					//create_frame(buffer, link->role, C_RR(!link->sequenceNumber), link->sequenceNumber);
					sendframe(fd, buf, C_RR(!link->sequenceNumber), nulo, 0);
					if(link->sequenceNumber == 0){
		              link->sequenceNumber =1;
		            }else if(link->sequenceNumber == 1){
		              link->sequenceNumber =0;
		            }
					return 1;
				}
				else
				{
					sendframe(fd, buf, C_REJ(!link->sequenceNumber), nulo, 0);
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
	}
	return -1;
}

int retrievedata(char* frame, int frame_size){
	char* new_frame[frame_size - 6];
	int i;
	for(i=0; i < frame_size-6; i++){
		new_frame[i] = frame[i+4];
	}
	return frame_size-6;
}																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																							