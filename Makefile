CC=gcc
CFLAGS=-Wall -pthread -std=c99
TARGET=osproj
SRC=http.c mlfb.c network.c node.c queue.c rcb.c rr.c sjf.c sws.c
OBJ=$(SRC:.c=.o)
HEADER=*.h

$(TARGET): $(OBJ) $(HEADER)
	mkdir -p out
	mkdir -p out/obj
	$(CC) $(CFLAGS) -o out/$(TARGET) $(OBJ)
	mv *.o out/obj