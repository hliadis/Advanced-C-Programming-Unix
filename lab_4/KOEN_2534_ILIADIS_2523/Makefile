all: errors.o step1.o hw4.o
	gcc errors.o step1.o -o step1
	gcc errors.o hw4.o -o hw4
	rm errors.o step1.o hw4.o  
	clear

errors.o: errors.c errors.h
	gcc -Wall -g -c errors.c

step1.o: step1.c
	gcc -Wall -g -c step1.c

hw4.o: hw4.c
	gcc -Wall -g -c hw4.c
