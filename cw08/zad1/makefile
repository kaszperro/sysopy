CC = gcc
FLAGS = -std=c99 -Wall
HEADERS = image.h filter.h
SRCS = image.c filter.c

.PHONY: main filter_generator report_generator

main:
	$(CC) $(FLAGS) main.c -o main $(HEADERS) $(SRCS) -lm -lpthread

filter_generator:
	$(CC) $(FLAGS) filter_generator.c -o filter_generator $(HEADERS) $(SRCS) -lm

report_generator:
	$(CC) $(FLAGS) report_generator.c -o report_generator

clean:
	$(RM) main filter_generator report_generator