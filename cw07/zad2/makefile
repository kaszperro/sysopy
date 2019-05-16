CC = gcc
FLAGS = -std=c99 -Wall -DPOSIX -lrt -pthread

SOURCES = ../zad1/

.PHONY: trucker loader spawner

all: trucker loader spawner

trucker:
	$(CC) $(FLAGS) -I$(SOURCES) $(SOURCES)trucker.c -o trucker $(SOURCES)keygen.c $(SOURCES)queue.c $(SOURCES)pack.c shared_mem.c

loader:
	$(CC) $(FLAGS) -I$(SOURCES) $(SOURCES)loader.c -o loader $(SOURCES)keygen.c $(SOURCES)queue.c $(SOURCES)pack.c shared_mem.c

spawner:
	$(CC) $(FLAGS) -I$(SOURCES) $(SOURCES)spawner.c -o spawner


clean:
	$(RM) trucker loader spawner