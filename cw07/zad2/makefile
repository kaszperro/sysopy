CC = gcc
FLAGS = -std=c99 -Wall -DPOSIX -lrt -pthread

SOURCES = ../zad1/

.PHONY: trucker worker spawner

all: trucker worker spawner

trucker:
	$(CC) $(FLAGS) -I$(SOURCES) $(SOURCES)trucker.c -o trucker $(SOURCES)keygen.c $(SOURCES)queue.c $(SOURCES)pack.c shared_mem.c

worker:
	$(CC) $(FLAGS) -I$(SOURCES) $(SOURCES)worker.c -o worker $(SOURCES)keygen.c $(SOURCES)queue.c $(SOURCES)pack.c shared_mem.c

spawner:
	$(CC) $(FLAGS) -I$(SOURCES) $(SOURCES)spawner.c -o spawner


clean:
	$(RM) trucker worker spawner