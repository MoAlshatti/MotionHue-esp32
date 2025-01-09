CC=gcc
INCLUDE=-Iinclude
LIBRARY=-lcurl
CFLAGS=-Wall -Wextra $(INCLUDE)

SRCS=src/server.c src/hue.c
OBJS=obj/server.o obj/hue.o
BIN=bin/server

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(LIBRARY) -o $@ $^ 

obj/%.o:src/%.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	rm -rf $(OBJS) $(BIN)
