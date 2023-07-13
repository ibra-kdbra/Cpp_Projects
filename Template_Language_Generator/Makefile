CPY ?= cp
RM ?= rm -f

PROG = cgen
SRC = .
DEST ?= /usr/local/bin

all: # Dummy

install:
	$(CPY) $(SRC)/$(PROG) $(DEST)/$(PROG)

uninstall:
	$(RM) $(DEST)/$(PROG)

.PHONY: all install uninstall

