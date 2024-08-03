.SUFFIXES: .c .lo

TARGET_PATH=/usr/local/bin
CONFIG_PATH=/usr/local/etc

COMPILE = $(CC) -O0 -Wall -D_FILE_OFFSET_BITS=64 -g
INC_PATH = -I../common -I/usr/local/include
LIB_PATH = -L/usr/local/lib -lfastcommon -lpthread

SHARED_OBJS = ../common/fcfg_proto.lo fcfg_admin_func.lo
ADMIN_SHARED_OBJS = fcfg_admin_env.lo fcfg_admin_config.lo

ALL_OBJS = $(SHARED_OBJS) $(ADMIN_SHARED_OBJS)

ALL_HEADERS = fcfg_admin.h ../common/fcfg_types_admin.h
SHARED_LIBS = libfcfgadmin.so
ALL_PRGS =
ALL_LIBS = $(SHARED_LIBS)

all: $(ALL_OBJS) $(ALL_PRGS) $(ALL_LIBS)
libfcfgadmin.so:
	$(COMPILE) -o $@ $< -shared $(ALL_OBJS) $(LIB_PATH)
.c:
	$(COMPILE) -o $@ $< $(ALL_OBJS) $(LIB_PATH) $(INC_PATH)
.c.lo:
	$(COMPILE) -c -fPIC -o $@ $<  $(INC_PATH)
install:
	mkdir -p $(DESTDIR)/usr/local/lib
	mkdir -p $(DESTDIR)/usr/lib
	mkdir -p $(DESTDIR)/usr/local/include/fcfg
	cp -f $(ALL_HEADERS) $(DESTDIR)/usr/local/include/fcfg
	install -m 755 $(SHARED_LIBS) $(DESTDIR)/usr/local/lib
	install -m 755 $(SHARED_LIBS) $(DESTDIR)/usr/lib
clean:
	rm -f $(ALL_OBJS) $(ALL_PRGS) $(ALL_LIBS)
