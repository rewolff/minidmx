CFLAGS=-Wall -O2
CC=gcc

PROGS=minidmx setdmx print_universe

all: $(PROGS)


install: all
	cp $(PROGS) /usr/bin 
