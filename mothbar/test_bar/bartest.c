#include <unistd.h> 
#include <stdio.h> 
#include <string.h>
#include <stdlib.h>
#include <xcb/xcb.h>

#define FONT "fixed"

#define FOREGROUND 0xFF000000
#define BACKGROUND 0xFFae5353
//#define BACKGROUND 0xFFaaaaaa
#define HEIGHT 40

static void
checkr(xcb_void_cookie_t c, xcb_connection_t *dpy, char *msg)
{
	xcb_generic_error_t *e;
	
	e = xcb_request_check(dpy, c);
	if (e) {
		fprintf(stderr, "ERROR: %s \n", msg);
		xcb_disconnect(dpy);
		exit(-1);
    }
}

static xcb_gc_t 
getgc(xcb_connection_t *dpy, xcb_screen_t *screen, xcb_window_t barid, const char *name)
{
	xcb_font_t			fontid;
	xcb_gcontext_t		gcid;
	xcb_void_cookie_t	r;
	uint32_t 			mask, value[3]; 
	
	fontid = xcb_generate_id(dpy);
	r = xcb_open_font_checked(dpy, fontid, strlen(name), name);
	checkr(r, dpy, "can not open font");
	
	gcid = xcb_generate_id(dpy);
	mask = XCB_GC_FOREGROUND | XCB_GC_BACKGROUND | XCB_GC_FONT;
	value[0] = FOREGROUND;
	value[1] = BACKGROUND;
	value[2] = fontid;
	r = xcb_create_gc_checked(dpy, gcid, barid, mask, value);
	checkr(r, dpy, "can not create gc");
    
	r = xcb_close_font_checked(dpy, fontid);
    checkr(r, dpy, "can not close font");

	return gcid;
}

static void
makeprint(xcb_connection_t *dpy, xcb_screen_t *screen, xcb_window_t barid, int16_t x1, int16_t y1, const char *label)
{
	xcb_gcontext_t    gcid;
	xcb_void_cookie_t r;
	
	gcid = getgc(dpy, screen, barid, FONT);
	 
	r = xcb_image_text_8_checked(dpy, strlen(label), barid, gcid, x1, y1, label);
    checkr(r, dpy, "can not print text");
	 
	r = xcb_free_gc(dpy, gcid);
	checkr(r, dpy, "can not free gc");
}

int
main()
{
	xcb_connection_t 			*dpy; 
	const xcb_setup_t 			*setup;
	xcb_screen_iterator_t 		iterator;
	xcb_screen_t 				*screen;
	xcb_window_t 				barid;
	xcb_void_cookie_t 			r;
	xcb_get_geometry_cookie_t 	g_cookie;
	xcb_get_geometry_reply_t 	*g_reply;
	uint32_t 					mask, values[3];
	int rootwidth;
		
	dpy = xcb_connect(NULL, NULL);

	if (xcb_connection_has_error(dpy)) {
		printf("X connection failed\n");
		return 1;
	}
	
	setup = xcb_get_setup(dpy);
	iterator = xcb_setup_roots_iterator(setup);
	screen = iterator.data;

	g_cookie = xcb_get_geometry(dpy, screen->root);
	g_reply = xcb_get_geometry_reply(dpy, g_cookie, NULL);
	
	rootwidth = g_reply->width;
	free(g_reply);
	
	barid = xcb_generate_id(dpy);
	mask = XCB_CW_BACK_PIXEL | XCB_CW_OVERRIDE_REDIRECT ;
    values[0]= BACKGROUND;
    values[1] = 1; 
	r = xcb_create_window(dpy, XCB_COPY_FROM_PARENT, barid, screen->root, 
                           0, 0, rootwidth, HEIGHT, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT, 
                           screen->root_visual, mask, values );

	checkr(r, dpy, "can not create window");

	r = xcb_map_window_checked(dpy, barid);
	checkr(r, dpy, "can not map window");

	makeprint(dpy, screen, barid, 5, HEIGHT - 5, "This is a test for a bar" );
	xcb_map_window(dpy, barid);
	xcb_flush(dpy);
	pause(); 

	xcb_disconnect(dpy);
	return 0;
}
