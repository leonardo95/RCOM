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

	llwrite(fd, packControl, packSize, role);
	stat_send_control++;
	return 1;
}

//retorna tamanho do ficheiro
int packageControl_recieve(int fd, int role, int* packet_type, char* filename, int* filelength)
{

	char* packet = malloc((DATASIZE+1)*2 + 4);

	llread(fd, packet,role);

	int i=0, pos =0;
		
	*packet_type = packet[pos];		
	pos++;
	if(packet[pos] != FILE_SIZE_CONTROL){
		printf("error: expected file size.");
		return 0;
	}
	pos++;	
	int numbytes = (int)packet[pos];
	pos++;
	char length[numbytes];
	for(i=0; i < numbytes; i++){
		length[i] = packet[i+pos];
	}
	pos += i;
	*filelength = atoi(length);
	if(packet[pos]!=FILE_NAME_CONTROL){
		printf("error: expected file name.\n");
		
		return 0;
	}
	pos++;
	numbytes = packet[pos];
	pos++;
	for(i=0; i < numbytes; i++){
		filename[i] = packet[i+pos];
	}
	stat_rec_control++;
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
	unsigned int packetsize = llread(fd, packet, app_layer->status);
	if(packetsize < 0){
		printf(" error reading data packet\n");
		return 0;
	}
	if(packet[0] != 0){
		printf("error: not a data packet\n");
		return 0;
	}
	*N = (char) packet[1];
	*length = 256 * packet[2] + packet[3]; // K = 256 * L2 + L1
	memcpy(*buffer, &packet[4], *length);
	return 1;

}


int sendFile(int fd, char* filename) {

	FILE* file = fopen(filename, "rb");

	int sizeread =0 , N =0;
	fseek(file, 0L, SEEK_END);
	unsigned int size = ftell(file);
	fseek(file, 0L, SEEK_SET);
	char fileSize[sizeof(int) *3 +2] = "";
	snprintf(fileSize, sizeof fileSize, "%d", size);
	if(!packetControl_send(fd, CONTROL_START,  link_layer->role, filename, fileSize) ){
		printf("error sending start control packet\n");
		return 0;
	}
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

	return 1;	

}


int receiveFile(int fd,int port){

	int packet_start, filesize;
	char filename[100];
	if(!packageControl_recieve(fd, link_layer->role, &packet_start, filename, &filesize)){
		printf("error receiving start control packet\n");
		return 0;
	}
	if(packet_start != CONTROL_START){
		printf("error: expected start control packet\n");
		return 0;
	}
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
		fwrite(filebuffer, sizeof(char), data_length , newFile);
		size_read += data_length;
	}

	fclose(newFile);

	int packet_end;


	if(!packageControl_recieve(fd, link_layer->role, &packet_end, filename, &filesize)){
		printf("error receiving end control packet\n");
		return 0;
	}

	if(packet_end != CONTROL_END){
		printf("error: expected end control packet\n");
		return 0;
	}

	return 1;

}
