
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>

#include "config.h"

struct winsize _winsize;
#define W _winsize.ws_col
#define H _winsize.ws_row

void sigwinch(int _) {
	(void)_;
	ioctl(0, TIOCGWINSZ, &_winsize);
}

struct {
	int x;
	int y;
	int dir;
} pos;

void setcolor() {
	randcolor();
	printf("\x1b[38;2;%u;%u;%um", color[0], color[1], color[2]);
}

char _sgetcharwaiting;
char sgetchar() {
	ioctl(0, FIONREAD, &_sgetcharwaiting);
	if (_sgetcharwaiting == 0) return 0;
	return getchar();
}


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
	tcgetattr(0, &tres); // backup the original terminal state to restore later
	tcur.c_lflag &= ~(ICANON|ECHO|ISIG);
	tcsetattr(0, TCSANOW, &tcur);


	// Init screen
	pos.x = rand() % W;
	pos.y = rand() % H;
	setcolor();
	
	// Mainloop
	static unsigned char printc = 1;
	static int randn;
	static char c;
	while (1) {
		c = sgetchar();
		switch (c) {
			case 3: // ctrl-c
				goto l_end;
		}
		switch (pos.dir) {
			case 0: 
				pos.x += 1; 
				if (pos.x > W) {
					pos.x = 0;
					setcolor();
				}
				break;
			case 1: 
				pos.y += 1; 
				if (pos.y > H) {
					pos.y = 0;
					setcolor();
				}
				break;
			case 2: 
				pos.x -= 1; 
				if (pos.x < 1) {
					pos.x = W;
					setcolor();
				}
				break;
			case 3: 
				pos.y -= 1;
				if (pos.y < 1) {
					pos.y = H;
					setcolor();
				}
				break;
		}
		printf("\x1b[%d;%dH", pos.y, pos.x);
		randn = rand();
		if (randn % 100 > OPTCHANCE) {
			// 0 1 2 3 4 5
			// ┃ ━ ┏ ┓ ┗ ┛;
			if (randn > RAND_MAX / 2) {
				switch (pos.dir) {
					case 0: pos.dir = 1; printc = 3; break;
					case 1: pos.dir = 2; printc = 5; break;
					case 2: pos.dir = 3; printc = 4; break;
					case 3: pos.dir = 0; printc = 2; break;
				}
			} else {
				switch (pos.dir) { // 
					case 3: pos.dir = 2; printc = 3; break;
					case 2: pos.dir = 1; printc = 2; break;
					case 1: pos.dir = 0; printc = 4; break;
					case 0: pos.dir = 3; printc = 5; break;
				}
			}
			printf(chars[printc]);
			printc = pos.dir == 1 || pos.dir == 3 ? 0 : 1;
		} else {
			printf(chars[printc]);
		}
		fflush(stdout);
		usleep(OPTTIME);
	}

	l_end:

	tcsetattr(0, TCSANOW, &tres);
	#ifdef OPTBOLD
		printf("\x1b[22m\x1b[?25h\x1b[0m\x1b[%dB\n\x1b[?1049l", H - pos.y);
	#else
		printf("\x1b[?25h\x1b[0m\x1b[%dB\n\x1b[?1049l", H - pos.y);
	#endif
	
	return 0;
	
}
