/*Test program for hw4 . It prints all the numbers from 
 0 to max with a sleeping mode of 5 seconds. It also handles
 the SIGUSR1 signal.*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include "errors.h"

#define NAME "step1"

volatile sig_atomic_t i = 0;

static void handler(int sig){
	i = 0;
}

int main(int argc , char * argv[]){
	
	int maxnum, action, j;
	sigset_t set;
	struct sigaction act = {{0}};
	
	if(argc != 5){
		error_Arg(NAME);
	}
	
	if(strcmp(argv[1],"-m") != 0){
		fprintf(stderr,"Wrong 1st argument\n");
		exit(EXIT_FAILURE);
	}
	
	if(strcmp(argv[3],"-b") != 0){
		fprintf(stderr,"Wrong 3rd argument\n");
		exit(EXIT_FAILURE);
	}
	
	maxnum = atoi(argv[2]);
	action = atoi(argv[4]);
	if ((action == 0)||(action == 1)){
		act.sa_handler = handler;
			
			act.sa_flags = SA_RESTART;
			
			if(sigaction(SIGUSR1, &act, NULL) == -1){
				error_Sigaction(NAME, errno);
			}
		if(action == 1){
			//Block SIGUSR1.If action is 1.
			if(sigemptyset(&set) == -1){
				error_Sigemptyset(NAME, errno);
			}
			
			if(sigaddset(&set, SIGUSR1) == -1){
				error_Sigaddset(NAME, errno);
			}
			
			if(sigprocmask(SIG_BLOCK, &set, NULL) == -1){
				error_Sigprocmask(NAME, errno);
			}
		}
		
		for(j = 0; i <= maxnum ; i++){
			
			sleep(5);
			
			printf("ID: %d. Counter is %d .The max number is %d\n", getpid(), i, maxnum);
			//Unblock sigusr1 when action = 1 and keep it unblocked.
			if((i == maxnum / 2) && (action == 1)&&(j == 0)){
				j++;
				if(sigprocmask(SIG_UNBLOCK, &set, NULL) == -1){
					error_Sigprocmask(NAME, errno);
				}
			}
			
		}
		return 0;
	}
	else{
		fprintf(stderr,"Wrong 4th argument\n");
		exit(EXIT_FAILURE);
	}
}