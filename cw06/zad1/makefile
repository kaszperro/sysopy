CC = gcc
FLAGS = -std=c99 -Wall

.PHONY: server client

server:
	$(CC) $(FLAGS) server.c keygen.c -o server

client:
	$(CC) $(FLAGS) client.c keygen.c -o client