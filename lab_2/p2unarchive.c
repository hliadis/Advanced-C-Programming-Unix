/*P2UNARCHIVE:
 * Creates a directory using the data it receives
 * from its std input.
 */
#include "errors.h"
#include "pipes.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <utime.h>
#include <string.h>
#include <errno.h>

int main(int argc, char* argv[]){
	
	DIR * directory;
	int fd , numOfBlocks, numOfLeftBlocks, i;
	struct utimbuf time;
	time_t lastAccess, lastMod;
	mode_t accessRights;
	off_t fileLength;
	size_t nameLength;
	ssize_t read_return;
	char fileName[SIZE -1];
	char finalPath[SIZE-1];
	char name[] = "p2unarchive.c";
	void *block;
	
	if(argc != 2){
		error_Arg(name);
	}
	
	//creates directory
	directory = opendir(argv[1]);
	if(errno != ENOENT){
		error_ExistingDirectory(name, directory);
	}
	
	if (mkdir(argv[1],0777) == -1 ){
		error_Mkdir(name, errno);
	}
	
	
	do{
		//reading useful file info
		read_return = myread(STDIN_FILENO,&nameLength,sizeof(size_t));
		if (read_return == -1){
			closedir(directory);
			error_Read(name, errno);
		}
		
		if (myread(STDIN_FILENO,fileName,nameLength) == -1){
			closedir(directory);
			error_Read(name, errno);
		}
		
		fileName[nameLength] = '\0';
		
		if (myread(STDIN_FILENO,&lastAccess,sizeof(time_t)) == -1){
			closedir(directory);
			error_Read(name, errno);
		}
		
		if (myread(STDIN_FILENO,&lastMod,sizeof(time_t)) == -1){
			closedir(directory);
			error_Read(name, errno);
		}
		
		if (myread(STDIN_FILENO,&accessRights,sizeof(mode_t)) == -1){
			closedir(directory);
			error_Read(name, errno);
		}
		
		if (myread(STDIN_FILENO,&fileLength,sizeof(off_t)) == -1){
			closedir(directory);
			error_Read(name, errno);
		}
		
		strcpy(finalPath , argv[1]);
		strcat(finalPath , "/");
		strcat(finalPath, fileName);
		
		//Creating file in the directory
		fd = open(finalPath,O_CREAT|O_RDWR,0600);
		if (fd == -1){
			closedir(directory);
			error_Open(name, errno);
		}
		
		//Transfer data from the std input to the file
		numOfBlocks = fileLength / 512 ;
		
		numOfLeftBlocks = fileLength - (numOfBlocks * 512);
		
		block = malloc(512);
		if(block == NULL){
			closedir(directory);
			close(fd);
			error_Mem(name);
		}
		
		for(i = 0; i < numOfBlocks; i++){

			if (myread(STDIN_FILENO,block,512) == -1){
				free(block);
				closedir(directory);
				close(fd);
				error_Read(name, errno);
			}
			
			if (mywrite(fd,block,512)== -1){
				free(block);
				closedir(directory);
				close(fd);
				error_Write(name, errno);
			}
		}
		
		if (myread(STDIN_FILENO,block,numOfLeftBlocks) == -1){
			free(block);
			closedir(directory);
			close(fd);
			error_Read(name, errno);
		}
		
		if (mywrite(fd,block,numOfLeftBlocks) == -1){
			free(block);
			closedir(directory);
			close(fd);
			error_Write(name, errno);
		}
		
		free(block);
		
		//Changing file info
		if(chmod(finalPath,accessRights) == -1){
			closedir(directory);
			close(fd);
			error_Chmod(name, errno); 
		}
		
		time.actime = lastAccess;
		
		time.modtime = lastMod;
		
		if (utime(finalPath,&time) == -1){
			closedir(directory);
			close(fd);
			error_Utime(name, errno);
		}
		
		if (read_return == 0){
			closedir(directory);
			close(fd);
			break;
		}
	}while(1);
	
	return 0;
}