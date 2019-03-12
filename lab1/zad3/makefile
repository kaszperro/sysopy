CC = gcc
FLAGS = -std=c99 -Wall
LIB_NAME = libfind
LIB_PATH = ../zad1
TEST_PATH = ../zad2
TEST_SOURCE_NAME = main.c

static:
	$(CC) $(FLAGS) -I$(LIB_PATH) $(LIB_PATH)/$(LIB_NAME).a $(TEST_PATH)/$(TEST_SOURCE_NAME) -o main

shared:
	$(CC) $(FLAGS) -I$(LIB_PATH) -L$(LIB_PATH) -lfind $(TEST_PATH)/$(TEST_SOURCE_NAME) -o main

dynamic:
	$(CC) $(FLAGS) -I$(LIB_PATH) $(TEST_PATH)/$(TEST_SOURCE_NAME) -o main -ldl -D DYNAMIC	

test:
	LD_LIBRARY_PATH=$(LIB_PATH) ./main $(shell cat args.txt)

test_all:
	printf "static\n========\n" > $(RAPORT)
	make static
	make test
	printf "\n\nshared\n========\n" >> $(RAPORT)
	make shared
	make test
	printf "\n\ndynamic\n========\n" >> $(RAPORT)
	make dynamic
	make test
