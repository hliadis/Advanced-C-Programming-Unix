
/*P2CRYPT:
 * Encrypts the data that it receives from the std input
 * using a keyword and prints them in its std output.
 */
#include "pipes.h"
#include "errors.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

int main(int argc, char* argv[]){
	
	char* buf;
	char name[] = "p2crypt.c"; 
	ssize_t read_value_temp;
	int i,j;
	
	if(argc != 2){
		error_Arg(name);
	}
	
	buf = (char*)malloc(512);
	if(buf == NULL){
		error_Mem(name);
	}
	
	do{
		//read data.
		read_value_temp = myread(0,buf,512);
		
		if (read_value_temp == -1){
			free(buf);
			error_Read(name, errno);
		}
		
		//encrypts the data using keyword and the bitwise xor.
		for(i = 0 , j = 0; i < read_value_temp ; i++, j++){
			if  (argv[1][j] == '\0'){
				j = 0;
			}
			buf[i] = buf[i] ^ argv[1][j];
		}
		
		//write data.
		if (mywrite(1,buf,read_value_temp) == -1){
			free(buf);
			error_Write(name, errno);
		}
		
		if (read_value_temp < 512){
			free(buf);
			break;
		}
		
	}while(1);
	
	return(0);
	
}