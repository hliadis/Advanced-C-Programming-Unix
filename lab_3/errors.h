#include <sys/types.h>
#include <dirent.h>
#define SIZE 300
//1
void error_Write(char *, int);
//2
void error_Read(char *, int);
//3
void error_Open(char *, int);
//4
void error_ExistingFile(char *, int);
//5
void error_Opendir(char *, int);
//6
void error_ExistingDirectory(char *, DIR *);
//7
void error_Mkdir(char *, int);
//8
void error_Readdir(char*, int);
//9
void error_Stat(char*, int);
//10
void error_Chmod(char *, int);
//11
void error_Utime(char *, int);
//12
void error_Mem(char *);
//13
void error_Arg(char *);
//14
void error_Fork(char *, int);
//15
void error_Exec(char *, int error, char *);
//16
void error_Waitpid(char *, int);

void error_Fdopen(char * nameOfFunction, int error);

void error_Ftok(char * nameOfFunction, int error);

void error_Lseek(char * nameOfFunction, int error);

void error_Shmget(char * nameOfFunction, int error);

void error_Shmat(char * nameOfFunction, int error);

void error_Ftruncate(char * nameOfFunction, int error);

void error_Socket(char * nameOfFunction, int error);

void error_Bind(char * nameOfFunction, int error);

void error_Listen(char * nameOfFunction, int error);

void error_Select(char * nameOfFunction, int error);

void error_Accept(char * nameOfFunction, int error);

void error_Connect(char * nameOfFunction, int error);

void error_Semget(char * nameOfFunction, int error);

void error_Semctl(char * nameOfFunction, int error);

void error_Semop(char * nameOfFunction, int error);