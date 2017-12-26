all:main

CC=gcc
cflags=-O2 -Wall -g

main:main.o my_icmp.o my_teredo.o simple_conf.o
	$(CC) -o $@ $^ $(cflags)

%.o:%.c
	$(CC) -c $< $(cflags)

clean:
	rm *.o main
