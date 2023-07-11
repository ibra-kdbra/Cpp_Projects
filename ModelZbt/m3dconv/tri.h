/*
 * m3dconv/tri.h
 *
 * Copyright (C) 2020 bzt (bztsrc@gitlab)
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
 * @brief triangulate shapes and voxels in Model 3D files
 * https://gitlab.com/bztsrc/model3d
 *
 */

/**
 * Convert surfaces and other parameterized shapes into triangle mesh
 */
void tri_shapes(float n, m3d_t *m3d)
{
    if(n <= 0.0 || !m3d->numshape || !m3d->shape) return;
    /* TODO */
    fprintf(stderr,"m3dconv: shape triangulation is under development.\n");
}

/**
 * Convert voxel images into triangle mesh (opposite of voxelize_face() in voxel.h)
 */
void tri_voxels(float n, m3d_t *m3d)
{
    if(n <= 0.0 || !m3d->numvoxel || !m3d->voxel) return;
    /* unlike the post processing in m3d.h, this is not the fast, but the memory efficient version */
    /* TODO */
    fprintf(stderr,"m3dconv: voxel triangulation is under development.\n");
}
