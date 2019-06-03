CC = gcc
FLAGS = -std=c99 -Wall

.PHONY: server client

server:
	$(CC) $(FLAGS) server.c -o server  -pthread
client:
	$(CC) $(FLAGS) client.c -o client list.c -pthread

clean:
	$(RM) server