CC=gcc

# original version was not showing in xscreensaver little window;
# it also did not respond to resize window events (for example when run from terminal).

CFLAGS = -Wall -O2
CLIBS = -lX11 -lm

all : lavanet.c vroot.h
		$(CC) $(CFLAGS) -o lava lavanet.c $(CLIBS)

.PHONY: clean

clean :
		-rm lava