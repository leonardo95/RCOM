#include "project.h"

applicationLayer* app_layer;

int start_applicationlayer(char* port, int role, char* filename)
{
	app_layer = (applicationLayer*) malloc(sizeof(applicationLayer));

	app_layer->fileDescriptor= llopen(atoi(port),role);

	app_layer->status= role;

	app_layer->filename = filename;

	ini_link_layer(port, BAUDRATE, role, 3);
	switch(role){

		case RECEIVER:
			receiveFile(app_layer->fileDescriptor, *port);
			break;

		case TRANSMITER:
			sendFile(app_layer->fileDescriptor, app_layer->filename);
			break;

		default:
			break;

	}

	llclose(app_layer->fileDescriptor, app_layer->status);


	return 0;
}

int packetControl_send(int fd,int C, int role, char* filename, char* filesize)
{
	int counter=0, i=0;
	int packSize = 5 + strlen(filesize) + 1 + strlen(filename) + 1;
	char packControl[packSize];
	packControl[counter]=C;
	counter++;
	packControl[counter]=FILE_SIZE_CONTROL;
	counter++;
	packControl[counter]=strlen(filesize) + 1;
	counter++;
	for(i=0;i<strlen(filesize) + 1;i++)
	{
		packControl[counter]=filesize[i];
		counter++;
	}
	packControl[counter]= FILE_NAME_CONTROL;
	counter++;
	packControl[counter]=strlen(filename);
	counter++;

	for(i=0;i<strlen(filename) + 1;i++)
	{
		packControl[counter++]=filename[i];
	}
	printf("-> ");
	for(i=0;i<packSize;i++)
	{
		printf(" %x ", packControl[i]);
	}
	printf("\n");

	llwrite(fd, packControl, packSize, role);
	return 1;
}

//retorna tamanho do ficheiro
int packageControl_recieve(int fd, int role, int* packet_type, char* filename, int* filelength)
{
	printf("package control receive 1\n");
	char* packet = malloc((DATASIZE+1)*2 + 4);
	printf("package control receive 2\n");
	llread(fd, packet,role);
	printf("package control receive 3\n");
	int i=0, pos =0;
	for(;i<(DATASIZE+1)*2 + 4;i++)
	{
	  printf("%x ", packet[i]);
	}
	printf("\n");
	i=0;
		
	*packet_type = packet[pos];		
	pos++;
	printf("package control receive 4\n");
	if(packet[pos] != FILE_SIZE_CONTROL){
		printf("error: expected file size.");
		return 0;
	}
	pos++;
	printf("package control receive 5\n");	
	int numbytes = (int)packet[pos];
	pos++;
	char length[numbytes];
	for(i=0; i < numbytes; i++){
		length[i] = packet[i+pos];
	}
	pos += i;
	printf("package control receive 6\n");
	*filelength = atoi(length);
	printf("FILENLENGTH: %d\n", *filelength);
	printf("packet pos: %x\n", packet[pos]);
	printf("FILE NAME CONTROL: %x\n", FILE_NAME_CONTROL);
	if(packet[pos]!=FILE_NAME_CONTROL){
		printf("error: expected file name.\n");
		
		return 0;
	}
	pos++;
	printf("package control receive 7\n");
	numbytes = packet[pos];
	printf("%d\n", numbytes);
	pos++;
	for(i=0; i < numbytes; i++){
		filename[i] = packet[i+pos];
	}
	printf("\n");
	printf("package control receive 8\n");
	return 1;

}


int sendDataPacket(int fd, int N, char* buffer, int length){

	unsigned int packetsize = length + 4;

	char* packet = (char*) malloc(packetsize);


	packet[0] = 0;				//C
	packet[1] = N;
	packet[2] = length / 256;	//L2
	packet[3] = length % 256;	//L1

	memcpy(&packet[4], buffer, length);

	if(!llwrite(fd, packet, packetsize, link_layer->role)){
		free(packet);

		printf("error writing data packet\n");

		return 0;
	}

	free(packet);

	return 1;

}

int receiveDataPacket(int fd, int* N, char** buffer, int* length){

	char* packet=(char*) malloc(*length);
	printf("receiveDataPacket 1\n");
	unsigned int packetsize = llread(fd, packet, app_layer->status);
	printf("receiveDataPacket 2\n");
	if(packetsize < 0){
		printf(" error reading data packet\n");
		return 0;
	}
	printf("receiveDataPacket 3\n");
	if(packet[0] != 0){
		printf("error: not a data packet\n");
		return 0;
	}
	printf("receiveDataPacket 4\n");
	*N = (char) packet[1];
	printf("receiveDataPacket 5\n");
	*length = 256 * packet[2] + packet[3]; // K = 256 * L2 + L1
	printf("receiveDataPacket 6\n");
	printf("length: %d\n",*length);
	printf("packet[4]: %s\n",&packet[4]);
	memcpy(*buffer, &packet[4], *length-5);
	printf("receiveDataPacket 7\n");
	return 1;

}


int sendFile(int fd, char* filename) {
	printf("SENDING FILE\n");

	FILE* file = fopen(filename, "rb");

	int sizeread =0 , N =0;
	fseek(file, 0L, SEEK_END);
	unsigned int size = ftell(file);
	printf("FILE SIZE: %d \n", size);
	fseek(file, 0L, SEEK_SET);
	char fileSize[sizeof(int) *3 +2] = "";
	snprintf(fileSize, sizeof fileSize, "%d", size);
	printf("%s \n", fileSize);
	if(!packetControl_send(fd, CONTROL_START,  link_layer->role, filename, fileSize) ){
		printf("error sending start control packet\n");
		return 0;
	}
	printf("PACKAGE CONTROL SENT\n");
	char* filebuffer = malloc(DATASIZE);



	while( (sizeread = fread(filebuffer,sizeof(char), DATASIZE, file)) > 0 ){

		if(!sendDataPacket(fd, N % 255, filebuffer, sizeread)){
			return 0;
		}

		memset(filebuffer, 0, DATASIZE);
		puts(filebuffer);
	}

	fclose(file);

	if(!packetControl_send(fd, CONTROL_END, link_layer->role, filename, fileSize)){
		printf("error sending end control packet\n");
		return 0;
	}

	//llclose(fd, link_layer->role);
	return 1;	

}


int receiveFile(int fd,int port){

	int packet_start, filesize;
	char filename[100];
	printf("receive file 1\n");
	if(!packageControl_recieve(fd, link_layer->role, &packet_start, filename, &filesize)){
		printf("error receiving start control packet\n");
		return 0;
	}
	printf("receive file 2\n");
	if(packet_start != CONTROL_START){
		printf("error: expected start control packet\n");
		return 0;
	}
	printf("receive file 3\n");
	printf("FILENAME=%s\n", filename);
	FILE* newFile = fopen(filename, "wb");
	
	int size_read =0; 
	int N=0;
	
	while(size_read != filesize){
		char* filebuffer = malloc(DATASIZE);
		int data_length =0;
		if(0 == receiveDataPacket(fd, &N, &filebuffer, &data_length)){
			printf("error receiving %d data packet\n", N);
			return 0;
		}
		printf("receiveFile 1\n");
		fwrite(filebuffer, sizeof(char), data_length , newFile);
		printf("receiveFile 2\n");
		size_read += data_length;
		printf("receiveFile 3\n");
	}

	fclose(newFile);

	int packet_end;


	if(!packageControl_recieve(fd, link_layer->role, &packet_end, NULL, 0)){
		printf("error receiving end control packet\n");
		return 0;
	}

	if(packet_end != CONTROL_END){
		printf("error: expected end control packet\n");
		return 0;
	}


	
	//llclose(fd, link_layer->role);

	return 1;

}
