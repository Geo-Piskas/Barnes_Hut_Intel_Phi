CC=icc
CFLAGS=-openmp -std=c99 -Wall -O3
BIN=../../bin/
BUILD=../../build/

all: barneshut

barneshut: main.o octree.o physics.o io.o types.o
	$(CC) $(CFLAGS) -o $(BIN)barneshut-s $(BUILD)*.o

main.o: $(SRC)main.c
	$(CC) $(CFLAGS) -o $(BUILD)main.o -c $(SRC)main.c

octree.o: $(SRC)octree.c
	$(CC) $(CFLAGS) -o $(BUILD)octree.o -c $(SRC)octree.c

physics.o: $(SRC)physics.c
	$(CC) $(CFLAGS) -o $(BUILD)physics.o -c $(SRC)physics.c

io.o: $(SRC)io.c
	$(CC) $(CFLAGS) -o $(BUILD)io.o -c $(SRC)io.c
	
types.o: $(SRC)types.c
	$(CC) $(CFLAGS) -o $(BUILD)types.o -c $(SRC)types.c