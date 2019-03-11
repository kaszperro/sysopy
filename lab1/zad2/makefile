CC = gcc
FLAGS = -std=c99 -Wall
LIB_NAME = libfind
LIB_PATH = ../zad1


static:
	make -C $(LIB_PATH) static
	$(CC) $(FLAGS) -L$(LIB_PATH) -I$(LIB_PATH) $(LIB_PATH)/$(LIB_NAME).a main.c -o main

shared:
	make -C $(LIB_PATH) shared
	$(CC) $(FLAGS) -L$(LIB_PATH) -I$(LIB_PATH) -lfind main.c -o main
	make -C $(LIB_PATH) install

dynamic:
	make -C $(LIB_PATH) shared
	$(CC) $(FLAGS) -L$(LIB_PATH) -I$(LIB_PATH) main.c -ldl -D DYNAMIC -o main

test:
	LD_LIBRARY_PATH=$(LIB_PATH) ./execute_tests.sh



