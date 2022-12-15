/*The AGENT access the shared memory if the server hasn't reached
 the maximum AGENTs. The AGENT can FIND a flight or RESERVE a flight 
 or QUIT from the program */

#include "errors.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/un.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>

#define AGENT "Agent"

void print_menu(void);

struct data{
    char company_code[4];
    char airport_dep_code[5];
    char airport_arr_code[5];
    int stops;
    int seats;
    struct data *nxt;
};

int main(int argc, char *argv[]) {
    
    struct sockaddr_un addr;
    int socket_fd;
    pid_t processID;
	char action[8], supp[5];
    char airport_dep_code[5];
    char airport_arr_code[5];
	char company_code[4];
    int shmid,seats, semid, i;
	long *p ,off;
	struct data *n, *k;
	struct sembuf op;
	
	if(argc != 2){
		error_Arg(AGENT);
	}
	
	//Connecting to the socket
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path,argv[1]);
    
    socket_fd = socket(AF_UNIX, SOCK_STREAM ,0);
	if(socket_fd == -1){
		error_Socket(AGENT, errno);
	}
	
	while(connect(socket_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1){
		if(errno == ENOENT){
			sleep(1);
			continue;
		}
		
		close(socket_fd);
		error_Connect(AGENT,errno);
	}
	
	//Reading shmid from the AGENT.
	if(read(socket_fd, &shmid, sizeof(int)) == -1){
		close(socket_fd);
		error_Read(AGENT,errno);
	}
	
	if(shmid == -1){
		fprintf(stderr, "Server reached maximum AGENTs. Try again later...\n");
		fprintf(stderr, "Exiting program ....\n");
		close(socket_fd);
		exit(EXIT_FAILURE);
	}
	
	
	printf("Connected successfully with server...\n");
	
	//Sending the ID proccess to the AGENT.
	processID = getpid();
	
	if(write(socket_fd, &processID, sizeof(pid_t)) == -1){
		close(socket_fd);
		error_Write(AGENT,errno);
	}
	
	//access shared memory
	p = (long *)shmat(shmid, NULL, 0);
	
	off = (long)p - *p;
	
	k = (struct data *)(p + 1);
	
	//access semaphores
	semid = semget(123456, 1, 0);
	if(semid == -1){
		close(socket_fd);
		error_Semget(AGENT, errno);
	}
	
	op.sem_num = 0;
	op.sem_flg = 0;
	
	do{
		
		n = k;
		
		print_menu();
		
		scanf(" %7s" ,action);
		
		op.sem_op = -1;
		
		if(semop(semid, &op, 1) == -1){
			
			if (errno != EINVAL){
				close(socket_fd);
				error_Semop(AGENT, errno);
			}
			else{
				printf("WARNING! SERVER HAS BEEN CLOSED.\n");
				printf("Exiting program...\n");
				break;
			}
		}
		
		if((strcmp(action, "FIND") != 0) && (strcmp(action, "RESERVE") != 0) && (strcmp(action, "QUIT") != 0)){
			
			printf("Wrong action. Try again\n");
			continue;
			
		}
		
		i = 0;
		
		if(strcmp(action, "FIND") == 0){
			
			scanf(" %4s" , airport_dep_code);
			scanf(" %4s" , airport_arr_code);
			scanf(" %4s" , supp);
			seats = atoi(supp);
			
			printf("Available flights based on your search:\n");
			printf("*****************\n");
			
			while(n != NULL){
				
				if ((strcmp(n->airport_dep_code, airport_dep_code) != 0) || (strcmp(n->airport_arr_code, airport_arr_code) != 0) || (seats > n->seats)){
					if(n->nxt == NULL){
					n = NULL;
					}
					else{
						n = (struct data *)(off + (long)(n->nxt));
					}
					continue;
				}
				
				i++;
				printf("%s", n->company_code);
				printf(" %s", n->airport_dep_code);
				printf(" %s", n->airport_arr_code);
				printf(" %d", n->stops);
				printf(" %d\n", n->seats);
				if(n->nxt == NULL){
					n = NULL;
				}
				else{
					n = (struct data *)(off + (long)(n->nxt));
				}
			}
			if(i == 0){
				printf("Couldn't find available flights based on your search.\n");
			}
		}
		
		if(strcmp(action, "RESERVE") == 0){
			
			scanf(" %4s" , airport_dep_code);
			scanf(" %4s" , airport_arr_code);
			scanf(" %3s", company_code);
			scanf(" %4s", supp);
			seats = atoi(supp);
			while(n != NULL){
				if ((strcmp(n->airport_dep_code, airport_dep_code) == 0) && (strcmp(n->airport_arr_code, airport_arr_code) == 0) && (seats <= n->seats) && (strcmp(n->company_code,company_code) == 0)){
					
					printf("Flight found and tickets have been reserved\n");
					n->seats = n->seats - seats;
					
					if(write(socket_fd, &seats, sizeof(int)) == -1){
						close(socket_fd);
						error_Write(AGENT,errno);
					}
					
					break;
				}
				else{
					if(n->nxt == NULL){
						n = NULL;
						printf("ERROR: Flight not found.\n");
					}
					else{
						n = (struct data *)(off + (long)(n->nxt));
					}
					continue;		
				}
			}
		}
		
		op.sem_op = 1;
		
		if(semop(semid, &op, 1) == -1){
			if (errno != EINVAL){
				close(socket_fd);
				error_Semop(AGENT, errno);
			}
			else{
				printf("SERVER HAS BEEN CLOSED.\n");
				printf("Exiting program...\n");
				break;
			}
		}
		
		if(strcmp(action, "QUIT") == 0){
			break;
		}
		
		
	}while(1);
	
	close(socket_fd);
	
	shmdt(p);
	
	return 0;
}


void print_menu(void){
	printf("*****************\n");
	printf("ACTION MENU:\n");
	printf("FIND <SRC> <DEST> <AVAILABLE TICKETS>\n");
	printf("RESERVE <SRC> <DEST> <AIRPORT CODE> <TICKETS YOU WANT TO RESERVE>\n");
	printf("QUIT\n");
	printf("CHOOSE ACTION: ");
	fflush(stdout);
	return;
}