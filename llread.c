#include "project.h"

int llread(int fd, char* buffer, int flag_type)
{
	int reading=TRUE, res=0;
	char* frame=malloc((DATASIZE+1)*2 + 4);
	char* new_buf=malloc(10);
	//signal_set();

	while(reading==TRUE)
	{	
		int frame_size = receiveframe(fd, frame);
		if(res==0)
		{
			if(IS_I(frame[2])){
				if(GET_C(frame[2]) == link_layer->sequenceNumber)
				{
					char* frame_data=malloc(frame_size);
					frame_data=frame;

					char* new_frame=malloc(frame_size - 5);
					int frame_data_size = retrievedata(frame_data, frame_size, new_frame);				
					memcpy(buffer, frame_data, frame_data_size+6);
					
					char nulo[1];
					nulo[0] = '\0';
					
					sendframe(fd, new_buf, C_RR(link_layer->sequenceNumber), nulo, 0, link_layer->sequenceNumber);
					
					if(link_layer->sequenceNumber == 0){
		              link_layer->sequenceNumber =1;
		            }else if(link_layer->sequenceNumber == 1){
		              link_layer->sequenceNumber =0;
		            }
					return frame_size;
				}
				else
				{
					char nulo[1];
					nulo[0] = '\0';
					sendframe(fd, new_buf, C_REJ(!link_layer->sequenceNumber), nulo, 0, link_layer->sequenceNumber);
				}
			}
			if(frame[2]==C_SET)
			{
				char ua[5];
				ua_function(ua);
				res=write(fd,ua,5);
				return res;
			}
		}
	}
	return -1;
}

int retrievedata(char* frame, int frame_size, char* new_frame){
	int i;
	for(i=0; i < frame_size-5; i++){
		new_frame[i] = frame[i+4];
	}
	return frame_size-5;
}																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																							
