CC = gcc
FLAGS = -std=c99 -Wall
SOURCE_NAME = generator
GEN_PROG_NAME  = generator_prog

static:
	$(CC) $(FLAGS) -c $(SOURCE_NAME).c -o lib$(SOURCE_NAME).o
	ar rcs lib$(SOURCE_NAME).a lib$(SOURCE_NAME).o
	make clean

prog:
	make static
	$(CC) $(FLAGS) $(GEN_PROG_NAME).c -o $(GEN_PROG_NAME) lib$(SOURCE_NAME).a

test:
	make prog
	rm -f report.txt

	for byte_num in 1 4 512  1024  4096 8192 ; do \
		for rec_num in 10 100 ; do \
			./$(GEN_PROG_NAME) generate dane $$rec_num $$byte_num ; \
			./$(GEN_PROG_NAME) copy dane dane.lib $$rec_num $$byte_num lib ; \
			./$(GEN_PROG_NAME) copy dane dane.sys $$rec_num $$byte_num sys ; \
			./$(GEN_PROG_NAME) sort dane.lib $$rec_num $$byte_num lib ; \
			./$(GEN_PROG_NAME) sort dane.sys $$rec_num $$byte_num sys ; \
		done; \
	done


clean:
	rm -f *.o