#include "project.h"

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
  
  inf[0]=FLAG;
  inf[1]=A_SET;
  inf[2]=C_SET;
  inf[3]=BCC_SET;
  memcpy(&inf[4], buffer, length);
  inf[4+length]=BCC_UA;
  inf[5+length]=FLAG;
  
  res=write(fd, inf, length);
  if(res!=length)
  {
    printf("Error sending information\n");
  }
  
  link_layer->numMessages++;
  
  return res;
}