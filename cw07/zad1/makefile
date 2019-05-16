CC = gcc
FLAGS = -std=c99 -Wall

.PHONY: trucker loader spawner

all: trucker loader spawner

trucker:
	$(CC) $(FLAGS) trucker.c -o trucker keygen.c queue.c pack.c shared_mem.c

loader:
	$(CC) $(FLAGS) loader.c -o loader keygen.c queue.c pack.c shared_mem.c

spawner:
	$(CC) $(FLAGS) spawner.c -o spawner


clean:
	$(RM) trucker loader spawner