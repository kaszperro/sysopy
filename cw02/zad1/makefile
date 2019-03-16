CC = gcc
FLAGS = -std=c99 -Wall
SOURCE_NAME = generator


static:
	$(CC) $(FLAGS) -c $(SOURCE_NAME).c -o lib$(SOURCE_NAME).o
	ar rcs lib$(SOURCE_NAME).a lib$(SOURCE_NAME).o
	make clean

test:
	make static
	$(CC) $(FLAGS) main.c -o main lib$(SOURCE_NAME).a

clean:
	rm -f *.o