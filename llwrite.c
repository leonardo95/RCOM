#include "project.h"

int ini_link_layer(char* port,int baudrate, unsigned int role, int timeout){
  
  char new_port[255];
  strcpy(new_port, "/dev/ttyS");
  strcat(new_port, port);
  //printf("%s\n", new_port);
  //printf("%s\n", link_layer->port);
  strcpy(link_layer->port, new_port);
  // printf("TESTE 2\n");
  link_layer->baudRate = baudrate;
  // printf("TESTE 3\n");
  link_layer->role = role;
  // printf("TESTE 4\n");
  link_layer->sequenceNumber = 0;
  // printf("TESTE 5\n");
  link_layer->timeout = timeout;
  // printf("TESTE 6\n");

  return 0;

}

int create_frame(char * buffer, int role, int frame_type, int frame_nr, char* data, int datasize){

  buffer[0] = FLAG;
  buffer[1] = A_DECIDE(frame_type, role);
  if(frame_type == C_DISC || frame_type == C_SET || frame_type == C_UA)
    buffer[2] = frame_type;
  else if(frame_type == C_RR(0))
  {
    buffer[2] = C_RR(frame_nr);
  }
  else if(frame_type == C_REJ(0))
    buffer[2] = C_REJ(frame_nr);
  else if(frame_type == C_I(0))
    buffer[2] = C_I(frame_nr);

  buffer[3] = buffer[1] ^ buffer[2];
  if(frame_type == C_I(0)){
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
  int i=0;
  printf("---->");
   for(i=0;i<datasize;i++)
  {
          printf(" %x ", data[i]);
  }
  printf("\n");
  if(role==0)
  {
   return 4+datasize+1;
  }
   return 4+datasize+2;
}

int C_check(char c, int frame_nr){
  if(c != C_REJ(frame_nr) && c != C_RR(!frame_nr) && c != C_I(0) && c != C_I(1) && c != C_SET && c != C_UA  && c != C_DISC)
    return -1;
  else return 0;
}

int check_frame(char* frame, int framesize, int role, int frame_nr){

  printf("initiating check_frame function\n");
  if(frame[0] != FLAG){
    printf("frame not starting in FLAG\n" );
    return 0;
  }
  if(frame[1] != (A_DECIDE(frame[2], !role))){
    printf("wrong A\n" );
    return 0;
  }

  if(C_check(frame[2] , frame_nr)){
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
  printf("check_frame terminated succefully\n");
  return 1;
}

int stuffing(char** frame, int framesize){
  int  newSize = framesize;
  int i=0;

  for(i=4; i < framesize-1; i++){
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


int destuffing(char** frame, int framesize){
  printf("initiating destuffing function\n");
  int i=0;
  for(i=1; i< framesize; i++){
    if( (*frame)[i] == ESCAPE ){
      memmove(*frame +i , *frame + i + 1, framesize -i -1);

      framesize--;

      (*frame)[i] ^= 0x20;
    }
  }

  *frame = (char*) realloc(*frame, framesize);
  printf("destuffing terminated succefully\n");
  return framesize;
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
        int i=0;
        printf("-->\n");
        for(i=0;i<length;i++)
        {
          printf(" %x ", buffer[i]);
        }
        printf("\n");
        int suc = sendframe(fd, frame, C_I(0), buffer, length, link_layer->sequenceNumber);
        if(try == 0){
          signal_set();
          if(!suc){
            try++;
          }
        }
        char* received  = malloc((DATASIZE+1)*2 + 4);
        
        receiveframe(fd, received);
        printf("TESTE\n");
        if(IS_RR(received[2])){
          signal_stop();
          done_transfering=1;
        
        }else if(IS_REJ(received[2])){
          signal_stop();
          try =0;
        }
    }
  }
  printf("AQUI %d\n", length);
  return length;
}

int sendframe(int fd, char* buffer, int frame_type, char* data, int datasize, int number){

  int j=0;
  printf("---> ");
   for(j=0;j<datasize;j++)
  {
    printf(" %x ", data[j]);
  }
  printf("\n");
  int buffersize =  create_frame(buffer, link_layer->role, frame_type, number, data, datasize);
  
  printf("-----> ");
   for(j=0;j<buffersize;j++)
  {
    printf(" %x ", buffer[j]);
  }
  printf("\n");

  int framesize = stuffing(&buffer,  buffersize);

  printf("------> ");
   for(j=0;j<framesize;j++)
  {
    printf(" %x ", buffer[j]);
  }
  printf("\n");

 int res = write(fd, buffer, framesize);

printf("REEEEEEES  %d\n", res);

  if(res != framesize){
    printf("error while sending file.\n");
    return 0;
  }
  return 1;
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
      printf("%x\n", c);
      if(res == 0){
        printf("nothing received.\n");
        return ind;
      }
    }

    switch (state) {
      case START: 
      printf("case: STATE_MACHINE_START\n");
        if(c == FLAG){
          printf("state: FLAG\n");
          state = FLAG_RCV;

          //ind++;
          frame[ind] = c;
        }
        break;
      case FLAG_RCV:
      printf("case: FLAG_RCV\n");
        if(c == A_UA){
          printf("state: A\n");
          state = A_RCV;
          ind++;
          frame[ind] = c;
        }else if(c != FLAG){
          ind =0;
          state = START;
        }
        break;
      case A_RCV:
      printf("case: A_RCV\n");
        if(c != FLAG){
          printf("state: C\n");
          state = C_RCV;
          ind++;
          frame[ind] = c;
        }
        else if(c == FLAG){
          printf("state: FLAG\n");
          ind = 1;
          state = FLAG_RCV;
        }else{
          ind = 0;
          state = START;
        }
        break;
        case C_RCV:
         printf("case: C_RCV\n");
	printf("AQUIIII %x\n", frame[1]^frame[2]);
          if(!(frame[1]^frame[2])){
          ind++;
          frame[ind] = c; 
          state = BCC_OK;
          }
          else if (c == FLAG){
            printf("state: FLAG\n");
          ind = 1;
          state = FLAG_RCV;
          } else{
            ind = 0;
            state = START;
          }
          break;
          break;
          case BCC_OK:
           printf("case: BCC_OK\n");
            if(c == FLAG){
               ind++;
              frame[ind] = c;
              state = STATE_MACHINE_STOP;
            }
            else if (c != FLAG){
              ind++;
              frame[ind] = c; 
            }
            break;
          case STATE_MACHINE_STOP:

       printf("state: STATE_MACHINE_STOP\n");
              done = TRUE;
              break;
          default:
              break;
            }
          }

          printf("initiating check_frame test\n");
          if(check_frame(frame, ind+1,link_layer->role,link_layer->sequenceNumber)){

          //if(check_frame(frame, ind+1,1,0)){
            signal_stop();
          printf("receiveframe terminated succefully\n");

            return ind;
          }else{

            if(try == 0){
              signal_set();
            }
            try++;

          }
          return 0;
}
