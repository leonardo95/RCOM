#include "project.h"



int sendControl(int fd, char* filename){

	FILE* file = fopen(filename, "rb");
	fseek(file, 0, SEEK_END);
	int size = ftell(file);
	fseek(file, 0, SEEK_SET);
	char filesize[255];
	sprintf(filesize, "%d", size);
	printf("%s\n" ,filesize);
	
	llwrite(fd, filesize, strlen(filesize));

	return 0;
}

int readControl(int fd){



}

/*
int main( int argc, char ** argv){
	int fd = atoi(argv[1]);
	char* filename = argv[2];
	sendControl(fd, filename);
	return 0;
}
*/
