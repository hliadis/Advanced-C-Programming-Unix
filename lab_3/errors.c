#include "errors.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>

//1
void error_Write(char *nameOfFunction, int error){
	fprintf(stderr, "ERROR: Unable to write(%s): %s.\n", nameOfFunction, strerror(error));
	fprintf(stderr, "Exiting program ....\n");
	exit(EXIT_FAILURE);
}

//2
void error_Read(char * nameOfFunction, int error){
	fprintf(stderr, "ERROR: Unable to read(%s): %s.\n", nameOfFunction, strerror(error));
	fprintf(stderr, "Exiting program ....\n");
	exit(EXIT_FAILURE);
}

//3
void error_Open(char * nameOfFunction, int error){
	fprintf(stderr, "ERROR: Unable to open(%s): %s.\n", nameOfFunction, strerror(error));
	fprintf(stderr, "Exiting program ....\n");
	exit(EXIT_FAILURE);
}

//4
void error_ExistingFile(char * nameOfFunction ,int fd){
	fprintf(stderr, "ERROR: File already exists(%s).\n", nameOfFunction);
	fprintf(stderr, "Exiting program ....\n");
	exit(EXIT_FAILURE);
}
//5
void error_Opendir(char * nameOfFunction, int error){
	fprintf(stderr, "ERROR: Unable to open directory(%s): %s.\n", nameOfFunction, strerror(error));
	fprintf(stderr, "Exiting program ....\n");
	exit(EXIT_FAILURE);
}

//6
void error_ExistingDirectory(char * nameOfFunction, DIR *fd){
	fprintf(stderr, "ERROR: Directory already exists(%s).\n", nameOfFunction);
	fprintf(stderr, "Exiting program ....\n");
	closedir(fd);
	exit(EXIT_FAILURE);
} 

//7
void error_Readdir(char* nameOfFunction, int error){
	fprintf(stderr, "ERROR: Unable to read directory(%s): %s.\n", nameOfFunction, strerror(error));
	fprintf(stderr, "Exiting program ....\n");
	exit(EXIT_FAILURE);
}

//8
void error_Mkdir(char * nameOfFunction, int error){
	fprintf(stderr, "ERROR: Unable to make directory(%s): %s.\n", nameOfFunction, strerror(error));
	fprintf(stderr, "Exiting program ....\n");
	exit(EXIT_FAILURE);
}

//9
void error_Stat(char* nameOfFunction, int error){
	fprintf(stderr, "ERROR: Unable to load info about file(%s): %s.\n", nameOfFunction, strerror(error));
	fprintf(stderr, "Exiting program ....\n");
	exit(EXIT_FAILURE);
}

//10
void error_Chmod(char * nameOfFunction, int error){
	fprintf(stderr, "ERROR: Unable to change access rights(%s): %s.\n", nameOfFunction, strerror(error));
	fprintf(stderr, "Exiting program ....\n");
	exit(EXIT_FAILURE);
}

//11
void error_Utime(char * nameOfFunction, int error){
	fprintf(stderr, "ERROR: Unable to change time(%s): %s.\n", nameOfFunction, strerror(error));
	fprintf(stderr, "Exiting program ....\n");
	exit(EXIT_FAILURE);
}

//12
void error_Mem(char * nameOfFunction){
	fprintf(stderr, "ERROR: Unable to allocate memory(%s).\n", nameOfFunction);
	fprintf(stderr, "Exiting program ....\n");
	exit(EXIT_FAILURE);
}

//13
void error_Arg(char * nameOfFunction){
	fprintf(stderr, "ERROR: Wrong number of arguments(%s).\n", nameOfFunction);
	fprintf(stderr, "Exiting program ....\n");
	exit(EXIT_FAILURE);
}

//14
void error_Fork(char * nameOfFunction, int error){
	fprintf(stderr, "ERROR: Unable to create child(%s): %s.\n", nameOfFunction, strerror(error));
	fprintf(stderr, "Exiting program ....\n");
	exit(EXIT_FAILURE);
}

//15
void error_Exec(char * nameOfFunction, int error, char * nameOfFunction2){
	fprintf(stderr, "ERROR: Unable to run code(%s) from(%s) : %s.\n", nameOfFunction2, nameOfFunction, strerror(error));
	fprintf(stderr, "Exiting program ....\n");
	exit(EXIT_FAILURE);
}

//16
void error_Waitpid(char * nameOfFunction, int error){
	fprintf(stderr, "ERROR: Child not terminated correctly(%s): %s.\n", nameOfFunction, strerror(error));
	fprintf(stderr, "Exiting program ....\n");
	exit(EXIT_FAILURE);
}

void error_Fdopen(char * nameOfFunction, int error){
	fprintf(stderr, "ERROR: Couldn 't make a FILE pointer(%s): %s.\n", nameOfFunction, strerror(error));
	fprintf(stderr, "Exiting program ....\n");
	exit(EXIT_FAILURE);
}

void error_Ftok(char * nameOfFunction, int error){
	fprintf(stderr, "ERROR: Ftok failed. Unable to produce a key(%s): %s.\n", nameOfFunction, strerror(error));
	fprintf(stderr, "Exiting program ....\n");
	exit(EXIT_FAILURE);
}

void error_Lseek(char * nameOfFunction, int error){
	fprintf(stderr, "ERROR: Lseek failed(%s): %s.\n", nameOfFunction, strerror(error));
	fprintf(stderr, "Exiting program ....\n");
	exit(EXIT_FAILURE);
}

void error_Shmget(char * nameOfFunction, int error){
	fprintf(stderr, "ERROR: Shmget failed(%s): %s.\n", nameOfFunction, strerror(error));
	fprintf(stderr, "Exiting program ....\n");
	exit(EXIT_FAILURE);
}

void error_Shmat(char * nameOfFunction, int error){
	fprintf(stderr, "ERROR: Shmat failed(%s): %s.\n", nameOfFunction, strerror(error));
	fprintf(stderr, "Exiting program ....\n");
	exit(EXIT_FAILURE);
}

void error_Ftruncate(char * nameOfFunction, int error){
	fprintf(stderr, "ERROR: Ftruncate failed(%s): %s.\n", nameOfFunction, strerror(error));
	fprintf(stderr, "Exiting program ....\n");
	exit(EXIT_FAILURE);
}

void error_Socket(char * nameOfFunction, int error){
	fprintf(stderr, "ERROR: Creating socket failed(%s): %s.\n", nameOfFunction, strerror(error));
	fprintf(stderr, "Exiting program ....\n");
	exit(EXIT_FAILURE);
}

void error_Bind(char * nameOfFunction, int error){
	fprintf(stderr, "ERROR: Bind failed(%s): %s.\n", nameOfFunction, strerror(error));
	fprintf(stderr, "Exiting program ....\n");
	exit(EXIT_FAILURE);
}

void error_Listen(char * nameOfFunction, int error){
	fprintf(stderr, "ERROR: Listen failed(%s): %s.\n", nameOfFunction, strerror(error));
	fprintf(stderr, "Exiting program ....\n");
	exit(EXIT_FAILURE);
}

void error_Select(char * nameOfFunction, int error){
	fprintf(stderr, "ERROR: Select failed(%s): %s.\n", nameOfFunction, strerror(error));
	fprintf(stderr, "Exiting program ....\n");
	exit(EXIT_FAILURE);
}

void error_Accept(char * nameOfFunction, int error){
	fprintf(stderr, "ERROR: Accept failed(%s): %s.\n", nameOfFunction, strerror(error));
	fprintf(stderr, "Exiting program ....\n");
	exit(EXIT_FAILURE);
}

void error_Connect(char * nameOfFunction, int error){
	fprintf(stderr, "ERROR: Connect failed(%s): %s.\n", nameOfFunction, strerror(error));
	fprintf(stderr, "Exiting program ....\n");
	exit(EXIT_FAILURE);
}

void error_Semget(char * nameOfFunction, int error){
	fprintf(stderr, "ERROR: Semget failed(%s): %s.\n", nameOfFunction, strerror(error));
	fprintf(stderr, "Exiting program ....\n");
	exit(EXIT_FAILURE);
}

void error_Semctl(char * nameOfFunction, int error){
	fprintf(stderr, "ERROR: Semctl failed(%s): %s.\n", nameOfFunction, strerror(error));
	fprintf(stderr, "Exiting program ....\n");
	exit(EXIT_FAILURE);
}

void error_Semop(char * nameOfFunction, int error){
	fprintf(stderr, "ERROR: Semop failed(%s): %s.\n", nameOfFunction, strerror(error));
	fprintf(stderr, "Exiting program ....\n");
	exit(EXIT_FAILURE);
}