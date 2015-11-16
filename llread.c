#include "project.h"

int llread(int fd, char* buffer, int flag_type)
{
	int reading=TRUE;
	char* frame=malloc((DATASIZE+1)*2 + 4);
	
	char* new_buf=malloc(10);
	while(reading==TRUE)
	{	
		int frame_size = receiveframe(fd, frame);
		if (frame_size == -1) {
			printf("Nothing received or received with errors.\n");
		}

		if(IS_I(frame[2])){
		printf("sn: %d\n", link_layer->sequenceNumber);
			if(GET_C(frame[2]) == link_layer->sequenceNumber)
			{
				char* frame_data = frame;
				printf("Aqui 1 \n");
				int frame_data_size = retrievedata(frame_data, frame_size, buffer);
				printf("Aqui 2 \n");
				int w = 0;
for (; w < frame_data_size; w++) printf("%x ", buffer[w]); printf("\n");
				
				char nulo[1];
				nulo[0] = '\0';
			
				printf("TESTE1\n");
				if(link_layer->sequenceNumber == 0){
	            			  link_layer->sequenceNumber =1;
					
	            		}else if(link_layer->sequenceNumber == 1)
				{
	              			link_layer->sequenceNumber =0;
	            		}
				

				sendframe(fd, new_buf, C_RR(link_layer->sequenceNumber), nulo, 0, link_layer->sequenceNumber);
				stat_send_rr++;
				printf("llread 1\n");
					

				return frame_size;
			}
			else
			{	
				char nulo[1];
				nulo[0] = '\0';
				sendframe(fd, new_buf, C_REJ(link_layer->sequenceNumber), nulo, 0, link_layer->sequenceNumber);
				printf("---------------TESTE2\n");
				stat_send_rej++;
				
			}
		}
		if(frame[2]==C_SET)
		{
			printf("llread 3\n");
			char ua[5];
			ua_function(ua);
	
			return write(fd,ua,5);
		}
		
	}
		printf("llread 2\n");
	return -1;
}

int retrievedata(char* frame, int frame_size, char* new_frame){
	int i;
	for(i=0; i < frame_size-5; i++){
		new_frame[i] = frame[i+4];
	}
	return frame_size-5;
}																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																							
