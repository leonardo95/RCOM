#include "project.h"

int main(int argc, char** argv)
{
	
 link_layer = &link_layerInit;
  char* port=argv[1];
  int flag=atoi(argv[2]);
  char* filename=argv[3];
  
  start_applicationlayer(port, flag, filename);

  stats();
  return 0;
}

void stats()
{
  printf("\n\n***************************** \n\n");
  
  printf("************SENT*************\n");
  printf("UA %d\n", stat_send_ua);
  printf("SET %d\n", stat_send_set);
  printf("DISC %d\n", stat_send_disc);
  printf("CONTROL %d\n", stat_send_control);
  printf("I %d\n", stat_send_i);
  printf("RR %d\n", stat_send_rr);
  printf("REJ %d\n", stat_send_rej);
  printf("\n");
  
  printf("*********RECEIVED************\n");
  printf("UA %d\n", stat_rec_ua);
  printf("SET %d\n", stat_rec_set);
  printf("DISC %d\n", stat_rec_disc);
  printf("CONTROL %d\n", stat_rec_control);
  printf("I %d\n", stat_rec_i);
  printf("RR %d\n", stat_rec_rr);
  printf("REJ %d\n", stat_rec_rej);
  printf("\n");
  
  printf("***************************** \n\n");
}
