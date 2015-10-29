#include "project.h"

int llread(int fd, char* buffer, int flag_type)
{
	printf("initating llread function\n");
	int reading=TRUE, res=0;
	char frame[(DATASIZE+1)*2 + 4];
	
	signal_set();

	while(reading==TRUE)
	{	
		printf("LLREAD initating receiveframe test\n");
		int frame_size = receiveframe(fd, frame);
		if(res==0)
		{
			if(IS_I(frame[2])){	
				if(GET_C(frame[2]) == 0)
				//if(GET_C(frame[2]) == link_layer->sequenceNumber)
				{
					char* frame_data=malloc(frame_size);
					frame_data=frame;
					printf("initating retrievedata test\n");
					int frame_data_size = retrievedata(frame_data, frame_size);
					memcpy(buffer, frame_data, frame_data_size);
					char nulo[1];
					nulo[0] = '\0';
					printf("Initating sendframe test\n");
					//sendframe(fd, buffer, C_RR(0), nulo, 0, link_layer->sequenceNumber);
					printf("BUFFER[2]= %x\n", buffer[2]);
					printf("->C_RR=%x\n", C_RR(0));
					sendframe(fd, buffer, C_RR(0), nulo, 0, 0);
					//if(link_layer->sequenceNumber == 0){
						if(0 == 0){
		              //ink_layer->sequenceNumber =1;
		            //}else if(link_layer->sequenceNumber == 1){
		          }else if(0 == 1){
		              //link_layer->sequenceNumber =0;
		            }
					return frame_size;
				}
				else
				{
					char nulo[1];
					nulo[0] = '\0';
					sendframe(fd, buffer, C_REJ(!link_layer->sequenceNumber), nulo, 0, link_layer->sequenceNumber);
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

int retrievedata(char* frame, int frame_size){
	printf("initating retrievedata function\n");
	char* new_frame=malloc(frame_size - 6);
	int i;
	for(i=0; i < frame_size-6; i++){
		new_frame[i] = frame[i+4];
	}
	printf("retrievedata terminated succefully\n");
	return frame_size-6;
}																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																							