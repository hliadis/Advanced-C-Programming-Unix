#include "pipes.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

ssize_t myread(int fd, void *buf, size_t count){
	ssize_t read_value, read_value_temp;
	
	read_value = read(fd,buf,count);
	read_value_temp = count;
	if(read_value == -1){
		return(-1);
	}
	
	if (read_value < count){
		read_value_temp = 0;
		do{
			read_value_temp = read_value_temp + read_value;
			read_value = read(fd,buf + read_value_temp, count - read_value_temp);
			if(read_value == -1){
				return(-1);
			}
			if(read_value == 0){
				break;
			}
		}while(1);
	}	
	return(read_value_temp);
}

ssize_t mywrite(int fd, void *buf, size_t count){
	
	ssize_t write_value, write_value_temp;
	
	write_value = write(fd,buf,count);
	write_value_temp = count;
	if(write_value == -1){
		return(-1);
	}
	
	if (write_value < count){
		write_value_temp = 0;
		do{
			write_value_temp = write_value_temp + write_value;
			write_value = write(fd,buf + write_value_temp, count - write_value_temp);
			if(write_value == -1){
				
				return(-1);
			}
			if(write_value == 0){
				break;
			}
		}while(1);
	}	
	return(write_value_temp);
}
