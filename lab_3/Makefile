.PHONY: clean

all: agent server 
	rm errors.o agent.o server.o
	
agent: agent.o errors.o
	gcc -Wall -g agent.o errors.o -o agent
	
server: server.o errors.o 
	gcc -Wall -g server.o errors.o -o server
	
server.o: server.c errors.h
	gcc -Wall -g -c server.c
	
agent.o: agent.c errors.h
	gcc -Wall -g -c agent.c
	
errors.o: errors.c errors.h
	gcc -Wall -g -c errors.c 


	
