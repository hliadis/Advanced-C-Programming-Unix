/*A small shell program that executes other programs with different pids
 and also can send signals : SIGUSR1 , SIGTERM, SIGCONT, SIGSTOP
 finally it handles signals : SIGUSR1, SIGCHLD, SIGALRM*/

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include "errors.h"
#include <sys/time.h>

#define SIZEOFLINE 300
#define NUMOFARG 21
#define NAME "hw4"


struct data{
	
	short int work;
	pid_t pid;
	char *arg[NUMOFARG];
	struct data *nxt;

};

struct data *init;

/*Handler of the SIGALRM it handles SIGSTOP and SIGCONT between 
 the nodes of the list(pids)*/ 
static void alarmhandler(int sig){
	
	struct data *work_node;
	struct data *work_nxt;
    
	if (write(STDOUT_FILENO, "TING\n", 5*sizeof(char)) == -1){
		_exit(EXIT_FAILURE);
	}
	
    init->work = 1;
	
    for (work_node = init->nxt; work_node->work != 1; work_node = work_node->nxt);
	
	if (init -> nxt == init){
		return;
	}
	
	work_node -> work = 0;
	if (kill(work_node -> pid, SIGSTOP) == -1){
		_exit(EXIT_FAILURE);
	}
	
	if (work_node -> nxt == init){
		work_nxt = init -> nxt;
	}
	else{
		work_nxt = work_node -> nxt;
	}
	
	work_nxt -> work = 1;
	if (kill(work_nxt -> pid, SIGCONT) == -1){
		_exit(EXIT_FAILURE);
	}
}

/*Handler of SIGUSR1 it sends SIGUSR1 to all the pids of the list*/
static void usr1handler(int sig){
	struct data * curr;
	sigset_t set;
	
	//Block SIGALRM and SIGCHLD
	if (sigemptyset(&set) == -1){
		_exit(EXIT_FAILURE);
	}
	if (sigaddset(&set, SIGALRM) == -1){
		_exit(EXIT_FAILURE);
	}
	if (sigaddset(&set, SIGCHLD) == -1){
		_exit(EXIT_FAILURE);
	}
	if(sigprocmask(SIG_BLOCK, &set, NULL) == -1){
		_exit(EXIT_FAILURE);
	}
	
	for(curr = init -> nxt ; curr != init; curr = curr -> nxt){
		
		if(curr -> work == 0){
			if(kill(curr -> pid, SIGCONT) == -1){
				_exit(EXIT_FAILURE);
			}
		}
		
		if (kill(curr->pid, SIGUSR1) == -1){
			_exit(EXIT_FAILURE);
		}
		
		if(curr -> work == 0){
			if(kill(curr -> pid, SIGSTOP) == -1){
				_exit(EXIT_FAILURE);
			}
		}
	}
	
	if(sigprocmask(SIG_UNBLOCK, &set, NULL) == -1){
		_exit(EXIT_FAILURE);
	}
}

/*Handler of SIGCHLD: it waits children that are
  terminated normally / terminated by signal/ received SIGSTOP/ 
  received SIGCONT.*/
static void chldhandler(int sig){
	
	int status;
	struct data *current, *prev;
	pid_t pid;
	
	pid = waitpid(-1, &status, WUNTRACED| WCONTINUED);
		
	if(pid == -1){
		_exit(EXIT_FAILURE);
	}
	if((WIFEXITED(status)) || (WIFSIGNALED(status))){
		
		init->pid = pid;
	
		for (prev = init, current = init->nxt; current->pid != pid; prev = current, current = current->nxt);
	
		prev->nxt = current->nxt;
	
		free(current);
		
		if (kill(getpid(), SIGALRM) == -1 ){
			_exit(EXIT_FAILURE);
		}
	}
}

/*Inserts new node to the list.*/
struct data *insertnode(){
	
	struct data *last, *node;
	int i;
	
	for(last = init; last->nxt != init; last = last->nxt);
	
	node = (struct data*)malloc(sizeof(struct data));
	
	if(node == NULL){
		error_Mem(NAME);
	}
	
	for(i = 0 ; i <= NUMOFARG; i++ ){
		node->arg[i] = NULL;
	}
	
	last->nxt = node;
	
	node -> nxt = init;

	return(node);
	
}

/*Prints the info of the nodes of the list.*/
void list(){
	
	struct data *curr;
	
	int i;
	
	for(curr = init -> nxt; curr != init; curr = curr -> nxt){
		
		printf("pid: %d", curr->pid);
		
		printf(", name: (%s ", curr->arg[0]);
		
		for(i = 1; curr -> arg[i] != NULL ; i++){
			printf(", %s", curr->arg[i]);
		}
		printf(")");
		
		if(curr->work == 1){
			printf("(R)\n");
		}
		else{
			printf("\n");
		}
	}
	
	return;
}

/*Removes node from the list and free the dynamic 
 *allocated memmory*/
void remove_node(pid_t search_pid){
	
    struct data *current, *prev;
    
	init->pid = search_pid;
	
    for (prev = init, current = init->nxt; current->pid != search_pid; prev = current, current = current->nxt);
	
	prev->nxt = current->nxt;
	
	free(current);
}

/*Finds the pointer to a node based on a given pid.*/
struct data *find_node(pid_t search_pid){
    
	struct data *current;
    
    init->pid = search_pid;
	
    for (current = init->nxt; current->pid != search_pid; current = current->nxt);
	
    return(current);
}

/*Waits chlidren*/
void waitchld(){
	
	int status = 0;
	pid_t pid;
	
		pid = waitpid(-1, &status, WUNTRACED| WCONTINUED);
		
		if(pid == -1){
			error_Waitpid(NAME, errno);
		}
		if((WIFEXITED(status)) || (WIFSIGNALED(status))){
			remove_node(pid);
		}
		
	return ;
	
}
int main(int argc , char* argv[]){
	
	char action[5], *curr, usr1pid[8], termpid[8];
	char *line;
	struct data *makesentinel, *node, *current_p;
	struct sigaction usr1act = {{0}};
	struct sigaction chldact = {{0}};
	struct sigaction alarmact = {{0}};
	struct itimerval time;
	int i;
	pid_t pid;
	sigset_t set;
	
	if (sigemptyset(&set) == -1){
		error_Sigemptyset(NAME , errno);
	}
	if (sigaddset(&set, SIGUSR1) == -1){
		error_Sigaddset(NAME , errno);
	}
	if (sigaddset(&set, SIGALRM) == -1){
		error_Sigaddset(NAME , errno);
	}
	if (sigaddset(&set, SIGCHLD) == -1){
		error_Sigaddset(NAME , errno);
	}
	
	usr1act.sa_handler = usr1handler;
	usr1act.sa_flags = SA_RESTART;
	if(sigaction(SIGUSR1, &usr1act, NULL) == -1){
		error_Sigaction(NAME, errno);
	}
	
	
	chldact.sa_handler = chldhandler;
	chldact.sa_flags = SA_RESTART;
	if(sigaction(SIGCHLD, &chldact, NULL ) == -1){
		error_Sigaction(NAME, errno);
	}
	
	alarmact.sa_handler = alarmhandler;
	alarmact.sa_flags = SA_RESTART;
	if(sigaction(SIGALRM, &alarmact, NULL ) == -1){
		error_Sigaction(NAME, errno);
	}
	
	time.it_interval.tv_sec = 20;
	time.it_interval.tv_usec = 0;
	time.it_value.tv_sec = 20;
	time.it_value.tv_usec = 0;
	
	if (setitimer(ITIMER_REAL, &time, NULL) == -1){
		error_Setitimer(NAME, errno);
	}
	
	//Create the sentinel node of the list.
	makesentinel = (struct data *)malloc(sizeof(struct data));
	
	if(makesentinel == NULL){
		error_Mem(NAME);
	}
	
	init = makesentinel;
	init -> nxt = init;
	
	
	do{
		scanf(" %4s", action);
		
		if(strcmp(action, "exec") == 0){
			
			//Create the arguments table
			line = (char*)malloc(500);
			if(line == NULL){
				error_Mem(NAME);
			}
			
			getchar();
			
			fgets(line, SIZEOFLINE, stdin);
			
			line[strlen(line)-1]= '\0';
			
			line = (char*)realloc(line, (strlen(line) + 1));
			if(line == NULL){
				error_Mem(NAME);
			}
			
			node = insertnode();
			
			i = 0;
			
			//add info to the node
			do{
				
				curr = strchr(line, ' ');
				
				if(curr == NULL){
					node->arg[i] = line;
					break;
				}
				
				*curr = '\0';
				
				node -> arg[i] = line;
				
				curr = curr + 1;
				
				line = curr;
				
				i++;
				
			}while(1);
			
			pid = fork();
			if (pid == 0){
				
				if (execvp(node -> arg[0], node -> arg) == -1){
					error_Exec(NAME, errno, node->arg[0]);
				}
				
			}
			
			if(pid == -1){
				error_Fork(NAME, errno);
			}
			
			node -> pid = pid;
			
			//Turn the working switch on.
			if(init -> nxt -> nxt == init){
				node -> work = 1;
			}
			else{
				node -> work = 0;
				if(kill(pid, SIGSTOP) == -1){
					error_Kill(NAME , errno);
				}
			}
		}
		else{
			
			//Send SIGTERM to a given pid. 
			if(strcmp(action, "term") == 0){
				
				scanf(" %7s", termpid);
				
				current_p = find_node(atoi(termpid));
				
				if(current_p -> work == 0){

					if(kill((pid_t)atoi(termpid), SIGCONT) == -1){
						if(errno == ESRCH){
							printf("WARNING! wrong pid. Try again...\n");
						}
						else{
							error_Kill(NAME, errno);
						}
					}
				}
				
				if(kill((pid_t)atoi(termpid), SIGTERM) == -1){
					if(errno == ESRCH){
						printf("WARNING! wrong pid. Try again...\n");
					}
					else{
						error_Kill(NAME, errno);
					}
				}
			}
			else{
				
				//Send SIGUSR1 to a given pid.
				if(strcmp(action, "sig") == 0){
					
					scanf(" %7s", usr1pid);
					
					if (sigdelset(&set, SIGUSR1) == -1){
						error_Sigdelset(NAME , errno);
					}
					
					if(sigprocmask(SIG_BLOCK, &set, NULL) == -1){
						error_Sigprocmask(NAME, errno);
					}
					
					current_p = find_node(atoi(termpid));
					
					if(current_p -> work == 0){
						if(kill((pid_t)atoi(termpid), SIGCONT) == -1){
							if(errno == ESRCH){
								printf("WARNING! wrong pid. Try again...\n");
							}
							else{
								error_Kill(NAME, errno);
							}
						}
					}
					if(kill((pid_t)atoi(usr1pid), SIGUSR1) == -1){
						if(errno == ESRCH){
							printf("WARNING! wrong pid. Try again...\n");
						}
						else{
							error_Kill(NAME, errno);
						}
					}
					if(current_p -> work == 0){
						if(kill((pid_t)atoi(termpid), SIGSTOP) == -1){
							if(errno == ESRCH){
								printf("WARNING! wrong pid. Try again...\n");
							}
							else{
								error_Kill(NAME, errno);
							}
						}
					}
					if(sigprocmask(SIG_UNBLOCK, &set, NULL) == -1){
						error_Sigprocmask(NAME, errno);
					}
					if (sigaddset(&set, SIGUSR1) == -1){
						error_Sigaddset(NAME , errno);
					}
				}
				else{
					
					//Prints to STDOUT the list showing the node that is working.
					if(strcmp(action, "list") == 0){
						list();
					}
					else{
						
						//Term all the children wait them and free all the allocated memory.
						//And exit program.
						if(strcmp(action, "quit") == 0){
							
							if(sigprocmask(SIG_BLOCK, &set, NULL) == -1){
								error_Sigprocmask(NAME, errno);
							}
							
							for(current_p = init -> nxt ; current_p != init; current_p = init -> nxt){
								if(current_p -> work == 0){
									if(kill(current_p -> pid, SIGCONT) == -1){
										error_Kill(NAME, errno);
									}
								}
								if(kill(current_p -> pid, SIGTERM) == -1){
									error_Kill(NAME, errno);
								}
								
								waitchld();
							}
							
							free(init);
							break;
						}
					}
				}
			}
		}
	}while(1);
	
	return 0;
}