all: alarm.c applicationlayer.c communication.c llclose.c llopen.c llread.c llwrite.c project.h
	gcc alarm.c applicationlayer.c communication.c llclose.c llopen.c llread.c llwrite.c -o proj -Wall
