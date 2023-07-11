/*
 * tests/test07.c
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
 * @brief Differenct define combinations
 */

#include <stdlib.h>
#include <stdio.h>

#define M3D_LOG(s) fprintf(stderr,"m3d: %s\n",(s))
#define M3D_ASCII
#define M3D_NOANIMATION
#define M3D_IMPLEMENTATION
#include <m3d.h>
#include "../m3dconv/dump.h"

m3d_t *model = NULL;

unsigned char *readfile(char *fn, unsigned int *size)
{
    FILE *f;
    unsigned char *data = NULL;

    f = fopen(fn, "rb");
    if(f) {
        fseek(f, 0L, SEEK_END);
        *size = (unsigned int)ftell(f);
        fseek(f, 0L, SEEK_SET);
        data = (unsigned char*)malloc(*size + 1);
        if(data) {
            fread(data, *size, 1, f);
            data[*size] = 0;
        }
        fclose(f);
    }
    return data;
}

int main(int argc, char **argv)
{
    unsigned int size = 0;
    unsigned char *data = NULL;

    if(argc < 2) return 1;

    data = readfile(argv[1], &size);
    if(data) {
        model = m3d_load(data, readfile, free, NULL);
        if(argc == 2) dump_cstruct(model);
        m3d_free(model);
        free(data);
        return 0;
    }
    return 1;
}
