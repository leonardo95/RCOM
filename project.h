#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define FLAG 0x7E
#define C_SET 0x07
#define A_SET 0x03
#define BCC_SET (0x03^0x07)
#define A_UA 0x01
#define C_UA 0x03
#define BCC_UA (0x01^0x03)

typedef enum {
	STATE_MACHINE_START, FLAG_RCV, A_RCV, C_RCV, BCC_OK, STATE_MACHINE_STOP
} State;


volatile int STOP=FALSE;

void set_function(char *set);
void state_machine_ua(int fd, char* ua);
void ua_function(char *ua);
void state_machine_set(int fd, char* ua);