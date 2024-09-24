CC = gcc
CFLAGS = -c -g -Wall

EXECS = clientGame serverGame

all: $(EXECS)


clientGame: utils.o clientGame.o 
	$(CC) utils.o clientGame.o -o clientGame

clientGame.o: clientGame.c
	$(CC) $(CFLAGS) clientGame.c
		
utils.o: utils.c
	$(CC) $(CFLAGS) utils.c

serverGame: utils.o game.o serverGame.o 
	$(CC) utils.o game.o serverGame.o -o serverGame

serverGame.o: serverGame.c
	$(CC) $(CFLAGS) serverGame.c
	
game.o: game.c
	$(CC) $(CFLAGS) game.c
	
clean:
	rm -f  *.o
	rm -f $(EXECS)
