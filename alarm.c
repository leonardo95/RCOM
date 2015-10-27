#include "project.h"

void signal_handler(int signal){
	if(signal == SIGALRM){
	  alarm_off=1;
	  alarm(3);
	}
		
}

void signal_set(){
	alarm_off = 0;
	struct sigaction sig_action;
	sig_action.sa_handler = signal_handler;
	sigemptyset(&sig_action.sa_mask);
	sig_action.sa_flags = 0;

	sigaction(SIGALRM, &sig_action, NULL);
	alarm(3);
	
}

void signal_stop(){
	struct sigaction sig_action;
	sig_action.sa_handler = NULL;
	sigemptyset(&sig_action.sa_mask);
	sig_action.sa_flags = 0;

	sigaction(SIGALRM, &sig_action, NULL);
	alarm(0);
}
