CFILES = $(wildcard *.c)
OBJS   = $(patsubst %.c, %.o,$(CFILES))
CFLAGS=-I. -Wall -Wextra -Werror
BIN=gw-http
CC=gcc

%.o:%.c
	$(CC) $(CFLAGS) $(DEFINES) -o $@ -c $<

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(DEFINES) -o $(BIN) $^

clean:
	rm $(BIN) $(OBJS)
