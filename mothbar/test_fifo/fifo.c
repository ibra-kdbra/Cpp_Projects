
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#define FIFOFILE "/tmp/fifobar"
#define MAXFIFO 100

char _desk[MAXFIFO], _clock[MAXFIFO], _vol[MAXFIFO], _separator[MAXFIFO] = "     ";

int
checkfifostring (char *fifostring)
{
	char *text;
	char tmp[MAXFIFO];
		
	// ignore fifo string without @ and without \n
	text = fifostring;
	if (!strchr(text, '@') || !strchr(text, '\n'))
		return 0;
	
	strsep(&text, "@");
	strcat(text,"\0");
	text = strsep(&text, "\n");
	
	if (!strncmp(fifostring, "DESK", 4)) {
		strcpy(_desk,text);
	} else if (!strncmp(fifostring, "CLOCK", 5)) {
		strcpy(_clock,text);
	} else if (!strncmp(fifostring, "VOL", 3)) {
		strcpy(_vol,text);
	}		
	
	strcpy(tmp,"\0");
	strcat(tmp,_desk);
	strcat(tmp,_separator);
	strcat(tmp,_vol);
	strcat(tmp,_clock);
	printf("Output+ %s\n", tmp);
	
	return 0;
}

int 
main ()
{
    int fd;
	char *barfifo;
	char fifostring[MAXFIFO];
	    
    barfifo = FIFOFILE;
    mkfifo(barfifo, 0666);
    while (1) {
        fd = open(barfifo,O_RDONLY);
        read(fd, fifostring, MAXFIFO);
        close(fd);
		if (!strncmp(fifostring, "exit", 4)) 
			break;
		checkfifostring(fifostring);
    }
    printf("Exit\n");
    // TODO remove fifo file
    return 0;
}
