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
