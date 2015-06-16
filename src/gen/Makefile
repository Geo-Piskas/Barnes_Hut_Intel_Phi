CC=icc
CFLAGS=-std=c99 -Wall -O3
BIN=../../bin/
BUILD=../../build/

all: gen

gen: main.o
	$(CC) $(CFLAGS) -o $(BIN)gen $(BUILD)main.o

main.o: $(SRC)main.c
	$(CC) $(CFLAGS) -o $(BUILD)main.o -c $(SRC)main.c