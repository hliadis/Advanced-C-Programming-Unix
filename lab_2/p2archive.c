/*P2ARCHIVE:
 * Printing data in the std output using the filepaths
 * given by the program s std input.
 */
#include "errors.h"
#include "pipes.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

int main (int argc, char* argv[]){
	
	int fd_file;
	size_t nameLength;
	char *fileName; 
	char name[] = "p2archive.c";
	char filePath[SIZE], format[20];
	struct stat fileInfo;
	time_t lastAccess, lastMod;
	mode_t accessRights;
	off_t fileLength;
	ssize_t read_value_temp;
	void *buf, *temp, *blocks;
	long long int bufLength; 
	
	sprintf(format,"%%%ds",SIZE - 1);
	while(scanf(format, filePath) != EOF){
		
		//opening file given by std input
		fd_file = open(filePath,O_RDWR);
		if(fd_file == -1){
			error_Open(name, errno);
		}
		//finding info about the file
		if (fstat(fd_file ,&fileInfo) == -1){
			close(fd_file);
			error_Stat(name, errno);
		}
		
		//Creating file ID	
		fileName = strrchr(filePath,'/');
		if(fileName == NULL){
			fileName =  filePath;
		}
		else{
			fileName = fileName + 1;
		}
		
		nameLength = strlen(fileName);
		
		lastAccess = fileInfo.st_atim.tv_sec;
		
		lastMod = fileInfo.st_mtim.tv_sec;
		
		accessRights = fileInfo.st_mode;
		
		fileLength = fileInfo.st_size;
		
		buf = malloc(sizeof(size_t) + strlen(fileName) + 2*sizeof(time_t) + sizeof(mode_t) + sizeof(off_t));
		if(buf == NULL){
			close(fd_file);
			error_Mem(name);
		}
		
		temp = buf;
		
		memcpy(buf , &nameLength , sizeof(size_t));
		
		temp = temp + sizeof(size_t);
		
		memcpy(temp, fileName, strlen(fileName));
		
		temp = temp + strlen(fileName);
		
		memcpy(temp , &lastAccess , sizeof(time_t));
		
		temp = temp + sizeof(time_t);
		
		memcpy(temp , &lastMod , sizeof(time_t));
		
		temp = temp + sizeof(time_t);
		
		memcpy(temp , &accessRights , sizeof(mode_t));
		
		temp = temp + sizeof(mode_t);
		
		memcpy(temp , &fileLength , sizeof(off_t));
		
		bufLength = sizeof(size_t) + strlen(fileName) + 2*sizeof(time_t) + sizeof(mode_t) + sizeof(off_t);
		
		if(mywrite(1,buf,bufLength) == -1){
			close(fd_file);
			free(buf);
			error_Write(name, errno);
		}
		
		free(buf);
		
		//Transfer data with blocks of 512 bytes.
		blocks = malloc(512);
		if(blocks == NULL){
			close(fd_file);
			error_Mem(name);
		}
		do {
			read_value_temp = myread(fd_file,blocks,512);
			if(read_value_temp == -1){
				close(fd_file);
				free(blocks);
				error_Read(name, errno);
			}
			
			if (mywrite(1,blocks,read_value_temp) == -1){
				close(fd_file);
				free(blocks);
				error_Write(name, errno);
			}
			
			
			if(read_value_temp < 512){
				free(blocks);
				close(fd_file);
				break;
			}
			
		}while(1);
	}
	return 0;
}