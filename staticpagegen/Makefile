VERSION = 0.1.1
PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man
DOCPREFIX = ${PREFIX}/share/doc

# On GNU+Linux, the compilation will fail. This is due to glibc wrongfully
# defining getopt() in getopt.h instead of unistd.h. Either;
# (a) remove -std=89 from CFLAGS and remove LDFLAGS from the Makefile, or
# (b) wrongfully include getopt.h instead of unistd.h in main.c and remove
#	LDFLAGS from the Makefile, or
# (c) use a proper standard library by installing musl, setting CC to musl-gcc.
#
# option (c) is recommended (for a better world).
CC = cc

SRC = xhtmlbasic1-0.c staticpagegen.c main.c
OBJ = ${SRC:.c=.o}

CFLAGS = -std=c89 -pedantic -Wall -Wextra -Werror -O2 -pipe
LDFLAGS = -static -static-pie

all: staticpagegen

main.o: main.c
	sed -i "s/^#define STATICPAGEGEN_VERSION\ .*$$/\
#define STATICPAGEGEN_VERSION\ \"${VERSION}\"/" staticpagegen.h
	${CC} -c main.c ${CFLAGS}

.c.o:
	${CC} -c $< ${CFLAGS}

staticpagegen: ${OBJ}
	${CC} ${OBJ} -o $@ ${LDFLAGS}

install: all
	mkdir -p ${DESTDIR}${PREFIX}/bin
	mkdir -p ${DESTDIR}${MANPREFIX}/man1
	mkdir -p ${DESTDIR}${DOCPREFIX}/staticpagegen
	install -m 755 staticpagegen ${DESTDIR}${PREFIX}/bin
	install -m 644 staticpagegen.1 ${DESTDIR}${MANPREFIX}/man1
	install -m 644 README COPYING ${DESTDIR}${DOCPREFIX}/staticpagegen

uninstall: all
	rm -f ${DESTDIR}${PREFIX}/bin/staticpagegen \
	    ${DESTDIR}${MANPREFIX}/man1/staticpagegen.1
	rm -rf ${DESTDIR}${DOCPREFIX}/staticpagegen

dist: clean
	mkdir -p staticpagegen-${VERSION}
	cp Makefile ${SRC} staticpagegen.1 staticpagegen.h \
	    xhtmlbasic1-0.h COPYING README staticpagegen-${VERSION}
	tar -cf - staticpagegen-${VERSION} | bzip2 -9 > \
	    staticpagegen-${VERSION}.tar.bz2
	rm -rf staticpagegen-${VERSION}
	@echo "Make sure all header files and man pages are also distributed."
	
clean:
	rm -f ${OBJ} staticpagegen
