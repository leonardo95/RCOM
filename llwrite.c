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

return 4+datasize+2;
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

int stuffing(char** frame, int framesize){
  int  newSize = framesize;

  int i=0;

  for(i=1; i < framesize; i++){
     if( (*frame)[i] == FLAG || (*frame)[i] == ESCAPE ){
      newSize++;
     }
  }

  *frame = (char*) realloc(*buf, newSize);

  for(i=1; i < newSize; i++){
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
  int i=0;
  for(i=1; i< framesize; i++){
    if( (*frame)[i] == ESCAPE ){
      memmove(*frame +i , *frame + i + 1, framesize -i -1);

      framesize--;

      (*frame)[i] ^= 0x20;
    }
  }

  *frame = (char*) realloc(*frame, bufSize);

  return framesize;
}


int llwrite(int fd, char* buffer, int length)
{
  int try = 0, done_transfering =0;
  char* frame = malloc(255);

  while(!done_transfering){
    if(try == 0 || alarm_off == 1){
        alarm_off =0;

        if(try >= 3){
          signal_stop();
          printf("Max number of tries reached\n")
          return 0;
        }

        int suc = sendframe(fd, frame, role, C_I(0), buffer, length);

        if(try == 0){
          signal_set();
          if(!suc){
            try++;
          }
        }

        char* received = receiveframe(fd);

        if(IS_RR(received[2]){
          signal_stop();
          done_transfering=1;
          if(ll->ns == 0){
              ll->ns =1;
            }else if(ll->ns == 1){
              ll->ns =0;
            }
        }else if(IS_REJ(received[2])){
          signal_stop();
          try =0;
        }
    }
  }
}

int sendframe(int fd, const char* buffer, int frame_type, char* data, int datasize){
  int buffersize =  create_frame(buffer, ll->role, frame_type, ll->ns, data, datasize);

  int framesize = stuffing(&buffer,  buffersize);

  int res = write(fd, buffer, framesize);

  if(res != framesize){
    printf("error while sending file.\n");
    return 0;
  }

  return 1;
}


char* receiveframe(int fd){
  char* frame = malloc(255);

  State state= Start;

  int ind=0;
  int done = FALSE;

  while(!done){
    unsigned char c;

    if(state != STOP){

      int res = read(fd, &c, 1);

      if(res == 0){
        printf("nothing received.\n");
        return frame;
      }
    }

    switch (state) 
      case START: 
        if(c == FLAG){
          state = FLAG_RCV;
          ind++;

          frame[ind] = c;
        }
        break;
      case FLAG_RCV:
        if(c == A){
          state = A_RCV;
          ind++;
          frame[ind] = c;
        }else if(c != FLAG){
          ind =0;
          state = START;
        }
        break;
      case A_RCV:
        if(c != FLAG){

          state = C_RCV;
          ind++;
          frame[ind] = c;
        }
        else if(c == FLAG){
          ind = 1;
          state = FLAG_RCV;
        }else{
          ind = 0;
          state = START;
        }
        break;
        case C_RCV:
          if(frame[1]^frame[2]){
          ind++;
          frame[ind] = c; 
          state = BCC_OK;
          }
          else if (c == FLAG){
          ind = 1;
          state = FLAG_RCV;
          } else{
            ind = 0;
            state = START;
          }
          break;
          break;
          case BCC_OK:
            if(c == FLAG){
               ind++;
              frame[ind] = c;
              state = STOP;
            }else if (c != FLAG){
              ind++;
              frame[ind] = c; 
            }
            break;
          case STOP:

              frame[ind] = 0;
              done = true;
              break;
          default:
              break;
            }
          }

          if(check_frame(frame, ind+1, ll->role,ll->ns)){
            

            return frame;
          }

  }
}