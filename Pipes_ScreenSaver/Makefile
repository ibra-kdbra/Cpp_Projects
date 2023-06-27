NAME=pipes
SOURCE=main.c
CFLAGS=-O4 -s

build:
	cc $(SOURCE) -o $(NAME) $(CFLAGS)

install:
	rm -f "/usr/local/bin/$(NAME)"
	ln -s "`pwd`/$(NAME)" "/usr/local/bin/$(NAME)"

uninstall:
	rm -f "/usr/local/bin/$(NAME)"
