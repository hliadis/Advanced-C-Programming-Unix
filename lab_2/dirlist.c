/*DIRLIST:
 * This program opens a directory and prints in the std output it's
 * files including the relative or the absolute filepath.
 * Each file is printed in a different line.
 */
#include "errors.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

int main(int argc , char* argv[]){
	
	char filePath[SIZE], *filePath_2;
	char name[] = "dirlist.c";
	DIR * dirlist;
	struct dirent *nameOfFile;
	int fd;
	
	if (argc != 2){
		error_Arg(name);
	}
	
	strcpy(filePath,argv[1]);
	
	//opening directory.
	dirlist = opendir(filePath);
	if(dirlist == NULL){
		error_Opendir(name, errno);
	}
	
	do {
		errno = 0;
		//reading directory
		nameOfFile = readdir(dirlist);
		if ((nameOfFile == NULL) && (errno == 0)){
			break;
		}
		
		if(errno != 0){
			closedir(dirlist);
			error_Readdir(name, errno);
		}
		
		//creating the filepaths that are gonna be printed.
		filePath_2 =(char*)malloc(strlen(filePath)+strlen(nameOfFile->d_name)+3);
		if (filePath_2 == NULL){
			closedir(dirlist);
			error_Mem(name);
		}
		
		strcpy(filePath_2,filePath);
		
		filePath_2 = strcat(filePath_2,"/");
		
		filePath_2 = strcat(filePath_2,nameOfFile->d_name);
		
		//Checking if the names are files and not directories
		if ((fd = open(filePath_2,O_RDWR))  >= 0){
			printf("%s/%s\n",filePath,nameOfFile->d_name);
			close(fd);
		}
		
		free(filePath_2);
		
	}while(nameOfFile != NULL);
	
	closedir(dirlist);
	
	return 0;
}
