/*
 * validator/validator.c
 *
 * Copyright (C) 2022 bzt (bztsrc@gitlab)
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
 * @brief a web based Model 3D validator
 * https://gitlab.com/bztsrc/model3d
 *
 */

#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <emscripten.h>

int verbose = 0;
char *outbuf = NULL;
void myprintf(char *fmt, ...);
void myerrprintf(void *f, char *fmt, ...);

#define M3D_IMPLEMENTATION
#define M3D_ASCII
#define M3D_LOG(x) do{if(verbose>1)myprintf("  %s\n",x);}while(0)
#define M3D_VERTEXTYPE
#define M3D_VERTEXMAX
#include "../m3d.h"
#define dumplog myprintf
#define dumperr myerrprintf
#include "../m3dconv/dump.h"

/**
 * Redirect output to a html element
 */
void myprintf(char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vsnprintf(outbuf, BUFSIZ, fmt, args);
    EM_ASM({ output($0); }, strlen(outbuf) );
}
void myerrprintf(void *f, char *fmt, ...)
{
    (void)f;
    va_list args;
    va_start(args, fmt);
    vsnprintf(outbuf, BUFSIZ, fmt, args);
    EM_ASM({ output($0); }, strlen(outbuf) );
}

/**
 * Function to run validator
 */
void m3d_validate(unsigned char *buff, unsigned int size, int level, int ver, char *out)
{
    m3d_t *m3d = NULL;

    outbuf = out;
    verbose = ver;

    if(level == 99) {
        if(verbose) myprintf("Parsing model\n");
        m3d = m3d_load(buff, NULL, NULL, NULL);
        if(verbose) myprintf("\n");
        if(m3d) {
            dump_cstruct(m3d);
            m3d_free(m3d);
        } else
            myprintf("ERROR: unable to parse as a Model 3D file\n");
    } else {
        dump_file(buff, size, level);
    }
    /* let the JS know that there's no more output, so that it can re-enable the enter button */
    myprintf("\003");
}
