CC = gcc
FLAGS = -std=c99 -Wall

SOURCES_PATH = ../zad1

.PHONY: server client


server:
	$(CC) $(FLAGS) -o server -I$(SOURCES_PATH) $(SOURCES_PATH)/server.c $(SOURCES_PATH)/keygen.c que.c $(SOURCES_PATH)/utils.c -lrt

client:
	$(CC) $(FLAGS) -o client -I$(SOURCES_PATH) $(SOURCES_PATH)/client.c $(SOURCES_PATH)/keygen.c que.c $(SOURCES_PATH)/utils.c -lrt