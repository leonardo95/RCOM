#include "project.h"

int ini_link_layer(char* port,int baudrate, unsigned int role, int timeout){
  
  char new_port[255];
  strcpy(new_port, "/dev/ttyS");
  strcat(new_port, port);
  strcpy(link_layer->port, new_port);
  link_layer->baudRate = baudrate;
  link_layer->role = role;
  link_layer->sequenceNumber = 0;
  link_layer->timeout = timeout;

  return 0;

}

int create_frame(char * buffer, int role, int frame_type, int frame_nr, char* data, int datasize){

  buffer[0] = FLAG;
  buffer[1] = A_DECIDE(frame_type, role);
  if(frame_type == C_DISC || frame_type == C_SET || frame_type == C_UA)
{
    buffer[2] = frame_type;
}
  else if(frame_type == C_RR(0) || frame_type == C_RR(1))
  {
    buffer[2] = C_RR(frame_nr);
    stat_send_rr++;
  }
  else if(frame_type == C_REJ(0) || frame_type == C_REJ(1))
{
    buffer[2] = C_REJ(!frame_nr);
    stat_send_rej++;
}
  else if(frame_type == C_I(0) || frame_type == C_I(1))
{
    buffer[2] = C_I(frame_nr);
}

  buffer[3] = buffer[1] ^ buffer[2];
  if(frame_type == C_I(0) || frame_type == C_I(1)){
    memcpy(buffer + 4, data, datasize);
  
    char bcc2 =0;
    int i=0;
    for(; i < datasize; i++){
      bcc2 ^= data[i];
    }
    
    buffer[4+datasize] = bcc2;
    buffer[4+datasize+1] = FLAG;
  }else{
    buffer[4] = FLAG;
  }
 
  if(role==0)
  {
	return 4+datasize+1;
  }
	return 4+datasize+2;
}

int C_check(char c, int frame_nr){
  if(c != C_REJ(frame_nr) && c != C_RR(frame_nr) && c != C_I(0) && c != C_I(1) && c != C_SET && c != C_UA  && c != C_DISC)
    return 0;
  else return 1;
}

int check_frame(char* frame, int framesize, int role, int frame_nr){

  if(frame[0] != FLAG){
    printf("frame not starting in FLAG\n" );
    return 0;
  }
  if(frame[1] != (A_DECIDE(frame[2], !role))){
    printf("wrong A\n" );
    return 0;
  }

  if(!C_check(frame[2] , frame_nr)){
    printf("wrong C\n");
  }

  char bcc1 = frame[1] ^ frame[2];
  if(frame[3] != bcc1){
    printf("BCC1 error\n");
    return 0;
  }

  char bcc2=0;
  int i=4;
  for(; i < framesize - 2; i++){
    bcc2 ^= frame[i];
  }

  if(frame[framesize-2] != bcc2){
    printf("BCC2 error\n");
    return 0;
  }
  if(frame[framesize-1] != FLAG){
    printf("frame not ending in FLAG.\n");
    return 0;
  }
  return 1;
}

int stuffing(char** frame, int framesize){
  int  newSize = framesize;
  int i=0;

  for(i=4; i < framesize - 1; i++){
     if( (*frame)[i] == FLAG || (*frame)[i] == ESCAPE ){
      newSize++;
     }
  }
  *frame = (char*) realloc(*frame, newSize);
  for(i=4; i < newSize-1; i++){
    if( (*frame)[i] == FLAG || (*frame)[i] == ESCAPE ){
      memmove(*frame + i + 1, *frame + i, framesize - i);
      framesize++;

      (*frame)[i] = ESCAPE;
      (*frame)[i+1] ^= 0x20; 
     }
  }
  return newSize;
}


int destuffing(char** frame, int framesize, char* packet){
  int i=0; 
  int size=0;
  for(i=4; i<= framesize; i++){
    if((*frame)[i] == ESCAPE )
	{
		i++;
     		packet[size]=(*frame)[i] ^0x20;
		size++;      
    	}
	else
	{
	packet[size]=(*frame)[i];
	size++;
	}
  }

  return size;
}


int llwrite(int fd, char* buffer, int length, int role)
{
  int try = 0, done_transfering =0;
  char* frame = malloc((DATASIZE+1)*2 + 4);
  while(!done_transfering){
    if(try == 0 || alarm_off == 1){
        alarm_off =0;

        if(try >= 3){
          signal_stop();
          printf("Max number of tries reached\n");
          return -1;
        }
        int suc = sendframe(fd, frame, C_I(link_layer->sequenceNumber), buffer, length, link_layer->sequenceNumber);
	stat_send_i++;
	
        if(try == 0){
          signal_set();
          if(!suc){
            try++;
          }
        }
        char* received  = malloc((DATASIZE+1)*2 + 4);
        
        receiveframe(fd, received);
        
        if(IS_RR(received[2])){
	  stat_rec_rr++;
	  if(link_layer->sequenceNumber == 0){
	            			  link_layer->sequenceNumber =1;
					
	            		}else if(link_layer->sequenceNumber == 1)
				{
	              			link_layer->sequenceNumber =0;
	            		}
          signal_stop();
          done_transfering=1;
        
        }else if(IS_REJ(received[2])){
	  stat_rec_rej++;
          signal_stop();
          try =0;
        }
    }
  }
  return length;
}

int sendframe(int fd, char* buffer, int frame_type, char* data, int datasize, int number){
  
  int buffersize =  create_frame(buffer, link_layer->role, frame_type, number, data, datasize);

  int framesize = stuffing(&buffer,  buffersize);

 int res = write(fd, buffer, framesize);
stuffing(&buffer,  buffersize);

  if(res != framesize){
    printf("error while sending file.\n");
    return 0;
  }
	
  return 1;
}

