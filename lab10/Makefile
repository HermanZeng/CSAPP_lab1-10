# Makefile for SJTU ICS-2016 lab 10

CC=gcc

all: proxy

proxy: proxy.o csapp.o
	$(CC) $^ -lpthread -o $@

%.o: %.c
	$(CC) -c $< -o $@ -g -Wall


clean:
	rm -f *~ *.o proxy proxy.log
