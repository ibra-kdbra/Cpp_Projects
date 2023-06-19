CPP=g++
CC=gcc
CFLAGS=-lncurses -Wall -g -std=c++17
PREFIX ?= /usr/local
MANPREFIX ?= $(PREFIX)/share/man
INSTALL ?= install

BIN = tsnake

all: $(BIN) 

tsnake: tsnake.o InputParser.o
	$(CPP) tsnake.o InputParser.o -o tsnake $(CFLAGS)

tsnake.o: tsnake.cpp
	$(CPP) -c tsnake.cpp $(CFLAGS)

InputParser.o: InputParser.cpp
	$(CPP) -c InputParser.cpp

man: tsnake.1

tsnake.1: $(BIN)
	help2man --include tsnake.h2m -o tsnake.1 ./tsnake

install: $(BIN) 
	$(INSTALL) -m 0755 -d $(DESTDIR)$(PREFIX)/bin
	$(INSTALL) -m 0755 $(BIN) $(DESTDIR)$(PREFIX)/bin
	$(INSTALL) -m 0755 -d $(DESTDIR)$(MANPREFIX)/man1
	$(INSTALL) -m 0644 $(BIN).1 $(DESTDIR)$(MANPREFIX)/man1

uninstall:
	$(RM) $(DESTDIR)$(PREFIX)/bin/$(BIN)
	$(RM) $(DESTDIR)$(MANPREFIX)/man1/$(BIN).1

clean:
	$(RM) -f $(BIN) *.o

skip: ;

.PHONY: install uninstall clean
