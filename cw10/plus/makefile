CC = gcc
FLAGS = -std=c99 -Wall


.PHONY: server client

server:
	$(CC) $(FLAGS) server.c -o server -pthread


client:
	$(CC) $(FLAGS) client.c -o client

clean:
	$(RM) server client