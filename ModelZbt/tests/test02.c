/*
 * tests/test02.c
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
 * @brief Model 3D loader. See also test01
 *
 * Use like this:
 *  measuring execution time:     time ./test02 model.m3d
 *  measuring memory consumption: valgrind ./test02 model.m3d
 */

#include <stdlib.h>
#include <stdio.h>

#define M3D_IMPLEMENTATION
#define M3D_CPPWRAPPER
#define M3D_ASCII
#include <m3d.h>

m3d_t *scene = NULL;

/* assimp loads external assets, so it is only fair if we provide a read file callback to M3D */
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
        scene = m3d_load(data, readfile, free, NULL);
        free(data);
        /* don't free the scene, we need valgrind memleak to tell how much memory it requires */
        return 0;
    }
    return 1;
}
