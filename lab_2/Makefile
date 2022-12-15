hw2: dirlist p2archive p2crypt p2unarchive errors.o hw2.o
	gcc -Wall -g errors.o hw2.o -o hw2

hw2.o: hw2.c
	gcc -Wall -g -c hw2.c

dirlist: errors.o dirlist.o
	gcc -Wall -g errors.o dirlist.o -o dirlist

dirlist.o: dirlist.c
	gcc -Wall -g -c dirlist.c

p2archive: errors.o pipes.o p2archive.o
	gcc -Wall -g errors.o pipes.o p2archive.o -o p2archive

p2archive.o: p2archive.c
	gcc -Wall -g -c p2archive.c

p2crypt: errors.o pipes.o p2crypt.o
	gcc -Wall -g errors.o pipes.o p2crypt.o -o p2crypt

p2crypt.o: p2crypt.c
	gcc -Wall -g -c p2crypt.c

p2unarchive: errors.o pipes.o p2unarchive.o
	gcc -Wall -g errors.o pipes.o p2unarchive.o -o p2unarchive

p2unarchive.o: p2unarchive.c
	gcc -Wall -g -c p2unarchive.c

errors.o: errors.h errors.c
	gcc -Wall -g -c errors.c

pipes.o: pipes.h pipes.c
	gcc -Wall -g -c pipes.c
