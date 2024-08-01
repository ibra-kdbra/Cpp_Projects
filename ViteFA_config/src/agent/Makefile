.SUFFIXES: .c .o

COMPILE = $(CC)  -Wall -D_FILE_OFFSET_BITS=64 -D_GNU_SOURCE -g -O3 -DDEBUG_FLAG
INC_PATH = -I.. -I/usr/local/include
LIB_PATH =  -L/usr/local/lib -lfastcommon -lserverframe -lshmcache -lpthread \
            -ldl
TARGET_PATH = /usr/bin
CONFIG_PATH = $(TARGET_CONF_PATH)

ALL_OBJS = ../common/fcfg_proto.o ../common/fcfg_global.o fcfg_agent_func.o \
           fcfg_agent_handler.o fcfg_agent_global.o

ALL_PRGS = fcfg_agentd

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
