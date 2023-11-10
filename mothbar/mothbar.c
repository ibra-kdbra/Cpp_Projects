
#include <fcntl.h>
#include <spawn.h>
#include <stdio.h> 
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <xcb/xcb.h>

#include "declarations.h"

typedef struct {
	char *font, *fg, *bg;
	int height;
} mbar_xresources_t;

mbar_xresources_t mbxres;
const char *parsed_xres[4] = {"mbar.bg", "mbar.fg", "mbar.font", "mbar.height"};
xcb_connection_t *conn;
xcb_screen_t *screen;
xcb_window_t barid;
xcb_gc_t gcid;
int width;
char *fifofile;
char _desk[MAXFIFO];
char _vol[MAXFIFO];
char _clock[MAXFIFO];
char _separator[MAXFIFO] = "     ";
char eraser[MAXFIFO];

void
makespawns() 
{
	char *cmd, *argv[2];;
	
	argv[1] = NULL;	
	cmd = "/usr/local/bin/m_desk";
	argv[0] = cmd;
	posix_spawn(NULL, cmd, NULL, NULL, argv, NULL);
	
	cmd = "/usr/local/bin/m_clock";
	argv[0] = cmd;
	posix_spawn(NULL, cmd, NULL, NULL, argv, NULL);
}

char *
getresource(const char *resource) {
	xcb_get_property_cookie_t c;
	xcb_get_property_reply_t *c_reply;
	xcb_intern_atom_cookie_t a;
	xcb_intern_atom_reply_t *a_reply;
	char *p_value, *token, *value, *end, *out;

	c = xcb_get_property(conn, 0, screen->root, XCB_ATOM_RESOURCE_MANAGER, XCB_ATOM_ANY, 0, MAXLENGTH);
	c_reply = xcb_get_property_reply(conn, c, NULL);
	if (c_reply) {
		p_value = xcb_get_property_value(c_reply);
		if (p_value) {	
			a = xcb_intern_atom(conn, 1, strlen(resource), resource);
			a_reply = xcb_intern_atom_reply(conn, a, NULL);

			if (a_reply) {
				token = strtok(p_value, "\n");
				while (token != NULL) {
					if (strstr(token, resource)) {
						value = strchr(token, ':');
						if (value) {
							value++;
							out = value;
							while ((*out == ' ') || (*out == '\t'))
								++out;
							end = strchr(out, '\n');
							if (end) *end = '\0';

							xcb_change_property(conn, XCB_PROP_MODE_REPLACE, screen->root, a_reply->atom, XCB_ATOM_STRING, 8, strlen(out), out);
							free(a_reply);
							free(c_reply);

							return strdup(out);
						}
					}
					token = strtok(NULL, "\n");
				} 
			} 
		}
		free(c_reply);
		free(a_reply);
	}
	return NULL;
}

void
readXresources() 
{
	mbxres = (mbar_xresources_t){ 
		.bg = BACKGROUND,
		.fg = FOREGROUND,
		.font = FONT,
		.height = HEIGHT
	};

	if (getresource(parsed_xres[0]) != NULL) mbxres.bg = strdup(getresource(parsed_xres[0]));
	if (getresource(parsed_xres[1]) != NULL) mbxres.fg = strdup(getresource(parsed_xres[1]));
	if (getresource(parsed_xres[2]) != NULL) mbxres.font = strdup(getresource(parsed_xres[2]));
	if (getresource(parsed_xres[3]) != NULL) mbxres.height = strtol(getresource(parsed_xres[3]), NULL, 10);
}

void
seteraser() 
{
	int i;
	
	for(i=0; i<MAXFIFO; i++) {
		strcat(eraser, " ");
	}
	strcat(eraser, "\0");
}

void
checkr(xcb_void_cookie_t c, char *msg)
{
	xcb_generic_error_t *e;
	
	e = xcb_request_check(conn, c);
	if (e) {
		fprintf(stderr, "ERROR: %s \n", msg);
		xcb_disconnect(conn);
		exit(-1);
    }
}

void
makeconnection() 
{
	conn = xcb_connect(NULL, NULL);
	if (xcb_connection_has_error(conn)) {
		printf("X connection failed\n");
		exit(-1);
	}
}

void
getscreendata() 
{
	const xcb_setup_t *setup;
	xcb_screen_iterator_t iterator;
	
	setup = xcb_get_setup(conn);
	iterator = xcb_setup_roots_iterator(setup);
	screen = iterator.data;
}

void
getrootwidth() 
{
	xcb_get_geometry_cookie_t c;
	xcb_get_geometry_reply_t *r;
	
	c = xcb_get_geometry(conn, screen->root);
	r = xcb_get_geometry_reply(conn, c, NULL);
	width = r->width;
	free(r);
}

void
makebar() 
{
	xcb_void_cookie_t r;
	uint32_t mask, values[2];
		
	barid = xcb_generate_id(conn);	
	mask = XCB_CW_BACK_PIXEL | XCB_CW_OVERRIDE_REDIRECT ;
	values[0]= strtoul(mbxres.bg, NULL, 16);
	values[1] = 1; 
	
	r = xcb_create_window(conn, XCB_COPY_FROM_PARENT, barid, screen->root, 
                           0, 0, width, mbxres.height, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT, 
                           screen->root_visual, mask, values);
	checkr(r, "can not create window");

	r = xcb_map_window_checked(conn, barid);
	checkr(r, "can not map window");
}

void
makefifo() 
{
	fifofile = FIFOFILE;
	mkfifo(fifofile, 0666);
}

void
makegc()
{
	xcb_font_t fontid;
	xcb_void_cookie_t r;
	uint32_t mask, value[3]; 
	
	fontid = xcb_generate_id(conn);
	r = xcb_open_font_checked(conn, fontid, strlen(mbxres.font), mbxres.font);
	checkr(r, "can not open font");

	gcid = xcb_generate_id(conn);
	mask = XCB_GC_FOREGROUND | XCB_GC_BACKGROUND | XCB_GC_FONT;
	value[0] = strtoul(mbxres.fg, NULL, 16);
	value[1] = strtoul(mbxres.bg, NULL, 16);
	value[2] = fontid;
	r = xcb_create_gc_checked(conn, gcid, barid, mask, value);
	checkr(r, "can not create gc");
    
	r = xcb_close_font_checked(conn, fontid);
	checkr(r, "can not close font");
}

void
makewrite(const char *text)
{
	xcb_void_cookie_t r;
		
	r = xcb_image_text_8_checked(conn, strlen(text), barid, gcid, FONTX, FONTY, text);
	checkr(r, "can not print text");
	 
	xcb_map_window(conn, barid);
	xcb_flush(conn);
}

void
checkfifostring(char *fifostring)
{
	char *text;
	char aux[MAXFIFO];
		
	text = fifostring; 
	if (!strchr(text, '@') || !strchr(text, '\n'))
		return; // ignore fifo string without @ and without \n
	
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
	
	strcpy(aux,"\0");
	strcat(aux,_desk);
	strcat(aux,_separator);
	strcat(aux,_vol);
	strcat(aux,_clock);
	
	makewrite(eraser);	
	makewrite(aux);
}

int
main()
{
	int fd;
	char fifostring[MAXFIFO];
		
	makeconnection();
	getscreendata();
	readXresources();
	getrootwidth();
	makebar() ;
	makefifo();
	makegc();
	seteraser();
	makespawns();

	while (1) {
		fd = open(fifofile,O_RDONLY);
		read(fd, fifostring, MAXFIFO);
		close(fd);
		if (!strncmp(fifostring, "exit", 4)) 
			break;
		checkfifostring(fifostring);
	}
	xcb_disconnect(conn);
	return 0;
}
