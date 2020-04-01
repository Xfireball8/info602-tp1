CC=gcc
CFLAGS=-g -Wall -Werror -pedantic -std=c99
GTKCFLAGS=-g $(shell pkg-config --cflags gtk+-3.0)
GTKLIBS=$(shell pkg-config --libs gtk+-3.0)
TP_EXEC_TITLE=diletta_faisal_tp


all: clean
	$(CC) $(GTKCFLAGS) -o $(TP_EXEC_TITLE) main-window.c $(GTKLIBS)

clean:
	rm -rf $(TP_EXEC_TITLE)
