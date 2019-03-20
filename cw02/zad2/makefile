CC = gcc
FLAGS = -std=c99 -Wall
DIR_NAME = files_dir
NFTW_NAME = files_nftw

static_dir:
	$(CC) $(FLAGS) -c $(DIR_NAME).c -o $(DIR_NAME).o
	ar rcs $(DIR_NAME).a $(DIR_NAME).o

static_nftw:
	$(CC) $(FLAGS) -c $(NFTW_NAME).c -o $(NFTW_NAME).o
	ar rcs $(NFTW_NAME).a $(NFTW_NAME).o

dir:
	make static_dir
	$(CC) $(FLAGS) main.c -o main $(DIR_NAME).a
	make clean

nftw:
	make static_nftw
	$(CC) $(FLAGS) main.c -o main $(NFTW_NAME).a
	make clean

test:
	./main ~/Dokumenty/AGH/sysopy '>' "2019-03-20 11:39:26"

clean:
	rm -f *.o