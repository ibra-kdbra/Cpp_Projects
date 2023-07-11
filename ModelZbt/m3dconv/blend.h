/*
 * m3dconv/blend.h
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
 * @brief simple 3D model to M3D converter Blender importer
 * https://gitlab.com/bztsrc/model3d
 *
 */

unsigned char *readfile(char *fn, unsigned int *size);

/**
 * Load a model and convert it's structures into a Model 3D in-memory format
 */
m3d_t *blend_load(unsigned char *data, unsigned int size)
{
    char ptrsize, endian;
    m3d_t *m3d;

    m3d = (m3d_t*)malloc(sizeof(m3d_t));
    if(!m3d) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
    memset(m3d, 0, sizeof(m3d_t));
    m3d->flags = M3D_FLG_FREESTR;

    /* add default position and orientation, may be needed by bones in group statements */
    m3d->numvertex = 2;
    m3d->vertex = (m3dv_t*)malloc(m3d->numvertex * sizeof(m3dv_t));
    if(!m3d->vertex) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
    memset(m3d->vertex, 0, 2 * sizeof(m3dv_t));
    m3d->vertex[0].skinid = -1U;
    m3d->vertex[0].type = VT_WORLD;
    m3d->vertex[1].skinid = -2U;
    m3d->vertex[1].type = VT_QUATERN;

    ptrsize = data[7] == '-' ? 8 : 4;
    endian = data[9] == 'v' ? 0 : 1;

    (void)ptrsize; (void)endian; (void)size;
    return m3d;
}
