.SUFFIXES: .c .o

COMPILE = $(CC)  -Wall -D_FILE_OFFSET_BITS=64 -D_GNU_SOURCE -g -O3 -DDEBUG_FLAG
INC_PATH = -I/usr/local/include -I.. $(shell mysql_config --include)
LIB_PATH =  -lfastcommon -lserverframe $(shell mysql_config --libs)
TARGET_PATH = /usr/bin
CONFIG_PATH = $(TARGET_CONF_PATH)

ALL_OBJS = ../common/fcfg_proto.o ../common/fcfg_global.o fcfg_server_func.o \
           fcfg_server_handler.o fcfg_server_global.o fcfg_server_dao.o \
           fcfg_server_env.o fcfg_server_cfg.o fcfg_server_push.o

ALL_PRGS = fcfg_serverd

all: $(ALL_PRGS)

$(ALL_PRGS): $(ALL_OBJS)

.o:
	$(COMPILE) -o $@ $<  $(LIB_PATH) $(INC_PATH)
.c:
	$(COMPILE) -o $@ $<  $(ALL_OBJS) $(LIB_PATH) $(INC_PATH)
.c.o:
	$(COMPILE) -c -o $@ $<  $(INC_PATH)

install:
	mkdir -p $(TARGET_PATH)
	cp -f $(ALL_PRGS) $(TARGET_PATH)
clean:
	rm -f *.o $(ALL_OBJS) $(ALL_PRGS)
