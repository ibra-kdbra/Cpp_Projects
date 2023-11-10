
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
 
#include "declarations.h"

#define CLOCKDELAY 3

char line[MAXFIFO];
 
void
writefifo() 
{
	int fd;
	
	fd = open(FIFOFILE, O_WRONLY);
	if (fd) {
		write(fd, line, MAXFIFO);
		close(fd);
	}
}
 
int 
main()
{
	pid_t ppid;
	time_t clock;
	struct tm* current_time;
	char hour[3], min[3], newline[MAXFIFO]; 
	 
	ppid = getppid();
	strcpy(newline,"\0");
	while (1) {
		clock = time(NULL);
		current_time = localtime(&clock);
		sprintf(hour, "%d", current_time->tm_hour);
		sprintf(min, "%d", current_time->tm_min);
		strcpy(newline, "\0");
		strcat(newline, "CLOCK@");
		strcat(newline, "[");
		strcat(newline, hour);
		strcat(newline, ":");
		strcat(newline, min);
		strcat(newline, "]"); 
		strcat(newline, "\n");
		if (kill(ppid, 0))
			break;
		if (strncmp(newline,line,strlen(newline))) {
			strcpy(line,newline);
			writefifo();
		}
		sleep(CLOCKDELAY);
	}
	free(current_time);
	return 0;
}
