/*HW2:
 * Connects the execution of the programs with pipes
 * in order to make the final program work.
 */

#include "errors.h"
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

int main (int argc , char * argv []){
	
	pid_t pid1, pid2, pid3;
	char keyword[10];
	char name[] = "hw2.c";
	int fd1[2], fd2[2];
	int pipe1, pipe2;
	int fd;
	int i;
	ssize_t write_value , read_value;
	
	if (argc != 5){
		error_Arg(name);
	}
	
	if(strcmp(argv[1] , "-E") == 0){
		
		fd = open(argv[4],O_RDWR);
		if(fd > 0){
			error_ExistingFile(name, fd);
		}
		
		//Creating the file and writing the magic number
		fd = open(argv[4],O_RDWR|O_CREAT,0600);
		if (fd == -1){
			error_Open(name, errno); 
		}
		
		write_value = write(fd,"P2CRYPTAR",9);
		if(write_value == -1){
			close(fd);
			error_Write(name, errno);
		}
		
		//Creating pipes
		do{
			pipe1 = pipe(fd1);
			
		}while(pipe1 == -1);
		
		do{
			pipe2 = pipe(fd2);
			
		}while(pipe2 == -1);
		
		//Creating the first kid
		pid1 = fork();
		if(pid1 == -1){
			close(fd);
			error_Fork(name, errno);
		}
		
		if(pid1 == 0){
			//redirect pipes output with std output
			dup2(fd1[1],STDOUT_FILENO);
			
			//close the unnecessary fd s 
			close(fd1[0]);
			
			close(fd2[0]);
			
			close(fd1[1]);
			
			close(fd2[1]);
			
			//execute dirlist program
			if(execlp("./dirlist","dirlist",argv[2],NULL) == -1){
				close(fd);
				error_Exec("dirlist", errno, name);
			}
			
			close(fd);
			return 0;
		}
		else{
			//Creating 2nd kid
			pid2 = fork();
			if (pid2 == -1){
				close(fd);
				error_Fork(name, errno);
			}
			if(pid2 == 0){
				//redirect pipes accordingly
				dup2(fd1[0],STDIN_FILENO);
				
				dup2(fd2[1],STDOUT_FILENO);
				//close unnecessary fd s
				close(fd1[0]);
				
				close(fd2[0]);
				
				close(fd1[1]);
				
				close(fd2[1]);
				//execute p2archive
				if(execlp("./p2archive","p2archive",NULL) == -1){
					close(fd);
					error_Exec("p2archive", errno, name);
				}
				
				close(fd);
				return 0;
			}
			else{
				//Creating 3rd kid
				pid3 = fork();
				if(pid3 == -1){
					close(fd);
					error_Fork(name, errno);
				}
				if (pid3 == 0){
					//redirect pipe fd s accordingly
					dup2(fd2[0],STDIN_FILENO);
					
					dup2(fd,STDOUT_FILENO);
					//close unnecessary fd s
					close(fd1[0]);
					
					close(fd2[0]);
					
					close(fd1[1]);
					
					close(fd2[1]);
					//execute p2crypt
					if(execlp("./p2crypt","p2crypt",argv[3],NULL) == -1){
						close(fd);
						error_Exec("p2crypt", errno, name);
					}
					
					close(fd);
					
					return 0;
				}
				else{
					//parent
					//close unnecessary fd s
					close(fd1[0]);
					
					close(fd2[0]);
					
					close(fd1[1]);
					
					close(fd2[1]);
					
					// waiting the successful termination of the kids
					for(i = 0; i < 3 ; i++){
						
						if(waitpid(-1 ,NULL , 0) == -1){
							close(fd);
							error_Waitpid(name, errno);
						}
						
					}
					
					close(fd);
					
					return 0;
				}
			}
		}	
	}
	else{ 
		if(strcmp(argv[1], "-D") == 0){
		
			fd = open(argv[4], O_RDWR);
			if(fd == -1){
				error_Open(name, errno);
			}
			
			read_value = read(fd,keyword,9);
			if (read_value == -1){
				error_Read(name, errno);
			}
			
			keyword[9] = '\0';
			
			
			if(strcmp(keyword , "P2CRYPTAR") != 0){
				fprintf(stderr, "ERROR: This is not an encrypted file.\n");
				close(fd);
				exit(-1);
			}
			
			do{
				pipe1 = pipe(fd1);
				
			}while(pipe1 == -1);
			
			pid1 = fork();
			if(pid1 == -1){
				close(fd);
				error_Fork(name, errno);
			}
			
			if(pid1 == 0){
				
				dup2(fd1[1],STDOUT_FILENO);
				
				dup2(fd,STDIN_FILENO);
				
				close(fd1[0]);
				
				close(fd1[1]);
				
				if(execlp("./p2crypt","p2crypt",argv[3],NULL) == -1){
					close(fd);
					error_Exec("p2crypt", errno, name);
				}
				
				close(fd);
				
				return 0;
			}
			else{
				pid2 = fork();
				if (pid2 == -1){
					close(fd);
					error_Fork(name, errno);
				}
				if(pid2 == 0){
					
					dup2(fd1[0],STDIN_FILENO);
					
					close(fd1[0]);
					
					close(fd1[1]);
					
					if(execlp("./p2unarchive","p2unarchive",argv[2],NULL) == -1){
						close(fd);
						error_Exec("p2unarchive", errno, name);
					}
					
					close(fd);
					
					return 0;
				}
				else{
					
					close(fd1[0]);
					
					close(fd1[1]);
					
					for(i = 0; i < 2 ; i++){
						if(waitpid(-1 ,NULL , 0) == -1){
							close(fd);
							error_Waitpid(name, errno);
						}
					}
					
					close(fd);
					
					return 0;
				}
			}
		}
		else{
			fprintf(stderr,"ERROR: Wrong second argument(hw2.c).\n");
			fprintf(stderr,"Exiting program ....\n");
			exit(-1);
		}
	}
}