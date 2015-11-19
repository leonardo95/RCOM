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
			if(GET_C(frame[2]) == link_layer->sequenceNumber)
			{
				char* frame_data = frame;
				retrievedata(frame_data, frame_size, buffer);
		
				
				char nulo[1];
				nulo[0] = '\0';
			
				if(link_layer->sequenceNumber == 0){
	            			  link_layer->sequenceNumber =1;
					
	            		}else if(link_layer->sequenceNumber == 1)
				{
	              			link_layer->sequenceNumber =0;
	            		}
				

				sendframe(fd, new_buf, C_RR(link_layer->sequenceNumber), nulo, 0, link_layer->sequenceNumber);
				stat_send_rr++;					

				return frame_size;
			}
			else
			{	
				char nulo[1];
				nulo[0] = '\0';
				sendframe(fd, new_buf, C_REJ(link_layer->sequenceNumber), nulo, 0, link_layer->sequenceNumber);

				stat_send_rej++;
				
			}
		}
		if(frame[2]==C_SET)
		{
			char ua[5];
			ua_function(ua);
	
			return write(fd,ua,5);
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



int receiveframe(int fd, char* frame){
  State state= START;

  int ind=0,try=0;
  int done = FALSE;
  while(!done){
    char c;

    if(state != STATE_MACHINE_STOP){
      if(try >= 3){
        signal_stop();
          printf("Max number of tries reached\n");
          return -1;

      }
      int res = read(fd, &c, 1);
      
      if(res == 0){
        printf("nothing received.\n");
        return -1;
      }
    }

    switch (state) {
      case START: 
      	
        if(c == FLAG)
	{
          state = FLAG_RCV;

          frame[ind] = c;
	  ind++;
        }
        break;
      case FLAG_RCV:
	frame[ind]=c;
	ind++;
	if(c==FLAG)
	{
	  state=STATE_MACHINE_STOP;
	  done=TRUE;
	}
	break;
      default:
	break;
	}
    }
	//char* packet = malloc((DATASIZE+1)*2 + 4);
	//destuffing(&frame, ind, packet);
	//memcpy(frame, packet, ind);
          return ind;
}																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																						
