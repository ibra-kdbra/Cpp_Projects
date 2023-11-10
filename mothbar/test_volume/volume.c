#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/soundcard.h>

int 
main()
{
	char mixer[10] = "/dev/mixer";
	char *name;
	int fd, aux, lvol, rvol;
	
	name = mixer;
	fd = open(name, O_RDWR);
	ioctl(fd, MIXER_READ(0),&aux);
	close(fd);
	lvol = aux & 0x7f;
	rvol = (aux >> 8) & 0x7f;
	
	if (lvol > rvol) 
		printf("%d%%\n", lvol);
	else if ((lvol == rvol) && (lvol == 0))
		printf("mute\n");
	else 
		printf("%d%%\n", rvol);

	return (0);
}
