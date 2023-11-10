#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
 
#define MAXFIFO 100

char _clock[MAXFIFO];
 
int 
main()
{
	time_t clock;
	struct tm* current_time;
	char hour[3], min[3], out[8]; 
 
	strcpy(_clock,"\0");
	while (1) {
		clock = time(NULL);
		current_time = localtime(&clock);
		sprintf(hour, "%d", current_time->tm_hour);
		sprintf(min, "%d", current_time->tm_min);
		strcpy(out, "\0");
		strcat(out, "[");
		strcat(out, hour);
		strcat(out, ":");
		strcat(out, min);
		strcat(out, "]"); 
		if (strncmp(out,_clock,strlen(out))) {
			strcpy(_clock,out);
			printf("%s\n", out);
		}
		sleep (2);
	}
	return 0;
}
