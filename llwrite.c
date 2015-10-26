#include "project.h"


int create_frame(char * buffer, int role, int frame_type, int frame_nr, char* data, int datasize){

  buffer[0] = FLAG;
  buffer[1] = A_DECIDE(frame_type, role);
  if(frame_type == C_DISC || frame_type == C_SET || frame_type == C_UA)
    buffer[2] = frame_type;
  else if(frame_type == C_RR(0))
    buffer[2] = C_RR(frame_nr);
  else if(frame_type == C_REJ(0))
    buffer[2] = C_REJ(frame_nr);
  else if(frame_type == C_I(0))
    buffer[2] = C_I(frame_nr);
  buffer[3] = buffer[1] ^ buffer[2];
  if(frame_type == C_I(0)){
    memcpy(buffer + 4, data, datasize);
  
  char bcc =0;
  int i=0;
  for(; i < datasize; i++){
    bcc ^= data[i];
  }
  
  buffer[4+datasize] = bcc;
  buffer[4+datasize+1] = FLAG;
}else{
  buffer[4] = FLAG;
}
}

int C_check(char c, int frame_nr){
  if(c != C_REJ(frame_nr) || c != C_RR(!frame_nr) || c != C_I(0) || c != C_I(1) || c != C_SET || c != C_UA  || c != C_DISC)
    return -1;
  else return 0;
}

int check_frame(char* frame, int framesize, int role, int frame_nr){

  if(!frame[0] != FLAG){
    printf("frame not starting in FLAG\n" );
    return -1;
  }

  if(frame[1] != A_DECIDE(frame[2], role)){
    printf("wrong A\n" );
    return -1;
  }

  if(C_check(frame[2]) , frame_nr){
    printf("wrong C\n");
  }

  char bcc1 = frame[1] ^ frame[2];
  if(frame[3] != bcc1){
    printf("BCC1 error\n");
    return -1;
  }

  char bcc2=0;
  int i=4;
  for(; i < framesize - 2; i++){
    bcc ^= frame[i];
  }

  if(frame[framesize-2] != bcc2){
    printf("BCC2 error\n");
    return -1;
  }

  if(frame[framesize-1] != FLAG){
    printf("frame not ending in FLAG.\n");
    return -1;
  }

  return 0;
}

int llwrite(int fd, char* buffer, int length)
{
  int res=0, transffer=1, transmittion=0;
  
  while(transffer==1)
  {
    if(transmittion==0)
    {
      if(transmittion >= link_layer->numTransmissions)
      {
	       printf("Unable to send information number of transmissions exceeded\n");
	       res=-1;
	       return res;
      }
      
      res=send_inf(fd, buffer, length);
    }
    transffer=0;
  }
  
  return res;
}

int send_inf(int fd, char* buffer, int length)
{
  char* inf;
  int res;
  
 
  
  res=write(fd, inf, length);
  if(res!=length)
  {
    printf("Error sending information\n");
  }
  
  link_layer->numMessages++;
  
  return res;
}