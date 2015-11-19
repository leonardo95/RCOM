#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 
#define FALSE 0
#define TRUE 1
#define FLAG 0x7E
#define C_SET 0x07
#define A_SET 0x03
#define BCC_SET (0x03^0x07)
#define A_UA 0x01
#define C_UA 0x03
#define BCC_UA (0x01^0x03)
#define A_DISC 0x03
#define C_DISC 0x0B
#define BCC_DISC (0x03^0x0B)
#define START 0
#define FASE_1 1
#define FASE_2 2
#define FASE_3 3
#define FINISH 4
#define TRANSMITER 1
#define RECEIVER 0
#define ESCAPE 0x7d
#define DATASIZE 1024
#define END 0x5E
#define FILE_SIZE_CONTROL 0
#define FILE_NAME_CONTROL 1
#define CONTROL_START 1
#define CONTROL_END 2

#define C_RR(n) ((n << 5) | 1)
#define C_REJ(n) ((n << 5) | 5)
#define C_I(n) ((n << 5) | 0)

#define IS_RR(byte) (byte == C_RR(0) || byte == C_RR(1) )
#define IS_REJ(byte) ( byte == C_REJ(0) || byte == C_REJ(1) )
#define IS_I(byte) ( byte == C_I(0) || byte == C_I(1) )

#define GET_C(n) (n >> 5)

#define ISCOMAND(c) (c == C_SET || c == C_DISC || IS_I(c) )

#define ISREPLY(c) (c == C_UA || IS_RR(c) || IS_REJ(c) )

#define A_DECIDE(c, role) role == TRANSMITER ? (ISCOMAND(c)? 0x01 : 0x03) : ( ISREPLY(c)? 0x01 : 0x03)
typedef enum {
	STATE_MACHINE_START, FLAG_RCV, A_RCV, C_RCV, BCC_OK, STATE_MACHINE_STOP
} State;

typedef struct 
{
  int fileDescriptor;
  int status;
  char* filename;
}applicationLayer;

typedef struct {
  char port[20];
  int baudRate;
  unsigned int role;
  unsigned int sequenceNumber;
  unsigned int timeout;
  unsigned int numTransmissions;
  unsigned int numMessages;
  char frame[DATASIZE];
} linkLayer;

linkLayer link_layerInit;
linkLayer* link_layer;

int alarm_off;
int stat_send_set;
int stat_send_ua;
int stat_send_disc;
int stat_send_control;
int stat_send_i;
int stat_send_rr;
int stat_send_rej;
int stat_rec_set;
int stat_rec_ua;
int stat_rec_disc;
int stat_rec_control;
int stat_rec_rr;
int stat_rec_rej;
int stat_rec_i;

void set_function(char *set);
void state_machine_ua(int fd, char* ua);
void ua_function(char *ua);
void state_machine_set(int fd, char* set);
int llopen_reciever(int fd);
int llopen_transmitter(int fd);
int llclose_transmitter(int fd);
void disc_function(char *disc);
void state_machine_disc(int fd, char* disc);
int llclose_reciever(int fd);
int llwrite(int fd, char* buffer, int length, int role);
void signal_handler(int signal);
void signal_set();
void signal_stop();
int llopen(int port_num, int flag);
int llclose(int fd, int type);
int check_frame(char* frame, int framesize, int role, int frame_nr);
int sendframe(int fd, char* buffer, int frame_type, char* data, int datasize, int frame_nr);
int create_frame(char * buffer, int role, int frame_type, int frame_nr, char* data, int datasize);
int C_check(char c, int frame_nr);
int stuffing(char** frame, int framesize);
int destuffing(char** frame, int framesize, char* packet);
int receiveframe(int fd, char* frame);
int retrievedata(char* frame, int frame_size, char* new_frame);
int llread(int fd, char* buffer, int flag_type);
int ini_link_layer(char* port,int baudrate, unsigned int role, int timeout);
int receiveFile(int fd,int port);
int sendFile(int fd, char* filename);
int start_applicationlayer(char* port, int role, char* filename);
void stats();
