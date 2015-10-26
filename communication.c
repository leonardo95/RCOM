#include "project.h"

int main(int argc, char** argv)
{
  int port=atoi(argv[1]);
  int flag=atoi(argv[2]);
  
  llopen(port, flag);
  return 0;
}