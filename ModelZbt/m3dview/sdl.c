/*
 * m3dview/sdl.c
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
 * @brief SDL2 interface for the simple portable Model 3D viewer
 * https://gitlab.com/bztsrc/model3d
 *
 */

#include "viewer.h"
#include <gl.h>
#include <SDL.h>

SDL_Window *window;
int running = 1, px = 0, py = 0;

#ifdef PREVIEW
#include <SDL_image.h>
#include <SDL2_rotozoom.h>
#include <m3d.h>
extern float pitch, yaw;
extern m3d_t *model;

/**
 * Save the rendered frame into a preview PNG file
 */
void save_window(char *fn)
{
    unsigned int i, j, k = 0, *pix = (unsigned int*)malloc(screenw * 4);
    unsigned long int l = 0;
    SDL_Surface *image = SDL_CreateRGBSurface(SDL_SWSURFACE, screenw, screenh, 32, 0xFF, 0xFF00, 0xFF0000, 0xFF000000);
    SDL_Surface *scaled;
    if(!image) return;
    glReadBuffer(GL_FRONT);
    glReadPixels(0, 0, screenw, screenh, GL_RGBA, GL_UNSIGNED_BYTE, image->pixels);
    if(pix) {
        for(i = 0; i < (unsigned int)screenh / 2; i++) {
            memcpy(pix, (unsigned char*)(image->pixels) + i * image->pitch, screenw * 4);
            memcpy((unsigned char*)(image->pixels) + i * image->pitch, (unsigned char*)(image->pixels) +
                (screenh - i - 1) * image->pitch, screenw * 4);
            memcpy((unsigned char*)(image->pixels) + (screenh - i - 1) * image->pitch, pix, screenw * 4);
        }
        free(pix);
    }
    for(i = 0; i < (unsigned int)screenh * image->pitch/4; i++)
        if(((unsigned int*)(image->pixels))[i] == 0xFFFFFFFF) ((unsigned int*)(image->pixels))[i] = 0;
/*
    scaled = SDL_CreateRGBSurface(SDL_SWSURFACE, 320, 240, 32, 0xFF, 0xFF00, 0xFF0000, 0xFF000000);
    SDL_SetSurfaceBlendMode(scaled, SDL_BLENDMODE_BLEND);
    SDL_BlitScaled(image, NULL, scaled, NULL);
*/
    scaled = zoomSurface(image, 320.0/(double)screenw, 240.0/(double)screenh, 1);
    IMG_Init(IMG_INIT_PNG);
    IMG_SavePNG(scaled, fn);
    IMG_Quit();
    SDL_FreeSurface(image);
    SDL_FreeSurface(scaled);
    for(i = 0; i < model->numshape && k != 3; i++)
        for(j = 0; j < model->shape[i].numcmd && k != 3; j++)
            switch(model->shape[i].cmd[j].type) {
                 case m3dc_bezu: case m3dc_nurbsu: k |= 1; break;
                 case m3dc_bezn: case m3dc_nurbsn: k |= 2; break;
                 case m3dc_bezun: case m3dc_nurbsun: k = 3; break;
            }
    for(i = 0; i < model->numvoxel; i++)
        l += model->voxel[i].w * model->voxel[i].h * model->voxel[i].d;
    printf("%s\n%s, %s\n%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%ld\n", model->name, model->license, model->author, model->numface,
        model->numface && model->face[0].texcoord[0] != -1U ? 1 : 0,
        model->numface && model->face[0].normal[0] != -1U && !(model->flags & M3D_FLG_GENNORM) ? 1 : 0,
        model->numshape, k & 1 ? 1 : 0, k & 2 ? 1 : 0, model->numvertex, model->nummaterial, model->numtexture,
        model->numbone, model->numaction,model->numlabel,l);
}
#endif

/**
 * Set window title
 */
void set_title(char *title)
{
    SDL_SetWindowTitle(window, title);
}

/*
 * wrapper for Windows SDL support. This partialy came from SDL_windows_main.c
 */
#ifdef __WIN32__
#include <windows.h>

static void UnEscapeQuotes(char *arg)
{
    char *last = NULL;

    while (*arg) {
        if (*arg == '"' && (last != NULL && *last == '\\')) {
            char *c_curr = arg;
            char *c_last = last;

            while (*c_curr) {
                *c_last = *c_curr;
                c_last = c_curr;
                c_curr++;
            }
            *c_last = '\0';
        }
        last = arg;
        arg++;
    }
}

/* Parse a command line buffer into arguments */
static int ParseCommandLine(char *cmdline, char **argv)
{
    char *bufp;
    char *lastp = NULL;
    int argc, last_argc;

    argc = last_argc = 0;
    for (bufp = cmdline; *bufp;) {
        /* Skip leading whitespace */
        while (SDL_isspace(*bufp)) {
            ++bufp;
        }
        /* Skip over argument */
        if (*bufp == '"') {
            ++bufp;
            if (*bufp) {
                if (argv) {
                    argv[argc] = bufp;
                }
                ++argc;
            }
            /* Skip over word */
            lastp = bufp;
            while (*bufp && (*bufp != '"' || *lastp == '\\')) {
                lastp = bufp;
                ++bufp;
            }
        } else {
            if (*bufp) {
                if (argv) {
                    argv[argc] = bufp;
                }
                ++argc;
            }
            /* Skip over word */
            while (*bufp && !SDL_isspace(*bufp)) {
                ++bufp;
            }
        }
        if (*bufp) {
            if (argv) {
                *bufp = '\0';
            }
            ++bufp;
        }

        /* Strip out \ from \" sequences */
        if (argv && last_argc != argc) {
            UnEscapeQuotes(argv[last_argc]);
        }
        last_argc = argc;
    }
    if (argv) {
        argv[argc] = NULL;
    }
    return (argc);
}

int APIENTRY WinMain(__attribute__((unused)) HINSTANCE hInstance, __attribute__((unused)) HINSTANCE hPrevInstance,
    __attribute__((unused)) LPSTR lpCmdLine, __attribute__((unused)) int nCmdShow)
{
    OPENFILENAME  ofn;
    char *cmdline = GetCommandLine();
    int ret, argc = ParseCommandLine(cmdline, NULL);
    char **argv = SDL_stack_alloc(char*, argc+2);
    char fn[1024];
    ParseCommandLine(cmdline, argv);
    if(!argv[1]) {
        memset(&fn,0,sizeof(fn));
        memset(&ofn,0,sizeof(ofn));
        ofn.lStructSize     = sizeof(ofn);
        ofn.hwndOwner       = NULL;
        ofn.hInstance       = hInstance;
        ofn.lpstrFilter     = "Model Files (*.m3d, *.a3d)\0*.m3d;*.a3d\0All Files\0*\0\0";
        ofn.lpstrFile       = fn;
        ofn.nMaxFile        = sizeof(fn)-1;
        ofn.lpstrTitle      = "Please Select A File To Open";
        ofn.Flags           = OFN_FILEMUSTEXIST;
        if (GetOpenFileName(&ofn)) {
            argc++;
            argv[1] = fn;
        }
    }
    SDL_SetMainReady();
    ret = main(argc, argv);
    SDL_stack_free(argv);
    exit(ret);
    return ret;
}
#endif

/**
 * Main procedure. Set up and main loop
 */
int main(int argc, char **argv)
{
    SDL_Event e;
    SDL_GLContext ctx;

    load(argc, argv);
    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS)) error("unable to initialize SDL");
    SDL_GL_LoadLibrary(NULL);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetSwapInterval(1);
    if(!(window = SDL_CreateWindow(wintitle, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenw, screenh,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE)))
        error("unable to create SDL window");
    ctx = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, ctx);
#ifdef PREVIEW
    if(argc > 2) {
        pitch = -30;
        yaw = 30;
        setupgl();
        display(0);
        SDL_GL_SwapWindow(window);
        save_window(argv[2]);
        running = 0;
    } else
#endif
    setupgl();
    while(running) {
        /* hadle events */
        if(SDL_PollEvent(&e)) {
            switch(e.type) {
                case SDL_QUIT: running = 0; break;
                case SDL_KEYDOWN:
                    switch(e.key.keysym.sym) {
                        case SDLK_q:
                        case SDLK_ESCAPE: running = 0; break;
                        case SDLK_UP: mousex = 0; mousey = -10; mousemove = 1; break;
                        case SDLK_DOWN: mousex = 0; mousey = 10; mousemove = 1; break;
                        case SDLK_LEFT: mousex = -10; mousey = 0; mousemove = 1; break;
                        case SDLK_RIGHT: mousex = 10; mousey = 0; mousemove = 1;; break;
                        case SDLK_PAGEUP: actionid--; break;
                        case SDLK_TAB:
                        case SDLK_PAGEDOWN: actionid++; break;
                        case SDLK_COMMA: prevframe(); break;
                        case SDLK_PERIOD: nextframe(); break;
                        case SDLK_SPACE: continous(); break;
                        case SDLK_m: domesh ^= 1; break;
                        case SDLK_s: doskel ^= 1; break;
                        case SDLK_0:
                        case SDLK_1:
                        case SDLK_2:
                        case SDLK_3:
                        case SDLK_4:
                        case SDLK_5:
                        case SDLK_6:
                        case SDLK_7:
                        case SDLK_8:
                        case SDLK_9: fpsdiv(e.key.keysym.sym-SDLK_0); break;
                    }
                break;
                case SDL_WINDOWEVENT:
                    switch(e.window.event) {
                        case SDL_WINDOWEVENT_CLOSE: running = 0; break;
                        case SDL_WINDOWEVENT_RESIZED:
                        case SDL_WINDOWEVENT_SIZE_CHANGED:
                            screenw = e.window.data1;
                            screenh = e.window.data2;
                            glViewport(0, 0, screenw, screenh);
                        break;
                    }
                break;
                case SDL_MOUSEMOTION:
                    mousex = e.motion.x - px;
                    mousey = e.motion.y - py;
                    px = e.motion.x; py = e.motion.y;
                    mousemove = mousebtn;
                break;
                case SDL_MOUSEBUTTONDOWN:
                    if(e.button.button == 1) mousebtn = 1;
                    if(e.button.button == 2) zoomout();
                    if(e.button.button == 3) zoomin();
                break;
                case SDL_MOUSEBUTTONUP:
                    mousebtn = 0;
                break;
                case SDL_MOUSEWHEEL:
                    if(e.wheel.y < 0) zoomout();
                    else zoomin();
                break;
            }
        }
        /* render scene */
        display(SDL_GetTicks());
        SDL_GL_SwapWindow(window);
    }
    SDL_GL_DeleteContext(ctx);
    SDL_DestroyWindow(window);
    SDL_VideoQuit();
    SDL_GL_UnloadLibrary();
    SDL_Quit();
    cleanup();
    return 0;
}
