CC=gcc
INCLUDE=-Iinclude
LIBRARY=-lcurl
CFLAGS=-Wall -Wextra $(INCLUDE)
#cJSON is an external library
SRCS=src/server.c src/hue.c src/cJSON.c	
OBJS=obj/server.o obj/hue.o obj/cJSON.o
BIN=bin/server

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) -o $@ $^ $(LIBRARY)

obj/%.o:src/%.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	rm -rf $(OBJS) $(BIN)
