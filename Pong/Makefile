CC=gcc
CFLAGS=-Wall -Werror -ggdb3
LINK=-lSDL2 -lSDL2_mixer -lSDL2_ttf -ldl

OBJS=tffunctions.o sys.o

all: pong

pong: $(OBJS) main.c
	$(CC) -o $@ $^ $(CFLAGS) $(LINK)

clean:
	rm -f *.o pong

%.o: %.c
	$(CC) $^ -o $@ -c $(CFLAGS)
