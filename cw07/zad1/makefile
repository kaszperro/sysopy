CC = gcc
FLAGS = -std=c99 -Wall

.PHONY: trucker worker spawner

all: trucker worker spawner

trucker:
	$(CC) $(FLAGS) trucker.c -o trucker keygen.c queue.c pack.c shared_mem.c

worker:
	$(CC) $(FLAGS) worker.c -o worker keygen.c queue.c pack.c shared_mem.c

spawner:
	$(CC) $(FLAGS) spawner.c -o spawner


clean:
	$(RM) trucker worker spawner