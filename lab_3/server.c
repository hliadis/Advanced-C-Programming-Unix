#include "errors.h"
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/un.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/sem.h>

#define SERVER "Server"

struct data{
    char company_code[4];
    char airport_dep_code[5];
    char airport_arr_code[5];
    int stops;
    int seats;
    struct data *nxt;
};

struct clients{
	int fd;
	pid_t ID;
	int seats;
};

//Function that counts the lines from the database.
//Return value: The number of the lines.
int NumberOfLines(int database_fd){
    
    int counter = 0;
    char line[25];
    FILE *database_fp;
    
    
    database_fp = fdopen(database_fd,"r+");
    if(database_fp == NULL){
		fclose(database_fp);
        error_Fdopen(SERVER,errno);
    }
    
    while(fgets(line,24,database_fp) != NULL){
        counter++ ;
    }
    
    return(counter);
}

int main (int argc, char *argv[]) {
    
    int database_fd, oldin, oldout;
	int max_fd = 0, max_peers, curr_fd, curr_peers = 0, i, j;
	char curr[5];
    key_t key;
    int shmid, semid;
    long *p;
    struct data *n, *n2 ; 
	int lines;
    struct sockaddr_un sa;
	struct clients  *peer_array;
	fd_set set, read_set;
	ssize_t read_value;
	int error_value  = -1;
	char exiting_char;
	int curr_seats;
	
    if(argc != 4){
        error_Arg(SERVER);
    }
    
    database_fd = open(argv[2], O_RDWR);
    if(database_fd == -1){
        error_Open(SERVER,errno);
    }
	
	//creating sheared memory
	key = ftok(argv[2],11);
    if(key == -1){
		close(database_fd);
        error_Ftok(SERVER,errno);
    }
    
    lines = NumberOfLines(database_fd);
	
	//redirection of the std input.
	oldin = dup(STDIN_FILENO);
	dup2(database_fd, STDIN_FILENO);
	
	//setting the offset to the beginning of the file.
	if(lseek(database_fd,SEEK_SET,0) == -1){
		close(oldin);
		close(database_fd);
		error_Lseek(SERVER,errno);
	}
	
    shmid = shmget(key, lines * sizeof(struct data) + sizeof(long), IPC_CREAT|IPC_EXCL|S_IRWXU);
    if(shmid == -1){
		close(oldin);
		close(database_fd);
        error_Shmget(SERVER,errno);
    }
    
    p = (long*)shmat(shmid, NULL, 0);
	if( p == (void *)-1){
		close(oldin);
		close(database_fd);
		error_Shmat(SERVER,errno);
	}
    
    *p = (long)p; 
    
    n = (struct data *)(p + 1);
    
    for(n2 = n; n2 < n + lines; n2++){
		
		scanf(" %4s",n2->company_code);
		scanf(" %5s",n2->airport_dep_code);
		scanf(" %5s",n2->airport_arr_code);
		scanf(" %5s",curr);
		n2->stops = atoi(curr);
		scanf(" %5s",curr);
		n2->seats = atoi(curr);
		n2->nxt = n2 + 1;
		
	}
	
	(n + lines - 1) ->nxt = NULL;
	
	dup2(oldin, STDIN_FILENO);
	
	close(oldin);
	
	//Creat semaphotres
	semid = semget(123456, 1, IPC_CREAT|IPC_EXCL|S_IRWXU);
	if (semid == -1){
		close(database_fd);
		error_Semget(SERVER, errno);
	}
	
	if(semctl(semid, 0, SETVAL, 1) == -1){
		close(database_fd);
		error_Semctl(SERVER, errno);
	}
	
	//Creat multiple connections and handle the file 
	//descriptors with connect function.
	max_peers = atoi(argv[1]);
	
	peer_array = (struct clients *)malloc((max_peers + 2) * sizeof(struct clients));
	
	sa.sun_family = AF_UNIX;
	strcpy(sa.sun_path, argv[3]);
	
	peer_array[0].fd = socket(AF_UNIX, SOCK_STREAM,0);
	if (peer_array[0].fd == -1){
		free(peer_array);
		close(database_fd);
		error_Socket(SERVER, errno);
	}
	
	if (bind(peer_array[0].fd,(struct sockaddr *)&sa, sizeof(sa)) == -1){
		close(peer_array[0].fd);
		free(peer_array);
		close(database_fd);
		error_Bind(SERVER, errno);
	}
	
	if(listen(peer_array[0].fd,SOMAXCONN) == -1){
		close(peer_array[0].fd);
		free(peer_array);
		close(database_fd);
		error_Listen(SERVER, errno);
	}
	
	if (peer_array[0].fd > max_fd){
		max_fd = peer_array[0].fd;
	}
	
	FD_ZERO(&set);
	
	FD_SET(0, &set);
	FD_SET(peer_array[0].fd, &set);
	
	
	do{
		read_set = set;
		
		if (select(max_fd + 1, &read_set, NULL, NULL, NULL) == -1){
			close(peer_array[0].fd);
			free(peer_array);
			close(database_fd);
			error_Select(SERVER,errno);
		}
		
		for(curr_fd = 0; curr_fd <= max_fd; curr_fd++){
			
			if(FD_ISSET(curr_fd, &read_set)){
				
				if (curr_fd == 0){
					exiting_char = getchar();
					if(exiting_char == 'Q'){
						for(i = 0 ; i <= curr_peers ; i++){
							if(i > 0){
								printf("ID: %d reserved in total %d tickets \n",peer_array[i].ID, peer_array[i].seats);
							}
							FD_CLR(peer_array[i].fd, &set);
							close(peer_array[i].fd);
						}
						FD_CLR(0 , &set);
						free(peer_array);
						unlink(argv[3]);
					}
					break;
				}
				else{
					if((curr_fd == peer_array[0].fd )&& (curr_peers < max_peers)){
						
						curr_peers++;
						
						peer_array[curr_peers].fd = accept(peer_array[0].fd,NULL,0);
						if (peer_array[curr_peers].fd == -1){
							for(i = 0 ; i < curr_peers ; i++){
								close(peer_array[i].fd);
							}
							free(peer_array);
							close(database_fd);
							error_Accept(SERVER,errno);
						}
						
						peer_array[curr_peers].seats = 0; 
						
						if (write(peer_array[curr_peers].fd, &shmid, sizeof(int)) == -1){
							for(i = 0 ; i <= curr_peers ; i++){
								close(peer_array[i].fd);
							}
							free(peer_array);
							close(database_fd);
							error_Write(SERVER,errno);
						}
						
						if(read(peer_array[curr_peers].fd, &peer_array[curr_peers].ID, sizeof(pid_t)) == -1){
							for(i = 0 ; i <= curr_peers ; i++){
								close(peer_array[i].fd);
							}
							free(peer_array);
							close(database_fd);
							error_Read(SERVER,errno);
						}
						
						printf("Agent with ID:%d connected succesfully...\n", peer_array[curr_peers].ID);
						
						FD_SET(peer_array[curr_peers].fd, &set);
						
						if(peer_array[curr_peers].fd > max_fd){
							max_fd = peer_array[curr_peers].fd;
						}
					}
					else{
						if((curr_fd == peer_array[0].fd )&& (curr_peers == max_peers)){
							
							peer_array[curr_peers+1].fd = accept(peer_array[0].fd,NULL,0);
						
							if (peer_array[curr_peers+1].fd == -1){
								
								for(i = 0 ; i < curr_peers + 1  ; i++){
									close(peer_array[i].fd);
								}
								
								free(peer_array);
								close(database_fd);
								error_Accept(SERVER,errno);
							}
							if (write(peer_array[curr_peers+1].fd, &error_value, sizeof(int)) == -1){
								for(i = 0 ; i <= curr_peers ; i++){
									close(peer_array[i].fd);
								}
								free(peer_array);
								close(database_fd);
								error_Write(SERVER,errno);
							}
							
							close(peer_array[curr_peers + 1].fd);
						}
						else{
							
							for(i = 1; 1; i++){
								if(peer_array[i].fd ==curr_fd){
									break;
								}
							}
							read_value = read(peer_array[i].fd ,&curr_seats, sizeof(int));
							if (read_value == -1){
								for(j = 0 ; j <= curr_peers ; j++){
									close(peer_array[j].fd);
								}
								free(peer_array);
								close(database_fd);
								error_Read(SERVER,errno);
							}
							peer_array[i].seats = peer_array[i].seats + curr_seats;
							if(read_value == 0){
								FD_CLR(peer_array[i].fd, &set);
								if(peer_array[i].fd == max_fd){
									max_fd--;
								}
								close(peer_array[i].fd);
								curr_peers--;
							}
							else{
								printf("ID: %d\n",peer_array[i].ID);
								printf("Reserved %d seats.\n",curr_seats);
							}
						}
					}
				}
			}
		}
		
		if(exiting_char == 'Q'){
			break;
		}
		
	}while(1);

	//Recreate the database with new data.
	if (ftruncate(database_fd,0) == -1){
		close(database_fd);
		error_Ftruncate(SERVER,errno);
	}
	
	if(lseek(database_fd,SEEK_SET,0) == -1){
		close(database_fd);
		error_Lseek(SERVER,errno);
	}
	
	oldout = dup(STDOUT_FILENO);
	
	dup2(database_fd,STDOUT_FILENO);
	
	for(n2 = n; n2 < n + lines; n2++){
		printf("%s", n2->company_code);
		printf(" %s", n2->airport_dep_code);
		printf(" %s", n2->airport_arr_code);
		printf(" %d", n2->stops);
		printf(" %d\n", n2->seats);
		fflush(stdout);
	}
	
	dup2(oldout, STDOUT_FILENO);
	
	close(oldout);
	
	close(database_fd);
	
	shmdt(p);
	
	//Setting remove flags to the semaphores and to the shm memory.
	semctl(semid, 0, IPC_RMID);
	
	shmctl(shmid, IPC_RMID, 0);
	
    return(0);
}
