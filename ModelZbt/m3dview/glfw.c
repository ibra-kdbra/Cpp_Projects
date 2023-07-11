/*
 * m3dview/glfw.c
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
 * @brief GLFW interface for the simple portable Model 3D viewer
 * https://gitlab.com/bztsrc/model3d
 *
 */

#include "viewer.h"
#include <gl.h>
#include <glfw3.h>

GLFWwindow *window;
double px = 0, py = 0;
int running = 1;

/* the glfw error callback */
void glfw_error(int error, const char *msg)
{
    fprintf(stderr, "m3dview: glfw error %d: %s\n", error, msg);
}

/* process a keyboard event callback */
void glfw_key(__attribute__((unused)) GLFWwindow *window, int key, __attribute__((unused)) int scancode,
    int action, __attribute__((unused)) int mods)
{
    if(action != GLFW_PRESS) return;
    switch(key) {
        case GLFW_KEY_Q:
        case GLFW_KEY_ESCAPE: running = 0; glfwSetWindowShouldClose(window, GLFW_TRUE); break;
        case GLFW_KEY_UP: mousex = 0; mousey = -10; mousemove = 1; break;
        case GLFW_KEY_DOWN: mousex = 0; mousey = 10; mousemove = 1; break;
        case GLFW_KEY_LEFT: mousex = -10; mousey = 0; mousemove = 1; break;
        case GLFW_KEY_RIGHT: mousex = 10; mousey = 0; mousemove = 1; break;
        case GLFW_KEY_PAGE_UP: actionid--; break;
        case GLFW_KEY_TAB:
        case GLFW_KEY_PAGE_DOWN: actionid++; break;
        case GLFW_KEY_EQUAL: zoomin(); break;
        case GLFW_KEY_MINUS: zoomout(); break;
        case GLFW_KEY_COMMA: prevframe(); break;
        case GLFW_KEY_PERIOD: nextframe(); break;
        case GLFW_KEY_SPACE: continous(); break;
        case GLFW_KEY_M: domesh ^= 1; break;
        case GLFW_KEY_S: doskel ^= 1; break;
        case GLFW_KEY_0:
        case GLFW_KEY_1:
        case GLFW_KEY_2:
        case GLFW_KEY_3:
        case GLFW_KEY_4:
        case GLFW_KEY_5:
        case GLFW_KEY_6:
        case GLFW_KEY_7:
        case GLFW_KEY_8:
        case GLFW_KEY_9: fpsdiv(key-GLFW_KEY_0); break;
    }
}

/* mouse event callback */
void glfw_mouse(__attribute__((unused)) GLFWwindow *window, int button, int action, __attribute__((unused)) int mods) {
    int s = action == GLFW_PRESS;
    if (button == GLFW_MOUSE_BUTTON_LEFT) mousebtn = s;
    if (s && button == GLFW_MOUSE_BUTTON_MIDDLE) zoomin();
    if (s && button == GLFW_MOUSE_BUTTON_RIGHT) zoomout();
}

/* scrolling event callback */
void glfw_scroll(__attribute__((unused)) GLFWwindow *window, __attribute__((unused)) double xdelta, double ydelta) {
    if(ydelta < 0.0) zoomout();
    else zoomin();
}

/**
 * Set window title
 */
void set_title(char *title)
{
    glfwSetWindowTitle(window, title);
}

/**
 * Main procedure. Set up and main loop
 */
#ifdef __EMSCRIPTEN__
void loadbuf(unsigned char *data);
int m3d_viewer(unsigned char *data, int size)
#else
int main(int argc, char **argv)
#endif
{
    double mx, my;

#ifdef __EMSCRIPTEN__
    loadbuf(data);
#else
    load(argc, argv);
#endif
    if (!glfwInit() || !(window = glfwCreateWindow(screenw, screenh, wintitle, NULL, NULL)))
        error("unable to initialize glfw");
    glfwSetErrorCallback(glfw_error);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetKeyCallback(window, glfw_key);
    glfwSetMouseButtonCallback(window, glfw_mouse);
    glfwSetScrollCallback(window, glfw_scroll);
    setupgl();
    while(running) {
        /* handle window resize */
        glfwGetFramebufferSize(window, &screenw, &screenh);
        glViewport(0, 0, screenw, screenh);
        /* handle mouse movement */
        glfwGetCursorPos(window, &mx, &my);
        if(mx != px || my != py) {
            mousex = (int)(mx - px);
            mousey = (int)(my - py);
            px = mx; py = my;
            mousemove = mousebtn;
        }
        /* render scene */
        display((int)(glfwGetTime()*1000));
        glfwSwapBuffers(window);

        glfwPollEvents();
        if (glfwWindowShouldClose(window)) break;
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    cleanup();
    return 0;
}
