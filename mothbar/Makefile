BINS = mothbar m_desk m_clock
LOCATION = /usr/local/bin
CFLAGS = 
LDFLAGS = -I/usr/local/include -L/usr/local/lib -lxcb

all: $(BINS)

mothbar: 
	$(CC) $(CFLAGS) $(LDFLAGS) -o mothbar mothbar.c 

mbar_desk: 
	$(CC) $(CFLAGS) $(LDFLAGS) -o m_desk m_desk.c 
	
mbar_clock: 
	$(CC) $(CFLAGS) $(LDFLAGS) -o m_clock m_clock.c 	

install: all
	mkdir -p /usr/local/bin
	cp -f mothbar $(LOCATION)
	cp -f m_desk $(LOCATION)
	cp -f m_clock $(LOCATION)

uninstall:
	rm -f $(LOCATION)/mothbar
	rm -f $(LOCATION)/m_desk
	rm -f $(LOCATION)/m_clock

clean:
	rm -f $(BINS)
