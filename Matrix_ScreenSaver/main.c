
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>

#define lenof(l) (sizeof(l) / sizeof(l[0]))

#include "config.h"

unsigned int *col;
unsigned int colsize = 0;

struct winsize _winsize;
#define W _winsize.ws_col
#define H _winsize.ws_row
void sigwinch(int _) {
	(void)_;
	ioctl(0, TIOCGWINSZ, &_winsize);
	if (W > colsize) {
		colsize = W;
		col = realloc(col, W * sizeof(col[0]));
		for (unsigned int i = 0; i < colsize; ++i) {
			if (col[i] == 0)
				col[i] = rand() % OPTGAP;
		}
	}
}

char _sgetcharwaiting;
char sgetchar() {
	ioctl(0, FIONREAD, &_sgetcharwaiting);
	if (_sgetcharwaiting == 0) return 0;
	return getchar();
}

typedef struct {
	int x;
	int y;
	int l;
	unsigned char r;
	unsigned char g;
	unsigned char b;
} Line;
Line line[OPTBUFSIZE];
Line *linetail;
Line *linehead;
Line *lineend;

int main() {

	// Setup
	sigwinch(0);
	signal(SIGWINCH, sigwinch);
	srand(time(NULL));

	// Term
	#ifdef OPTBOLD
		printf("\x1b[?1049h\x1b[?25l\x1b[1m\x1b[2J");
	#else
		printf("\x1b[?1049h\x1b[?25l\x1b[2J");
	#endif
	struct termios tcur, tres;
	tcgetattr(0, &tcur);
	tcgetattr(0, &tres);
	tcur.c_lflag &= ~(ICANON|ECHO|ISIG);
	tcsetattr(0, TCSANOW, &tcur);

	// Init ptrs
	linetail = &line[0];
	linehead = &line[0];
	lineend = &line[lenof(line) - 1];
	
	// Mainloop
	while (1) {

		static char c;
		c = sgetchar();
		switch (c) {
			case 3: // ctrl-c
				goto l_end;
		}
		for (unsigned int x = 0; x < colsize; ++x) {
			if (col[x] > OPTGAP) {
				static int randn;
				randn = rand();
				if (randn % 100 < OPTCHANCE) {
					linehead->x = x + 1;
					linehead->l = (randn % (OPTSIZEMAX - OPTSIZEMIN)) + OPTSIZEMIN;
					linehead->y = 1; // linux terms start at 1 don't ask why
					randcolor();
					linehead->r = color[0];
					linehead->g = color[1];
					linehead->b = color[2];
					if (linehead == lineend)
						linehead = line;
					else
						++linehead;
					col[x] = 0;
				}
			} else {
				col[x] += 1;
			}
		}
		static Line *p;
		for (p = linetail; p != linehead; p = (p == lineend) ? line : p + 1) {
			if (p->y + 1 > p->l)
				printf("\x1b[%u;%uH ", p->y - p->l, p->x);
			if (p->y - 1 < H) {
				printf("\x1b[38;2;%u;%u;%um\x1b[%u;%uH%c", p->r, p->g, p->b, p->y, p->x, randchar(rand()));
			} else if (p->y - p->l - 5 > H) {
				if (linetail == lineend) // loop over
					linetail = line;
				else
					++linetail;
				continue;
			}
			p->y += 1;
		}
		//printf("%ld\n", linehead - linetail);
		
		fflush(stdout);
		usleep(OPTTIME);
	}

	l_end:

	tcsetattr(0, TCSANOW, &tres);	
	#ifdef OPTBOLD
		printf("\x1b[22m\x1b[?25h\x1b[0m\x1b[%u;%uH\n\x1b[?1049l", H, 0);
	#else
		printf("\x1b[?25h\x1b[0m\x1b[%u;%uH\n\x1b[?1049l", H, 0);
	#endif

	return 0;
	
}
