OBJS = main.o bfinterpreter.o
HEADERS = bfinterpreter.h
CC = gcc
CFLAGS = -c -std=gnu89 -Wall -Werror -pedantic
OUT = brainfinterpreter

.PHONY: clean

all: $(OUT)

$(OUT): $(OBJS) $(HEADERS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJS) $(OUT)
