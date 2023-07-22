CFLAGS=-Wall -O2
CC=gcc

PROGS=minidmx setdmx

all: $(PROGS)


install: all
	cp $(PROGS) /usr/bin 
