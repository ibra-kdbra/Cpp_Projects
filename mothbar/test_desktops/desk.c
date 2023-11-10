#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <xcb/xcb.h>

#include "declarations.h"

xcb_connection_t *conn;
xcb_screen_t *screen;
xcb_atom_t lid;
xcb_atom_t did;
xcb_atom_t cid; 
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

xcb_atom_t 
get_atom_id(char *name)
{
	xcb_atom_t a;
	xcb_intern_atom_cookie_t c;
	xcb_intern_atom_reply_t	*r;

	c = xcb_intern_atom(conn, 0, strlen(name), name);
	r = xcb_intern_atom_reply(conn, c, NULL);
	a = r->atom;
	free(r);
	return a;
}

int 
get_current_desk() 
{
	xcb_get_property_cookie_t c;
	xcb_get_property_reply_t *r;
	int *v, n;
	
	c = xcb_get_property(conn, 0, screen->root, cid, XCB_GET_PROPERTY_TYPE_ANY, 0, MAXLENGTH);
	r = xcb_get_property_reply(conn, c, NULL);
	v = xcb_get_property_value(r);
	n = v[0];
	free(r);
	free(v);
	return n;
}

void 
checkstate()
{
	xcb_get_property_cookie_t c;
	xcb_get_property_reply_t *r;
	xcb_window_t *win;
	int i, l, current, desk, *value;
	int flag[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	char number, newline[30];
	
	current = get_current_desk();
	flag[current] = 1;
	
	c = xcb_get_property(conn, 0, screen->root, lid, XCB_GET_PROPERTY_TYPE_ANY, 0, 100);
	r = xcb_get_property_reply(conn, c, NULL);
	l = xcb_get_property_value_length(r);
	win = xcb_get_property_value(r);
	free(r);
	
	for(i=0; i<l/4; i++) {
		c = xcb_get_property(conn, 0, win[i], did, XCB_GET_PROPERTY_TYPE_ANY, 0, 1000);
		r = xcb_get_property_reply(conn, c, NULL);
		value = xcb_get_property_value(r);
		desk = value[0];
// TODO fork flag checking with/without desktop 0
//		if(desk == 0)
//			flag[0] = 1;
		if(desk == 1) 
			flag[1] = 1; 
		else if(desk == 2) 
			flag[2] = 1; 
		else if(desk == 3) 
			flag[3] = 1; 
		else if(desk == 4) 
			flag[4] = 1; 
		else if(desk == 5) 
			flag[5] = 1; 
		else if(desk == 6) 
			flag[6] = 1; 
		else if(desk == 7) 
			flag[7] = 1; 
		else if(desk == 8) 
			flag[8] = 1; 
		else if(desk == 9) 
			flag[9] = 1; 
		free(value);
		free(r);
	}
	free(win);
	strcpy(newline, "\0");
	strcat(newline, "DESK@");
	for(i=0; i<10; i++) {
		if(flag[i]) {
			if(i == current) {
				number = i + '0';
				strcat(newline, "[");
				strcat(newline, &number);
				strcat(newline, "]"); 
			} else {
				number = i + '0';
				strcat(newline, " ");
				strcat(newline, &number);
				strcat(newline, " "); 
			}
		}
	}
	strcat(newline, "\n"); 
	if (strncmp(newline,line,strlen(newline))) {
		strcpy(line,newline);
		writefifo();
		printf("%s", line);
	}
}

int 
main() 
{
	pid_t pid;
	uint32_t value[1];
	xcb_generic_event_t *event;
	
	pid = getppid();
	conn = xcb_connect(NULL, NULL);
	screen = xcb_setup_roots_iterator(xcb_get_setup(conn)).data;
	lid = get_atom_id("_NET_CLIENT_LIST");
	did = get_atom_id("_NET_WM_DESKTOP");
	cid = get_atom_id("_NET_CURRENT_DESKTOP");
	value[0] = XCB_EVENT_MASK_PROPERTY_CHANGE;
	xcb_change_window_attributes(conn, screen->root, XCB_CW_EVENT_MASK, value);
	strcpy(line,"\0");
	xcb_flush(conn);
	checkstate();
	while ((event = xcb_wait_for_event(conn))) {
		if (kill(pid, 0))
			break;
		checkstate();
		free(event);
	}
	xcb_disconnect(conn);
	free(event);
	free(screen);
	free(conn);
	exit(0);
}
