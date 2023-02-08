but : snake

OFILES =  	snake.o \

CC = gcc

CFLAGS = -Wall -ansi -pedantic -g

snake.o : snake.c
	gcc -c snake.c

snake : $(OFILES)
	$(CC) $(CFLAGS) -o snake $(OFILES) -lgraph

run : snake
	./snake