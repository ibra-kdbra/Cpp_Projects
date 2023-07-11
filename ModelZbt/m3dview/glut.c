/*
 * m3dview/glut.c
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
 * @brief GLUT interface for the simple portable Model 3D viewer
 * https://gitlab.com/bztsrc/model3d
 *
 */

#include "viewer.h"
#include <glut.h>
#include <gl.h>

int px = 0, py = 0;

/* window resize event callback */
void glut_reshape(int w, int h)
{
    screenw = w;
    screenh = h;
    glViewport(0, 0, w, h);
}

/* render scene callback */
void glut_display(void)
{
    display(glutGet(GLUT_ELAPSED_TIME));
    glutSwapBuffers();
    glutPostRedisplay();
}

/* mouse button press event callback */
void glut_mouse(int button, int state, int x, int y)
{
    int s = state == GLUT_DOWN;
    if (button == GLUT_LEFT_BUTTON) mousebtn = s;
    if (s && button == GLUT_MIDDLE_BUTTON) zoomin();
    if (s && button == GLUT_RIGHT_BUTTON) zoomout();
    if(px && py) {
        mousex = x - px;
        mousey = y - py;
    }
    px = x; py = y;
    glutPostRedisplay();
}

/* mouse motion event callback */
void glut_motion(int x, int y)
{
    mousex = x - px;
    mousey = y - py;
    px = x; py = y;
    mousemove = 1;
    glutPostRedisplay();
}

/* normal key event callback */
void glut_keyboard(unsigned char key, __attribute__((unused)) int x, __attribute__((unused)) int y)
{
    switch (key) {
        case 27: case 'q': exit(1); break;
        case 9: actionid++; break;
        case '+': case '=': zoomin(); break;
        case '-': zoomout(); break;
        case ',': prevframe(); break;
        case '.': nextframe(); break;
        case ' ': continous(); break;
        case 'm': domesh ^= 1; break;
        case 's': doskel ^= 1; break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9': fpsdiv(key-'0'); break;
    }
    glutPostRedisplay();
}

/* special key event callback */
void glut_special(int key, __attribute__((unused)) int x, __attribute__((unused)) int y)
{
    switch (key) {
        case GLUT_KEY_F4: exit(1); break;
        case GLUT_KEY_UP: mousex = 0; mousey = -10; mousemove = 1; break;
        case GLUT_KEY_DOWN: mousex = 0; mousey = 10; mousemove = 1; break;
        case GLUT_KEY_LEFT: mousex = -10; mousey = 0; mousemove = 1; break;
        case GLUT_KEY_RIGHT: mousex = 10; mousey = 0; mousemove = 1;; break;
        case GLUT_KEY_PAGE_UP: actionid--; break;
        case GLUT_KEY_PAGE_DOWN: actionid++; break;
    }
    glutPostRedisplay();
}

/**
 * Set window title
 */
void set_title(char *title)
{
    glutSetWindowTitle(title);
}

/**
 * Main procedure. Set up and main loop
 */
int main(int argc, char **argv)
{
    load(argc, argv);
    glutInitWindowPosition(50, 50);
    glutInitWindowSize(screenw, screenh);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutCreateWindow(wintitle);
    screenw = glutGet(GLUT_WINDOW_WIDTH);
    screenh = glutGet(GLUT_WINDOW_HEIGHT);
    glutReshapeFunc(glut_reshape);
    glutDisplayFunc(glut_display);
    glutMouseFunc(glut_mouse);
    glutMotionFunc(glut_motion);
    glutKeyboardFunc(glut_keyboard);
    glutSpecialFunc(glut_special);
    setupgl();
    glutMainLoop();
    cleanup();
    return 0;
}
