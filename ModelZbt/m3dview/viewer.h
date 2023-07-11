/*
 * m3dview/viewer.h
 *
 * Copyright (C) 2019 bzt (bztsrc@gitlab)
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * @brief simple portable Model 3D viewer
 * https://gitlab.com/bztsrc/model3d
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* provided by the interface drivers */
extern void set_title(char *title);      /* set window title */

/* variables used by the interface drivers */
extern int screenw, screenh, mousebtn, mousemove, mousex, mousey, doframe, domesh, doskel;
extern unsigned int actionid;
extern char *wintitle;

void load(int argc, char **argv);   /* load the model */
void setupgl(void);                 /* set up OpenGL */
void zoomin(void);                  /* zoom in */
void zoomout(void);                 /* zoom out */
void fpsdiv(int idx);               /* set fps divisor */
void nextframe(void);               /* jump to next frame */
void prevframe(void);               /* jump to previous frame */
void continous(void);               /* toggle continous playback */
void display(unsigned int msec);    /* render the (animated) model */
void error(char *msg);              /* display an error message and exit */
void cleanup(void);                 /* free resources */
