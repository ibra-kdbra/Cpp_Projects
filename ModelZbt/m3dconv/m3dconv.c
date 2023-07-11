/*
 * m3dconv/m3dconv.c
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
 * @brief simple 3D model to M3D converter
 * https://gitlab.com/bztsrc/model3d
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <assimp/cimport.h>
#ifdef PROFILING
#include <sys/time.h>
#endif

/* command line parameters */
float arg_scale = 0.0, arg_framedelay = 1.0, arg_tri = 0.0;
char *arg_name=NULL, *arg_license=NULL, *arg_author=NULL, *arg_comment=NULL, *errstr = NULL, arg_rot[64];
int zip = 1, ascii = 0, doinline = 0, storeinline = 0, doextract = 0, doworld = 1, matlib = 0, quality = -1, dump = 0;
int right = 0, norm = 1, domesh = 0, withoutnorm = 0, withoutuv = 0, verbose = 0, flip = 0, notourimp = 0, arg_vox = 0;
int forcevoxpal = 0;

#define M3D_IMPLEMENTATION
#define M3D_EXPORTER
#define M3D_ASCII
#define M3D_LOG(x) do{if(verbose>1)printf("  %s\n",x);}while(0)
#define M3D_VERTEXTYPE
#define M3D_VERTEXMAX
#define M3D_NOVOXELS
#include <m3d.h>

/* vertex types */
#define VT_WORLD    0   /* vertex in world space */
#define VT_NORMAL   1   /* normal vector coordinate */
#define VT_RELATIVE 2   /* vertex relative to bone parent */
#define VT_QUATERN  3   /* vertex actually encodes a quaternion */

/* variables to the material library creator */
uint32_t nummaterial = 0;
m3dm_t *material = NULL;

uint32_t numinlined = 0;
m3di_t *inlined = NULL;

/* specific voxel palette */
m3d_t *voxpal = NULL;

/*** matrix helpers ***/
/* print an assimp matrix */
void _assimp_prt(struct aiMatrix4x4 *m)
{
    printf("%g %g %g %g\n", m->a1,m->a2,m->a3,m->a4);
    printf("%g %g %g %g\n", m->b1,m->b2,m->b3,m->b4);
    printf("%g %g %g %g\n", m->c1,m->c2,m->c3,m->c4);
    printf("%g %g %g %g\n\n", m->d1,m->d2,m->d3,m->d4);
}
/* fix minus zeros in matrix */
void _assimp_fixmat(struct aiMatrix4x4 *m)
{
    if(m->a1 > -M3D_EPSILON && m->a1 < M3D_EPSILON) m->a1 = 0.0;
    if(m->a2 > -M3D_EPSILON && m->a2 < M3D_EPSILON) m->a2 = 0.0;
    if(m->a3 > -M3D_EPSILON && m->a3 < M3D_EPSILON) m->a3 = 0.0;
    if(m->a4 > -M3D_EPSILON && m->a4 < M3D_EPSILON) m->a4 = 0.0;
    if(m->b1 > -M3D_EPSILON && m->b1 < M3D_EPSILON) m->b1 = 0.0;
    if(m->b2 > -M3D_EPSILON && m->b2 < M3D_EPSILON) m->b2 = 0.0;
    if(m->b3 > -M3D_EPSILON && m->b3 < M3D_EPSILON) m->b3 = 0.0;
    if(m->b4 > -M3D_EPSILON && m->b4 < M3D_EPSILON) m->b4 = 0.0;
    if(m->c1 > -M3D_EPSILON && m->c1 < M3D_EPSILON) m->c1 = 0.0;
    if(m->c2 > -M3D_EPSILON && m->c2 < M3D_EPSILON) m->c2 = 0.0;
    if(m->c3 > -M3D_EPSILON && m->c3 < M3D_EPSILON) m->c3 = 0.0;
    if(m->c4 > -M3D_EPSILON && m->c4 < M3D_EPSILON) m->c4 = 0.0;
    if(m->d1 > -M3D_EPSILON && m->d1 < M3D_EPSILON) m->d1 = 0.0;
    if(m->d2 > -M3D_EPSILON && m->d2 < M3D_EPSILON) m->d2 = 0.0;
    if(m->d3 > -M3D_EPSILON && m->d3 < M3D_EPSILON) m->d3 = 0.0;
    if(m->d4 > -M3D_EPSILON && m->d4 < M3D_EPSILON) m->d4 = 0.0;
}
/* those lazy assimp devs forgot to expose aiInverseMatrix4() in C API */
void _assimp_inverse(struct aiMatrix4x4 *m)
{
    struct aiMatrix4x4 ret;
    float det =
          m->a1*m->b2*m->c3*m->d4 - m->a1*m->b2*m->c4*m->d3 + m->a1*m->b3*m->c4*m->d2 - m->a1*m->b3*m->c2*m->d4
        + m->a1*m->b4*m->c2*m->d3 - m->a1*m->b4*m->c3*m->d2 - m->a2*m->b3*m->c4*m->d1 + m->a2*m->b3*m->c1*m->d4
        - m->a2*m->b4*m->c1*m->d3 + m->a2*m->b4*m->c3*m->d1 - m->a2*m->b1*m->c3*m->d4 + m->a2*m->b1*m->c4*m->d3
        + m->a3*m->b4*m->c1*m->d2 - m->a3*m->b4*m->c2*m->d1 + m->a3*m->b1*m->c2*m->d4 - m->a3*m->b1*m->c4*m->d2
        + m->a3*m->b2*m->c4*m->d1 - m->a3*m->b2*m->c1*m->d4 - m->a4*m->b1*m->c2*m->d3 + m->a4*m->b1*m->c3*m->d2
        - m->a4*m->b2*m->c3*m->d1 + m->a4*m->b2*m->c1*m->d3 - m->a4*m->b3*m->c1*m->d2 + m->a4*m->b3*m->c2*m->d1;
    if(det == 0.0 || det == -0.0) det = 1.0; else det = 1.0 / det;
    ret.a1 = det *(m->b2*(m->c3*m->d4 - m->c4*m->d3) + m->b3*(m->c4*m->d2 - m->c2*m->d4) + m->b4*(m->c2*m->d3 - m->c3*m->d2));
    ret.a2 = -det*(m->a2*(m->c3*m->d4 - m->c4*m->d3) + m->a3*(m->c4*m->d2 - m->c2*m->d4) + m->a4*(m->c2*m->d3 - m->c3*m->d2));
    ret.a3 = det *(m->a2*(m->b3*m->d4 - m->b4*m->d3) + m->a3*(m->b4*m->d2 - m->b2*m->d4) + m->a4*(m->b2*m->d3 - m->b3*m->d2));
    ret.a4 = -det*(m->a2*(m->b3*m->c4 - m->b4*m->c3) + m->a3*(m->b4*m->c2 - m->b2*m->c4) + m->a4*(m->b2*m->c3 - m->b3*m->c2));
    ret.b1 = -det*(m->b1*(m->c3*m->d4 - m->c4*m->d3) + m->b3*(m->c4*m->d1 - m->c1*m->d4) + m->b4*(m->c1*m->d3 - m->c3*m->d1));
    ret.b2 = det *(m->a1*(m->c3*m->d4 - m->c4*m->d3) + m->a3*(m->c4*m->d1 - m->c1*m->d4) + m->a4*(m->c1*m->d3 - m->c3*m->d1));
    ret.b3 = -det*(m->a1*(m->b3*m->d4 - m->b4*m->d3) + m->a3*(m->b4*m->d1 - m->b1*m->d4) + m->a4*(m->b1*m->d3 - m->b3*m->d1));
    ret.b4 = det *(m->a1*(m->b3*m->c4 - m->b4*m->c3) + m->a3*(m->b4*m->c1 - m->b1*m->c4) + m->a4*(m->b1*m->c3 - m->b3*m->c1));
    ret.c1 = det *(m->b1*(m->c2*m->d4 - m->c4*m->d2) + m->b2*(m->c4*m->d1 - m->c1*m->d4) + m->b4*(m->c1*m->d2 - m->c2*m->d1));
    ret.c2 = -det*(m->a1*(m->c2*m->d4 - m->c4*m->d2) + m->a2*(m->c4*m->d1 - m->c1*m->d4) + m->a4*(m->c1*m->d2 - m->c2*m->d1));
    ret.c3 = det *(m->a1*(m->b2*m->d4 - m->b4*m->d2) + m->a2*(m->b4*m->d1 - m->b1*m->d4) + m->a4*(m->b1*m->d2 - m->b2*m->d1));
    ret.c4 = -det*(m->a1*(m->b2*m->c4 - m->b4*m->c2) + m->a2*(m->b4*m->c1 - m->b1*m->c4) + m->a4*(m->b1*m->c2 - m->b2*m->c1));
    ret.d1 = -det*(m->b1*(m->c2*m->d3 - m->c3*m->d2) + m->b2*(m->c3*m->d1 - m->c1*m->d3) + m->b3*(m->c1*m->d2 - m->c2*m->d1));
    ret.d2 = det *(m->a1*(m->c2*m->d3 - m->c3*m->d2) + m->a2*(m->c3*m->d1 - m->c1*m->d3) + m->a3*(m->c1*m->d2 - m->c2*m->d1));
    ret.d3 = -det*(m->a1*(m->b2*m->d3 - m->b3*m->d2) + m->a2*(m->b3*m->d1 - m->b1*m->d3) + m->a3*(m->b1*m->d2 - m->b2*m->d1));
    ret.d4 = det *(m->a1*(m->b2*m->c3 - m->b3*m->c2) + m->a2*(m->b3*m->c1 - m->b1*m->c3) + m->a3*(m->b1*m->c2 - m->b2*m->c1));
    memcpy(m, &ret, sizeof(struct aiMatrix4x4));
}

/* compose a transformation matrix */
void _assimp_composematrix(struct aiMatrix4x4 *m, struct aiVector3D *p, struct aiQuaternion *q, struct aiVector3D *s)
{
    /* quat to rotation matrix */
    if(q->x == 0.0 && q->y == 0.0 && q->z >= 0.7071065 && q->z <= 0.7071075 && q->w == 0.0) {
        m->a2 = m->a3 = m->b1 = m->b3 = m->c1 = m->c2 = 0.0;
        m->a1 = m->b2 = m->c3 = -1.0;
    } else {
        m->a1 = 1 - 2 * (q->y * q->y + q->z * q->z); if(m->a1 > -M3D_EPSILON && m->a1 < M3D_EPSILON) m->a1 = 0.0;
        m->a2 = 2 * (q->x * q->y - q->z * q->w);     if(m->a2 > -M3D_EPSILON && m->a2 < M3D_EPSILON) m->a2 = 0.0;
        m->a3 = 2 * (q->x * q->z + q->y * q->w);     if(m->a3 > -M3D_EPSILON && m->a3 < M3D_EPSILON) m->a3 = 0.0;
        m->b1 = 2 * (q->x * q->y + q->z * q->w);     if(m->b1 > -M3D_EPSILON && m->b1 < M3D_EPSILON) m->b1 = 0.0;
        m->b2 = 1 - 2 * (q->x * q->x + q->z * q->z); if(m->b2 > -M3D_EPSILON && m->b2 < M3D_EPSILON) m->b2 = 0.0;
        m->b3 = 2 * (q->y * q->z - q->x * q->w);     if(m->b3 > -M3D_EPSILON && m->b3 < M3D_EPSILON) m->b3 = 0.0;
        m->c1 = 2 * (q->x * q->z - q->y * q->w);     if(m->c1 > -M3D_EPSILON && m->c1 < M3D_EPSILON) m->c1 = 0.0;
        m->c2 = 2 * (q->y * q->z + q->x * q->w);     if(m->c2 > -M3D_EPSILON && m->c2 < M3D_EPSILON) m->c2 = 0.0;
        m->c3 = 1 - 2 * (q->x * q->x + q->y * q->y); if(m->c3 > -M3D_EPSILON && m->c3 < M3D_EPSILON) m->c3 = 0.0;
    }

    /* scale matrix */
    if(s) {
        m->a1 *= s->x; m->a2 *= s->x; m->a3 *= s->x;
        m->b1 *= s->y; m->b2 *= s->y; m->b3 *= s->y;
        m->c1 *= s->z; m->c2 *= s->z; m->c3 *= s->z;
    }

    /* set translation */
    m->a4 = p->x; m->b4 = p->y; m->c4 = p->z;

    m->d1 = 0; m->d2 = 0; m->d3 = 0; m->d4 = 1;
}
/* get rotation matrix */
void _assimp_extract3x3(struct aiMatrix3x3 *m3, struct aiMatrix4x4 *m4)
{
    m3->a1 = m4->a1; m3->a2 = m4->a2; m3->a3 = m4->a3;
    m3->b1 = m4->b1; m3->b2 = m4->b2; m3->b3 = m4->b3;
    m3->c1 = m4->c1; m3->c2 = m4->c2; m3->c3 = m4->c3;
}
/* add a vertex to list */
m3dv_t *_assimp_addvrtx(m3dv_t *vrtx, uint32_t *numvrtx, m3dv_t *v, uint32_t *idx)
{
    if(v->x == (M3D_FLOAT)-0.0) v->x = (M3D_FLOAT)0.0;
    if(v->y == (M3D_FLOAT)-0.0) v->y = (M3D_FLOAT)0.0;
    if(v->z == (M3D_FLOAT)-0.0) v->z = (M3D_FLOAT)0.0;
    if(v->w == (M3D_FLOAT)-0.0) v->w = (M3D_FLOAT)0.0;
    vrtx = (m3dv_t*)M3D_REALLOC(vrtx, ((*numvrtx) + 1) * sizeof(m3dv_t));
    memcpy(&vrtx[*numvrtx], v, sizeof(m3dv_t));
    *idx = *numvrtx;
    (*numvrtx)++;
    return vrtx;
}
/* add a space (position + orientation + scaling) to vertex list */
static m3dv_t *_assimp_addspace(m3dv_t *vrtx, uint32_t *numvrtx, struct aiMatrix4x4 *m, uint32_t type, uint32_t *idx)
{
    struct aiVector3D p, s;
    struct aiQuaternion q;

    _assimp_fixmat(m);
    aiDecomposeMatrix(m, &s, &q, &p);
    if(s.x == (M3D_FLOAT)-0.0) s.x = (M3D_FLOAT)0.0;
    if(s.y == (M3D_FLOAT)-0.0) s.y = (M3D_FLOAT)0.0;
    if(s.z == (M3D_FLOAT)-0.0) s.z = (M3D_FLOAT)0.0;
    if(p.x == (M3D_FLOAT)-0.0) p.x = (M3D_FLOAT)0.0;
    if(p.y == (M3D_FLOAT)-0.0) p.y = (M3D_FLOAT)0.0;
    if(p.z == (M3D_FLOAT)-0.0) p.z = (M3D_FLOAT)0.0;
    if(q.x == (M3D_FLOAT)-0.0) q.x = (M3D_FLOAT)0.0;
    if(q.y == (M3D_FLOAT)-0.0) q.y = (M3D_FLOAT)0.0;
    if(q.z == (M3D_FLOAT)-0.0) q.z = (M3D_FLOAT)0.0;
    if(q.w == (M3D_FLOAT)-0.0) q.w = (M3D_FLOAT)0.0;

    vrtx = (m3dv_t*)M3D_REALLOC(vrtx, ((*numvrtx) + 3) * sizeof(m3dv_t));
    memset(&vrtx[*numvrtx], 0, 3 * sizeof(m3dv_t));
    *idx = *numvrtx;
    vrtx[*numvrtx].x = p.x;
    vrtx[*numvrtx].y = p.y;
    vrtx[*numvrtx].z = p.z;
    vrtx[*numvrtx].w = (M3D_FLOAT)1.0;
    vrtx[*numvrtx].skinid = (M3D_INDEX)-1U;
    vrtx[*numvrtx].type = type;
    (*numvrtx)++;
    vrtx[*numvrtx].x = q.x;
    vrtx[*numvrtx].y = q.y;
    vrtx[*numvrtx].z = q.z;
    vrtx[*numvrtx].w = q.w;
    vrtx[*numvrtx].skinid = (M3D_INDEX)-2U;
    vrtx[*numvrtx].type = VT_QUATERN;
    (*numvrtx)++;
    vrtx[*numvrtx].x = s.x;
    vrtx[*numvrtx].y = s.y;
    vrtx[*numvrtx].z = s.z;
    vrtx[*numvrtx].w = (M3D_FLOAT)1.0;
    vrtx[*numvrtx].skinid = (M3D_INDEX)-1U;
    vrtx[*numvrtx].type = VT_WORLD;
    (*numvrtx)++;
    return vrtx;
}

/*** other useful helpers ***/

/**
 * Convert euler radians into a quaternion
 */
void _m3d_euler_to_quat(M3D_FLOAT x, M3D_FLOAT y, M3D_FLOAT z, m3dv_t *q)
{
    M3D_FLOAT sr = (M3D_FLOAT)sin(x*0.5), cr = (M3D_FLOAT)cos(x*0.5);
    M3D_FLOAT sp = (M3D_FLOAT)sin(y*0.5), cp = (M3D_FLOAT)cos(y*0.5);
    M3D_FLOAT sy = (M3D_FLOAT)sin(z*0.5), cy = (M3D_FLOAT)cos(z*0.5);

    q->x = sr * cp * cy - cr * sp * sy;
    q->y = cr * sp * cy + sr * cp * sy;
    q->z = cr * cp * sy - sr * sp * cy;
    q->w = cr * cp * cy + sr * sp * sy;
}

/**
 * Add to animation frame
 */
void _m3d_addframe(m3da_t *action, uint32_t t, unsigned int bone, unsigned int pos, unsigned int ori)
{
    unsigned int i, j;

    if(t > action->durationmsec) action->durationmsec = t;

    /* find frame in action */
    for(i = 0; i < action->numframe && action->frame[i].msec < t; i++);
    if(i >= action->numframe || action->frame[i].msec > t) {
        action->frame = (m3dfr_t*)realloc(action->frame, (++action->numframe) * sizeof(m3dfr_t));
        if(!action->frame) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
        if(action->numframe - 1 > i)
            memmove(&action->frame[i + 1], &action->frame[i], (action->numframe - i - 1) * sizeof(m3dfr_t));
        memset(&action->frame[i], 0, sizeof(m3dfr_t));
        action->frame[i].msec = t;
    }

    /* find transformation for the bone in that frame */
    for(j = 0; j < action->frame[i].numtransform && action->frame[i].transform[j].boneid != bone; j++);
    if(j >= action->frame[i].numtransform) {
        action->frame[i].transform = (m3dtr_t*)realloc(action->frame[i].transform, (++action->frame[i].numtransform) * sizeof(m3dtr_t));
        if(!action->frame[i].transform) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
        action->frame[i].transform[j].boneid = bone;
    }
    action->frame[i].transform[j].pos = pos;
    action->frame[i].transform[j].ori = ori;
}

/**
 * External asset reader for the readfile callback. Also used to load the model itself
 */
unsigned char *readfile(char *fn, unsigned int *size)
{
    FILE *f;
    unsigned char *ret = NULL, *buff = NULL;
    char *in, *p;
    int i, j;

    *size = 0;
    f = fopen(fn, "rb");
    if(f) {
        fseek(f, 0L, SEEK_END);
        *size = (unsigned int)ftell(f);
        fseek(f, 0L, SEEK_SET);
        ret = (unsigned char*)malloc(*size + 1);
        if(ret) {
            fread(ret, *size, 1, f);
            /* if it's gzip compressed, uncompress it first */
            if(ret[0] == 0x1f && ret[1] == 0x8b) {
                /* skip over gzip header */
                buff = ret + 3;
                i = *buff++; buff += 6;
                if(i & 4) { j = *buff++; j += (*buff++ << 8); buff += j; }
                if(i & 8) { while(*buff++ != 0); }
                if(i & 16) { while(*buff++ != 0); }
                if(i & 2) buff += 2;
                *size -= (int)(buff - ret);
                buff = (uint8_t*)_m3dstbi_zlib_decode_malloc_guesssize_headerflag((const char*)buff, *size, 4096, &i, 0);
                if(buff) { free(ret); ret = buff; buff = NULL; *size = (unsigned int)i; }
            }
            ret[*size] = 0;
        } else
            *size = 0;
        fclose(f);
        if(storeinline) {
            inlined = (m3di_t*)realloc(inlined, (numinlined+1) * sizeof(m3di_t));
            if(!inlined) { fprintf(stderr, "m3dconv: memory allocation error\n"); exit(3); }
            in = strrchr(fn, '/');
            if(!in) in = strrchr(fn, '\\');
            if(!in) in = fn; else in++;
            in = _m3d_safestr(in, 0);
            p = strrchr(in, '.');
            if(p && (p[1] == 'p' || p[1] == 'P') && (p[2] == 'n' || p[2] == 'N') && (p[3] == 'g' || p[3] == 'G')) *p = 0;
            inlined[numinlined].name = in;
            inlined[numinlined].length = *size;
            inlined[numinlined].data = ret;
            numinlined++;
        }
    }
    return ret;
}

/* include M3D dump */
#include "dump.h"
/* include triangulation routines */
#include "tri.h"
/* include assimp related code */
#include "assimp.h"
/* include ISO-10303 related code */
#include "step.h"
/* include OBJ with NURBS related code */
#include "objnurbs.h"
/* include Blender related code */
#include "blend.h"
/* Voxel image format importers */
#include "voxel.h"
/* include MilkShape 3D related code */
#include "ms3d.h"
/* include FBX related code */
#include "fbx.h"

/**
 * Parse materials from a model, collect them in global variables and remove them from the file
 */
void parse_material(char *fn)
{
    unsigned int size, i, j, k, l;
    unsigned char *out, *data = readfile(fn, &size);
    m3d_t *m3d;
    FILE *f;

    if(data) {
        storeinline = 1;
        m3d = m3d_load(data, readfile, free, NULL);
        if(m3d && m3d->nummaterial) {
            for(i = 0; i < m3d->nummaterial; i++) {
                for(j = 0, k = -1U; j < nummaterial; j++)
                    if(!strcmp(m3d->material[i].name, material[j].name)) { k = j; break; }
                if(k == -1U) {
                    k = nummaterial++;
                    material = (m3dm_t*)realloc(material, nummaterial * sizeof(m3dm_t));
                    if(!material) { fprintf(stderr, "m3dconv: memory allocation error\n"); exit(3); }
                    material[k].name = m3d->material[i].name;
                    material[k].numprop = m3d->material[i].numprop;
                    material[k].prop = m3d->material[i].prop;
                    /* replace textureids with the index in the global inlined array */
                    for(j = 0; j < material[k].numprop; j++)
                        if(material[k].prop[j].type >= 128 && material[k].prop[j].value.textureid < m3d->numtexture)
                            for(l = 0; l < numinlined; l++)
                                if(!strcmp(m3d->texture[material[k].prop[j].value.textureid].name, inlined[l].name)) {
                                    material[k].prop[j].value.textureid = l;
                                    break;
                                }
                }
            }
            /* if it was a binary model, remove materials and save back */
            if(M3D_CHUNKMAGIC(data, '3','D','M','O')) {
                /* remove materials and inlined textures (but not inlined scripts) */
                for(i = 0; i < m3d->numinlined; i++)
                    for(j = 0; j < m3d->numtexture; j++)
                        if(!strcmp(m3d->inlined[i].name, m3d->texture[j].name))
                            m3d->inlined[i].name = NULL;
                m3d->nummaterial = 0;
                m3d->material = NULL;
                m3d->numtexture = 0;
                m3d->texture = NULL;
                out = m3d_save(m3d, m3d->vc_s == 1? 0 : (m3d->vc_s == 2 ? 1 : (m3d->vc_s == 8 ? 3 : 2)),
                    (m3d->flags & M3D_FLG_FREERAW ? 0 : M3D_EXP_NOZLIB) | (m3d->inlined ? M3D_EXP_INLINE : 0) |
                    M3D_EXP_NOVRTMAX, &size);
                if(out) {
                    f = fopen(fn, "wb");
                    if(f) {
                        fwrite(out, size, 1, f);
                        fclose(f);
                    }
                    free(out);
                }
            }
        }
        /* don't free data and m3d, because material array still has pointers into them */
    }
}

/**
 * Get the bounding cube of the model
 */
M3D_FLOAT bcube(m3d_t *m3d, int type, M3D_FLOAT *min_x, M3D_FLOAT *max_x, M3D_FLOAT *min_y, M3D_FLOAT *max_y,
    M3D_FLOAT *min_z, M3D_FLOAT *max_z)
{
    unsigned int i;
    M3D_FLOAT mi_x, ma_x, mi_y, ma_y, mi_z, ma_z, scale;
    *min_x = *min_y = *min_z = (M3D_FLOAT)1e10;
    *max_x = *max_y = *max_z = (M3D_FLOAT)-1e10;
    for(i = 0; i < m3d->numvertex; i++) {
        if((!type && m3d->vertex[i].type != VT_WORLD) || (type && m3d->vertex[i].type != VT_NORMAL)) continue;
        if(m3d->vertex[i].x > *max_x) *max_x = m3d->vertex[i].x;
        if(m3d->vertex[i].x < *min_x) *min_x = m3d->vertex[i].x;
        if(m3d->vertex[i].y > *max_y) *max_y = m3d->vertex[i].y;
        if(m3d->vertex[i].y < *min_y) *min_y = m3d->vertex[i].y;
        if(m3d->vertex[i].z > *max_z) *max_z = m3d->vertex[i].z;
        if(m3d->vertex[i].z < *min_z) *min_z = m3d->vertex[i].z;
    }
    mi_x = *min_x < (M3D_FLOAT)0.0 ? -*min_x : *min_x;
    ma_x = *max_x < (M3D_FLOAT)0.0 ? -*max_x : *max_x;
    mi_y = *min_y < (M3D_FLOAT)0.0 ? -*min_y : *min_y;
    ma_y = *max_y < (M3D_FLOAT)0.0 ? -*max_y : *max_y;
    mi_z = *min_z < (M3D_FLOAT)0.0 ? -*min_z : *min_z;
    ma_z = *max_z < (M3D_FLOAT)0.0 ? -*max_z : *max_z;
    scale = mi_x;
    if(ma_x > scale) scale = ma_x;
    if(mi_y > scale) scale = mi_y;
    if(ma_y > scale) scale = ma_y;
    if(mi_z > scale) scale = mi_z;
    if(ma_z > scale) scale = ma_z;
    if(scale == (M3D_FLOAT)0.0 || !m3d->numvertex) scale = (M3D_FLOAT)1.0;
    return scale;
}

/**
 * Get the bounding cube for a voxel image
 */
int bvox(m3d_t *m3d, int *min_x, int *max_x, int *min_y, int *max_y, int *min_z, int *max_z)
{
    unsigned int i;

    if(!m3d->numvoxel || !m3d->voxel) return 0;
    *min_x = *min_y = *min_z = 2147483647L;
    *max_x = *max_y = *max_z = -2147483648L;
    for(i = 0; i < m3d->numvoxel; i++) {
        if((int)m3d->voxel[i].x + (int)m3d->voxel[i].w > *max_x) *max_x = m3d->voxel[i].x + (int)m3d->voxel[i].w;
        if((int)m3d->voxel[i].x < *min_x) *min_x = m3d->voxel[i].x;
        if((int)m3d->voxel[i].y + (int)m3d->voxel[i].h > *max_y) *max_y = m3d->voxel[i].y + (int)m3d->voxel[i].h;
        if((int)m3d->voxel[i].y < *min_y) *min_y = m3d->voxel[i].y;
        if((int)m3d->voxel[i].z + (int)m3d->voxel[i].d > *max_z) *max_z = m3d->voxel[i].z + (int)m3d->voxel[i].d;
        if((int)m3d->voxel[i].z < *min_z) *min_z = m3d->voxel[i].z;
    }
    return 1;
}

/**
 * Mark vertices by type, this is an extra check
 */
void markvertices(m3d_t *m3d)
{
    unsigned int i, j, k, l, n;
    m3dcd_t *cd;

    if(!m3d->vertex) return;
    for(k = 0; k < m3d->numvertex; k++)
        m3d->vertex[k].type = 127;

    if(m3d->face)
        for(k = 0; k < m3d->numface; k++) {
            for(j = 0; j < 3; j++) {
                if(m3d->face[k].vertex[j] >= m3d->numvertex) { fprintf(stderr, "m3dconv: invalid vertex index in mesh\n"); exit(1); }
                if(m3d->face[k].texcoord[j] != M3D_UNDEF && m3d->face[k].texcoord[j] >= m3d->numvertex) { fprintf(stderr, "m3dconv: invalid UV index in mesh\n"); exit(1); }
                m3d->vertex[m3d->face[k].vertex[j]].type = VT_WORLD;
            }
        }

    if(m3d->shape)
        for(i = 0; i < m3d->numshape; i++)
            for(j = 0; j < m3d->shape[i].numcmd; j++) {
                if(m3d->shape[i].cmd[j].type >= (unsigned int)(sizeof(m3d_commandtypes)/sizeof(m3d_commandtypes[0])) ||
                    !m3d->shape[i].cmd[j].arg) continue;
                cd = &m3d_commandtypes[m3d->shape[i].cmd[j].type];
                for(k = n = 0, l = cd->p; k < l; k++) {
                    switch(cd->a[((k - n) % (cd->p - n)) + n]) {
                        case m3dcp_vi_t:
                            if(m3d->shape[i].cmd[j].arg[k] >= m3d->numvertex) { fprintf(stderr, "m3dconv: invalid vertex index in shape\n"); exit(1); }
                            m3d->vertex[m3d->shape[i].cmd[j].arg[k]].type = VT_WORLD;
                        break;
                        case m3dcp_qi_t:
                            if(m3d->shape[i].cmd[j].arg[k] >= m3d->numvertex) { fprintf(stderr, "m3dconv: invalid vertex index in shape\n"); exit(1); }
                            m3d->vertex[m3d->shape[i].cmd[j].arg[k]].type = VT_QUATERN;
                        break;
                        case m3dcp_va_t: n = k + 1; l += (m3d->shape[i].cmd[j].arg[k] - 1) * (cd->p - k - 1); break;
                    }
                }
            }

    if(m3d->bone)
        for(k = 0; k < m3d->numbone; k++) {
            if(m3d->bone[k].pos >= m3d->numvertex || m3d->bone[k].ori >= m3d->numvertex) { fprintf(stderr, "m3dconv: invalid vertex index in bone\n"); exit(1); }
            m3d->vertex[m3d->bone[k].pos].type = m3d->bone[k].parent != -1U ? VT_RELATIVE : VT_WORLD;
            m3d->vertex[m3d->bone[k].ori].type = VT_QUATERN;
        }

    if(m3d->action)
        for(k = 0; k < m3d->numaction; k++)
            for(j = 0; j < m3d->action[k].numframe; j++)
                for(i = 0; i < m3d->action[k].frame[j].numtransform; i++) {
                    if(m3d->action[k].frame[j].transform[i].pos >= m3d->numvertex || m3d->action[k].frame[j].transform[i].ori >= m3d->numvertex) {
                        fprintf(stderr, "m3dconv: invalid vertex index in bone\n"); exit(1);
                    }
                    m3d->vertex[m3d->action[k].frame[j].transform[i].pos].type =
                        m3d->bone[m3d->action[k].frame[j].transform[i].boneid].parent != -1U ? VT_RELATIVE : VT_WORLD;
                    m3d->vertex[m3d->action[k].frame[j].transform[i].ori].type = VT_QUATERN;
                }

    if(m3d->flags & M3D_FLG_GENNORM)
        for(k = 0; k < m3d->numface; k++) {
            for(j = 0; j < 3; j++) {
                if(m3d->face[k].normal[j] >= m3d->numvertex) { fprintf(stderr, "m3dconv: invalid normal index in mesh\n"); exit(1); }
                m3d->vertex[m3d->face[k].normal[j]].type = VT_NORMAL;
            }
        }
    else {
        for(k = 0; k < m3d->numface; k++) {
            for(j = 0; j < 3; j++) {
                if(m3d->face[k].normal[j] >= m3d->numvertex) { fprintf(stderr, "m3dconv: invalid normal index in mesh\n"); exit(1); }
                if(m3d->vertex[m3d->face[k].normal[j]].type == VT_NORMAL) continue;
                if(m3d->vertex[m3d->face[k].normal[j]].type != 127) {
                    i = m3d->numvertex++;
                    m3d->vertex = (m3dv_t*)realloc(m3d->vertex, m3d->numvertex * sizeof(m3dv_t));
                    if(!m3d->vertex) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                    memcpy(&m3d->vertex[i], &m3d->vertex[m3d->face[k].normal[j]], sizeof(m3dv_t));
                    m3d->face[k].normal[j] = i;
                    m3d->vertex[i].type = VT_NORMAL;
                }
            }
        }
    }

    for(k = 0; k < m3d->numvertex; k++)
        if(m3d->vertex[k].type == 127) {
            fprintf(stderr, "m3dconv: unreferenced vertices?\n");
            break;
        }
}

/**
 * Usage instructions
 */
void usage()
{
    printf("Model 3D Converter by bzt Copyright (C) 2019-2022 MIT license\n\n"
           "./m3dconv [-0|-1|-2|-3] [-u|-A] [-i|-e] [-t <n>] [-V <n>] [-F] [-x|-X] [-y|-Y]\n"
           "    [-z|-Y] [-R] [-r] [-g|-G] [-w] [-C] [-N] [-U] [-M] [-f <delay>] [-s <size>]\n"
           "    [-n <name>] [-l <lic>] [-a <author>] [-c <comment>] [-v|-vv] <in> <out.m3d>\n"
           "./m3dconv -m [-u|-A] [-i] [-n <name>] [-l <license>] [-a <author>]\n"
           "    [-c <comment>] [[-p|-P] <pal>] <in1.m3d> [<in2.m3d> ...] <out.m3d>\n"
           "./m3dconv [-d|-dd|-ddd|-D] <in.m3d>\n\n");
    printf(" -0:    use  8 bit coordinate precision (int8_t 1/256, default)\n"
           " -1:    use 16 bit coordinate precision (int16_t 1/65536)\n"
           " -2:    use 32 bit coordinate precision (float, default for NURBS)\n"
           " -3:    use 64 bit coordinate precision (double)\n"
           " -u:    save uncompressed binary model\n"
           " -A:    save in ASCII format\n"
           " -i:    inline assets (like textures)\n"
           " -e:    extract inlined assets\n"
           " -t:    triangulate shapes and voxels\n"
           " -V:    voxelize mesh\n"
           " -x:    rotate model +90 degrees clock-wise around its X axis in place\n");
    printf(" -X:    rotate model +90 degrees clock-wise around the X axis\n"
           " -y:    rotate model +90 degrees clock-wise around its Y axis in place\n"
           " -Y:    rotate model +90 degrees clock-wise around the Y axis\n"
           " -z:    rotate model +90 degrees clock-wise around its Z axis in place\n"
           " -Z:    rotate model +90 degrees clock-wise around the Z axis\n"
           " -R:    mirror coordinates on Z axis\n"
           " -r:    convert to right-handed coordinate system\n"
           " -g:    move model to ground (only positive Y, and centered X, Z)\n");
    printf(" -G:    move model global, to absolute center\n"
           " -w:    don't use world transformation\n"
           " -C:    don't normalize coordinates (only for debug purposes)\n"
           " -N:    don't save normal vectors\n"
           " -U:    don't save texture UV coordinates\n"
           " -F:    flip UV\n"
           " -f:    set framedelay multiplier\n"
           " -M:    export mesh only, no skeleton or animation\n"
           " -s:    set model's size in SI meters (float)\n"
           " -n:    set model's name\n"
           " -l:    set model's license string (\"MIT\", \"CC-0\", \"GPL\" etc.)\n");
    printf(" -a:    set author's name, contact (email address or git repo etc.)\n"
           " -c:    set comment on model\n"
           " -m:    create material library\n"
           " -O:    don't use our importers, use Assimp's (does not support NURBS)\n"
           " -v:    verbose\n"
           " -d:    dump contents of a M3D file\n"
           " -D:    dump the m3d_t in-memory structure\n"
           " -p:    use voxel palette\n"
           " -P:    force voxel palette\n"
           " in:    input file(s), any format that assimp can handle\n"
           " out:   output M3D filename\n\n");
    exit(0);
}

/**
 * Main function
 */
int main(int argc, char **argv)
{
    unsigned char *data = NULL, *out = NULL;
    char *infile = NULL, *outfile = NULL;
    unsigned int orig, size, k, n;
    int i, j, l, rot = 0, ret = 0, mi_x, ma_x, mi_y, ma_y, mi_z, ma_z, x, y, z, sx, sy, sz;
    long int ratio;
    FILE *f;
    m3d_t *m3d = NULL;
    M3D_FLOAT scale, t, min_x, max_x, min_y, max_y, min_z, max_z;
    M3D_VOXEL *vox = NULL;
    m3dv_t *normal = NULL, *v0, *v1, *v2, va, vb;
#ifdef PROFILING
    struct timeval tv0, tv1, tvd;
#endif

    /* parse flags and arguments */
    if(argc<3) usage();
    memset(&arg_rot, 0, sizeof(arg_rot));
    for(i=1;argv[i];i++){
        if(argv[i][0] == '-') {
            switch(argv[i][1]) {
                case 'n': if(++i>=argc) { usage(); } arg_name = argv[i]; continue;
                case 'l': if(++i>=argc) { usage(); } arg_license = argv[i]; continue;
                case 'a': if(++i>=argc) { usage(); } arg_author = argv[i]; continue;
                case 'c': if(++i>=argc) { usage(); } arg_comment = argv[i]; continue;
                case 's': if(++i>=argc) { usage(); } arg_scale = atof(argv[i]); continue;
                case 't': if(++i>=argc) { usage(); } arg_tri = atof(argv[i]); continue;
                case 'V': if(++i>=argc) { usage(); } arg_vox = atoi(argv[i]); continue;
                case 'f': if(++i>=argc) { usage(); } arg_framedelay = atof(argv[i]); continue;
                case 'p': case 'P':
                    forcevoxpal = argv[i][1] == 'P';
                    if(++i>=argc || voxpal) { usage(); }
                    data = readfile(argv[i], &orig);
                    if(data && orig) voxpal = m3d_load(data, NULL, NULL, NULL);
                continue;
                default:
                    for(j=1;argv[i][j];j++) {
                        switch(argv[i][j]) {
                            case 'd': dump++; break;
                            case 'D': dump = 99; break;
                            case 'r': right = 1; break;
                            case 'C': norm = 0; break;
                            case 'u': zip = 0; break;
                            case 'm': matlib = 1; break;
                            case 'i': doinline = 1; break;
                            case 'e': doextract = 1; break;
                            case 'A': ascii = 1; break;
                            case 'M': domesh = 1; break;
                            case 'N': withoutnorm = 1; break;
                            case 'U': withoutuv = 1; break;
                            case 'v': verbose++; break;
                            case 'w': doworld = 0; break;
                            case 'F': flip = 1; break;
                            case 'O': notourimp = 1; break;
                            case 'g':
                            case 'G':
                            case 'R':
                            case 'x':
                            case 'X':
                            case 'y':
                            case 'Y':
                            case 'z':
                            case 'Z': if(rot < (int)sizeof(arg_rot)-1) { arg_rot[rot++] = argv[i][j]; } break;
                            case '3':
                            case '2':
                            case '1':
                            case '0': quality = argv[i][j] - '0'; break;
                            default: fprintf(stderr, "unknown flag '%c'\n", argv[i][j]); return 1;
                        }
                    }
                break;
            }
        } else {
            if(!infile && !matlib) { infile = argv[i]; continue; }
            if(argv[i+1]) {
                if(matlib)
                    parse_material(argv[i]);
            } else outfile = argv[i];
        }
    }
    if(!dump && !outfile) usage();

    /* do the thing */
    if(matlib) {
        /* create a fake model from global material data and save it */
        m3d = (m3d_t*)malloc(sizeof(m3d_t));
        if(!m3d) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
        memset(m3d, 0, sizeof(m3d_t));
        m3d->name = arg_name && *arg_name ? arg_name : "MaterialLibrary";
        m3d->license = arg_license;
        m3d->author = arg_author;
        m3d->desc = arg_comment;
        m3d->scale = 1.0;
        m3d->nummaterial = nummaterial;
        m3d->material = material;
        /* use the inlined list for texture names, those must match anyway */
        m3d->numtexture = numinlined;
        m3d->texture = (m3dtx_t*)inlined;
        goto savem3d;
    } else {
        data = readfile(infile, &orig);
        if(data && orig > 0) {
            if(dump && dump < 99) {
                /* print out and explain every single byte in the model file */
                ret = dump_file(data, orig, dump);
            } else {
#ifdef PROFILING
                gettimeofday(&tv0, NULL);
#endif
                /* parse a (probably foreign) model into M3D in-memory structure */
                if(verbose) printf("Parsing model (%d bytes)\n", orig);
                setlocale(LC_NUMERIC, "C");
                storeinline = doinline;
                if(M3D_CHUNKMAGIC(data, '3','D','M','O') || M3D_CHUNKMAGIC(data, '3','d','m','o')) {
                    if(verbose > 1) printf("  M3D model\n");
                    /* if it's a binary or ASCII Model 3D file, use our own library */
                    m3d = m3d_load(data, readfile, free, NULL);
                    /* mark the vertices */
                    markvertices(m3d);
                } else
                if(!memcmp(data, "ISO-10303-2", 11)) {
                    if(verbose > 1) printf("  STEP model\n");
                    /* if it's a STEP file */
                    m3d = step_load((char*)data);
                } else if(!memcmp(data + 3, "Schematic", 9)) {
                    if(verbose > 1) printf("  Minecraft Schematic\n");
                    /* Minecraft NBT file */
                    m3d = schem_load(data, orig);
                } else if(!memcmp(data, "VOX ", 4)) {
                    if(verbose > 1) printf("  Magicavoxel\n");
                    /* Magicavoxel VOX file */
                    m3d = vox_load(data, orig);
                } else if(!memcmp(data, "#binvox", 7)) {
                    if(verbose > 1) printf("  binvox\n");
                    /* BINVOX file */
                    m3d = binvox_load(data, orig);
                } else if(!memcmp(data, "\001\001\000\000", 4)) {
                    if(verbose > 1) printf("  Qubicle\n");
                    /* Qubicle QB file */
                    m3d = qb_load(data, orig);
                } else if(!memcmp(data, "// MilkShape", 12)) {
                    if(verbose > 1) printf("  MilkShape ASCII model\n");
                    /* if it's a MilkShape 3D ASCII file */
                    m3d = ms3d_ascii_load((char*)data);
#if 0
                } else if(!memcmp(data, "MS3D000000\004", 11) && !notourimp) {
                    if(verbose > 1) printf("  MilkShape binary model\n");
                    /* if it's a MilkShape 3D bin file */
                    m3d = ms3d_bin_load(data, orig);
#endif
                } else if(!memcmp(data, "BLENDER", 7) && !notourimp) {
                    if(verbose > 1) printf("  Blender\n");
                    /* Blender file */
                    m3d = blend_load(data, orig);
                } else {
                    /* detect Wavefront OBJ and Autodesk FBX */
                    for(k = 0, i = 0; *data && k < orig && k < 65536; k++) {
                        if( !memcmp(data + k, "mtllib", 6) ||
                            !memcmp(data + k, "usemtl", 6) ||
                            !memcmp(data + k, "cstype", 6) ||
                            !memcmp(data + k, "v ", 2) ||
                            !memcmp(data + k, "vt ", 3) ||
                            !memcmp(data + k, "vn ", 3)) {
                                i = 1; break;
                            }
                        if( !memcmp(data + k, "Kaydara FBX Binary", 18) ||
                            !memcmp(data + k, "FBXVersion", 10)) {
                                i = 2; break;
                            }
                    }
                    if(i == 1 && !notourimp) {
                        /* use our OBJ loader because Assimp does not load curves and NURBS */
                        if(verbose > 1) printf("  Wavefront OBJ model\n");
                        m3d = obj_load((char*)data, infile);
                    } else
                    if(i == 2 && !notourimp) {
                        /* use UFBX because Assimp is buggy as hell */
                        if(verbose > 1) printf("  Autodesk FBX model\n");
                        m3d = fbx_load(data, orig);
                    } else {
                        /* use Assimp for all the other formats */
                        if(verbose > 1) printf("  Fallback to Assimp\n");
                        free(data);
                        data = NULL;
                        m3d = assimp_load(infile);
                    }
                }
                if(m3d) {
#ifdef PROFILING
                    gettimeofday(&tv1, NULL);
                    tvd.tv_sec = tv1.tv_sec - tv0.tv_sec;
                    tvd.tv_usec = tv1.tv_usec - tv0.tv_usec;
                    if(tvd.tv_usec < 0) { tvd.tv_sec--; tvd.tv_usec += 1000000L; }
                    printf("Imported in %ld.%06ld sec\n", tvd.tv_sec, tvd.tv_usec);
#endif
                    /* do a lot of funcky post-processing stuff with the model */
                    if(verbose) printf("Got model numvertex %d numface %d numshape %d numvoxtype %d numvoxel %d nummaterial %d\n",
                        m3d->numvertex, m3d->numface, m3d->numshape, m3d->numvoxtype, m3d->numvoxel, m3d->nummaterial);
                    /* if we need to voxelize the model */
                    if(arg_vox > 0) {
                        if(verbose) printf("  Voxelizing model (volume %d x %d x %d)\n", arg_vox, arg_vox, arg_vox);
                        if(m3d->numshape) voxelize_shapes(arg_vox, m3d);
                        if(m3d->numface)  voxelize_face(arg_vox, m3d);
                    } else
                    /* if we need to triangulate the model */
                    if(arg_tri > 0.0) {
                        if(verbose) printf("  Triangulating model (sampling %f)\n", arg_tri);
                        if(m3d->numshape) tri_shapes(arg_tri, m3d);
                        if(m3d->numvoxel) tri_voxels(arg_tri, m3d);
                    }
                    /* if we have to rotate or move the model */
                    if(rot) {
                        for(j = 0; j < rot; j++) {
                            if(verbose) {
                                switch(arg_rot[j]) {
                                    case 'g': printf("  Move model to ground\n"); break;
                                    case 'G': printf("  Move model globally centered\n"); break;
                                    case 'R': printf("  Make model right-handed\n"); break;
                                    default:  printf("  Rotating model (%c)\n", arg_rot[j]); break;
                                }
                            }
                            bcube(m3d, 0, &min_x, &max_x, &min_y, &max_y, &min_z, &max_z);
                            max_x -= min_x; max_x /= 2; max_x += min_x;
                            max_y -= min_y; max_y /= 2; max_y += min_y;
                            max_z -= min_z; max_z /= 2; max_z += min_z;
                            for(k = 0; k < m3d->numvertex; k++) {
                                switch(arg_rot[j]) {
                                    case 'g':
                                    case 'G':
                                        if(m3d->vertex[k].type == VT_WORLD) {
                                            m3d->vertex[k].x -= max_x;
                                            m3d->vertex[k].y -= arg_rot[j] == 'g' ? min_y : max_y;
                                            m3d->vertex[k].z -= max_z;
                                        }
                                    break;
                                    case 'R':
                                        m3d->vertex[k].z = -m3d->vertex[k].z;
                                    break;
                                    case 'X':
                                        t = m3d->vertex[k].z; m3d->vertex[k].z = -m3d->vertex[k].y; m3d->vertex[k].y = t;
                                    break;
                                    case 'x':
                                        if(m3d->vertex[k].type == VT_WORLD)
                                            { m3d->vertex[k].y -= max_y; m3d->vertex[k].z -= max_z; }
                                        t = m3d->vertex[k].z; m3d->vertex[k].z = -m3d->vertex[k].y; m3d->vertex[k].y = t;
                                        if(m3d->vertex[k].type == VT_WORLD)
                                            { m3d->vertex[k].y += max_y; m3d->vertex[k].z += max_z; }
                                    break;
                                    case 'Y':
                                        t = m3d->vertex[k].z; m3d->vertex[k].z = m3d->vertex[k].x; m3d->vertex[k].x = -t;
                                    break;
                                    case 'y':
                                        if(m3d->vertex[k].type == VT_WORLD)
                                            { m3d->vertex[k].x -= max_x; m3d->vertex[k].z -= max_z; }
                                        t = m3d->vertex[k].z; m3d->vertex[k].z = m3d->vertex[k].x; m3d->vertex[k].x = -t;
                                        if(m3d->vertex[k].type == VT_WORLD)
                                            { m3d->vertex[k].x += max_x; m3d->vertex[k].z += max_z; }
                                    break;
                                    case 'Z':
                                        t = m3d->vertex[k].y; m3d->vertex[k].y = -m3d->vertex[k].x; m3d->vertex[k].x = t;
                                    break;
                                    case 'z':
                                        if(m3d->vertex[k].type == VT_WORLD)
                                            { m3d->vertex[k].x -= max_x; m3d->vertex[k].y -= max_y; }
                                        t = m3d->vertex[k].y; m3d->vertex[k].y = -m3d->vertex[k].x; m3d->vertex[k].x = t;
                                        if(m3d->vertex[k].type == VT_WORLD)
                                            { m3d->vertex[k].x += max_x; m3d->vertex[k].y += max_y; }
                                    break;
                                }
                            }
                            bvox(m3d, &mi_x, &ma_x, &mi_y, &ma_y, &mi_z, &ma_z);
                            for(k = 0; k < m3d->numvoxel; k++) {
                                if(!m3d->voxel[k].w || !m3d->voxel[k].h || !m3d->voxel[k].d || !m3d->voxel[k].data) continue;
                                sx = m3d->voxel[k].w; sy = m3d->voxel[k].h; sz = m3d->voxel[k].d;
                                vox = (M3D_VOXEL*)malloc(sx * sy * sz * sizeof(M3D_VOXEL));
                                if(!vox) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                                switch(arg_rot[j]) {
                                    case 'g':
                                    case 'G':
                                        m3d->voxel[k].x -= (ma_x - mi_x)/2 + mi_x;
                                        m3d->voxel[k].y -= arg_rot[j] == 'g' ? (mi_y > 0 ? mi_y : 0) : (ma_y-mi_y)/2 + mi_y;
                                        m3d->voxel[k].z -= (ma_z - mi_z)/2 + mi_z;
                                        free(vox); vox = NULL;
                                    break;
                                    case 'R':
                                        for(y = l = 0; y < sy; y++)
                                            for(z = 0; z < sz; z++)
                                                for(x = 0; x < sx; x++)
                                                    vox[l++] = m3d->voxel[k].data[x + (sz-z-1) * sx + y * sx * sz];
                                        m3d->voxel[k].z = mi_z + ma_z - m3d->voxel[k].z - sz;
                                    break;
                                    case 'y':
                                    case 'Y':
                                        for(y = l = 0; y < sy; y++)
                                            for(x = 0; x < sx; x++)
                                                for(z = 0; z < sz; z++)
                                                    vox[l++] = m3d->voxel[k].data[x + (sz-z-1) * sx + y * sx * sz];
                                        m3d->voxel[k].w = sz;
                                        m3d->voxel[k].d = sx;
                                        l = m3d->voxel[k].z;
                                        m3d->voxel[k].z = m3d->voxel[k].x - mi_x + mi_z;
                                        m3d->voxel[k].x = ma_z - l - sz + mi_x;
                                    break;
                                    case 'z':
                                    case 'Z':
                                        for(x = l = 0; x < sx; x++)
                                            for(z = 0; z < sz; z++)
                                                for(y = 0; y < sy; y++)
                                                    vox[l++] = m3d->voxel[k].data[(sx-x-1) + z * sx + y * sx * sz];
                                        m3d->voxel[k].w = sy;
                                        m3d->voxel[k].h = sx;
                                        l = m3d->voxel[k].y;
                                        m3d->voxel[k].y = m3d->voxel[k].x - mi_x + mi_y;
                                        m3d->voxel[k].x = ma_y - l - sy + mi_x;
                                    break;
                                    case 'x':
                                    case 'X':
                                        for(z = l = 0; z < sz; z++)
                                            for(y = 0; y < sy; y++)
                                                for(x = 0; x < sx; x++)
                                                    vox[l++] = m3d->voxel[k].data[x + z * sx + (sy-y-1) * sx * sz];
                                        m3d->voxel[k].d = sy;
                                        m3d->voxel[k].h = sz;
                                        l = m3d->voxel[k].y;
                                        m3d->voxel[k].y = m3d->voxel[k].z - mi_z + mi_y;
                                        m3d->voxel[k].z = ma_y - l - sy + mi_z;
                                    break;
                                }
                                if(vox) {
                                    free(m3d->voxel[k].data);
                                    m3d->voxel[k].data = vox;
                                }
                            }
                        }
                    }
                    /* generate smooth normals. Assimp has done this for us, but not the other importers */
                    if(!withoutnorm && m3d->numface && m3d->face) {
                        /* if they are missing, calculate triangle normals into a temporary buffer */
                        for(k = 0, n = m3d->numvertex; k < m3d->numface; k++)
                            if(m3d->face[k].normal[0] == -1U) {
                                v0 = &m3d->vertex[m3d->face[k].vertex[0]];
                                v1 = &m3d->vertex[m3d->face[k].vertex[1]];
                                v2 = &m3d->vertex[m3d->face[k].vertex[2]];
                                va.x = v1->x - v0->x; va.y = v1->y - v0->y; va.z = v1->z - v0->z;
                                vb.x = v2->x - v0->x; vb.y = v2->y - v0->y; vb.z = v2->z - v0->z;
                                if(!normal) {
                                    normal = (m3dv_t*)malloc(m3d->numface * sizeof(m3dv_t));
                                    if(!normal) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                                }
                                v0 = &normal[k];
                                v0->x = (va.y * vb.z) - (va.z * vb.y);
                                v0->y = (va.z * vb.x) - (va.x * vb.z);
                                v0->z = (va.x * vb.y) - (va.y * vb.x);
                                t = _m3d_rsq((v0->x * v0->x) + (v0->y * v0->y) + (v0->z * v0->z));
                                v0->x *= t; v0->y *= t; v0->z *= t;
                                m3d->face[k].normal[0] = m3d->face[k].vertex[0] + n;
                                m3d->face[k].normal[1] = m3d->face[k].vertex[1] + n;
                                m3d->face[k].normal[2] = m3d->face[k].vertex[2] + n;
                            }
                        /* this is the fast way, we don't care if a normal is repeated in m3d->vertex */
                        if(normal) {
                            if(verbose) printf("  Generating normals");
                            m3d->numvertex <<= 1;
                            m3d->vertex = (m3dv_t*)realloc(m3d->vertex, m3d->numvertex * sizeof(m3dv_t));
                            if(!m3d->vertex) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                            memset(&m3d->vertex[n], 0, n * sizeof(m3dv_t));
                            for(k = 0; k < m3d->numface; k++)
                                for(j = 0; j < 3; j++) {
                                    v0 = &m3d->vertex[m3d->face[k].vertex[j] + n];
                                    v0->x += normal[k].x;
                                    v0->y += normal[k].y;
                                    v0->z += normal[k].z;
                                }
                            /* for each vertex, take the average of the temporary normals and use that */
                            for(k = 0, v0 = &m3d->vertex[n]; k < n; k++, v0++) {
                                t = _m3d_rsq((v0->x * v0->x) + (v0->y * v0->y) + (v0->z * v0->z));
                                v0->x *= t; v0->y *= t; v0->z *= t;
                                v0->skinid = -1U;
                            }
                            free(normal);
                        }
                    }
                    /* normalize coordinates. Don't rely on m3d_save, use M3D_EXP_NORECALC because
                     * we might need this with dump_cstruct() too */
                    scale = bcube(m3d, VT_WORLD, &min_x, &max_x, &min_y, &max_y, &min_z, &max_z);
                    if(norm) {
                        if(scale == 0.0f) scale = 1.0f;
                        if(verbose) printf("  Normalizing model %f\n", scale);
                        for(k = 0; k < m3d->numvertex; k++) {
                            if(m3d->vertex[k].type != VT_WORLD) continue;
                            m3d->vertex[k].x /= scale;
                            m3d->vertex[k].y /= scale;
                            m3d->vertex[k].z /= scale;
                        }
                        m3d->scale = scale;
                        scale = bcube(m3d, VT_NORMAL, &min_x, &max_x, &min_y, &max_y, &min_z, &max_z);
                        for(k = 0; k < m3d->numvertex; k++) {
                            if(m3d->vertex[k].type != VT_NORMAL) continue;
                            m3d->vertex[k].x /= scale;
                            m3d->vertex[k].y /= scale;
                            m3d->vertex[k].z /= scale;
                        }
                        /* normalize UV coordinates */
                        if(!withoutuv && m3d->numtmap && m3d->tmap) {
                            min_x = min_y = (M3D_FLOAT)1e10;
                            max_x = max_y = (M3D_FLOAT)0.0;
                            for(k = 0; k < m3d->numtmap; k++) {
                                if(m3d->tmap[k].u < (M3D_FLOAT)0.0) m3d->tmap[k].u = (M3D_FLOAT)1.0 - m3d->tmap[k].u;
                                if(m3d->tmap[k].v < (M3D_FLOAT)0.0) m3d->tmap[k].v = (M3D_FLOAT)1.0 - m3d->tmap[k].v;
                                if(m3d->tmap[k].u > max_x) max_x = m3d->tmap[k].u;
                                if(m3d->tmap[k].u < min_x) min_x = m3d->tmap[k].u;
                                if(m3d->tmap[k].v > max_y) max_y = m3d->tmap[k].v;
                                if(m3d->tmap[k].v < min_y) min_y = m3d->tmap[k].v;
                            }
                            scale = min_x;
                            if(max_x > scale) scale = max_x;
                            if(min_y > scale) scale = min_y;
                            if(max_y > scale) scale = max_y;
                            if(scale > (M3D_FLOAT)1.0)
                                for(k = 0; k < m3d->numtmap; k++) {
                                    m3d->tmap[k].u /= scale;
                                    m3d->tmap[k].v /= scale;
                                }
                        }
                    } else
                    /* int8 and int16 can only store normalized coordinates, when they are between -1.0 and 1.0 */
                    if(scale > 1.0f && quality < 2)
                        quality = 2;
                    /* convert to right-handed coordinate system. Don't use M3D_EXP_IDOSUCK flag
                     * here, because we might need this with dump_cstruct() too */
                    if(right) {
                        if(verbose) printf("  CCW conversion\n");
                        for(k = 0; k < m3d->numface; k++) {
                            /* swap 2nd and 3rd indices in triangles */
                            j = m3d->face[k].vertex[1];
                            m3d->face[k].vertex[1] = m3d->face[k].vertex[2];
                            m3d->face[k].vertex[2] = j;
                            j = m3d->face[k].texcoord[1];
                            m3d->face[k].texcoord[1] = m3d->face[k].texcoord[2];
                            m3d->face[k].texcoord[2] = j;
                            j = m3d->face[k].normal[1];
                            m3d->face[k].normal[1] = m3d->face[k].normal[2];
                            m3d->face[k].normal[2] = j;
                        }
                    }
                    /* override the default scale factor with command line argument if specified */
                    if(arg_scale > 0.0f) m3d->scale = (M3D_FLOAT)arg_scale;
                    if(m3d->scale <= (M3D_FLOAT)0.0) m3d->scale = (M3D_FLOAT)1.0;
                    if(!m3d->name && arg_name) m3d->name = _m3d_safestr(arg_name, 2);
                    if(!m3d->license && arg_license) m3d->license = _m3d_safestr(arg_license, 2);
                    if(!m3d->author && arg_author) m3d->author = _m3d_safestr(arg_author, 2);
                    if(!m3d->desc && arg_comment) m3d->desc = _m3d_safestr(arg_comment, 3);
#ifdef PROFILING
                    gettimeofday(&tv0, NULL);
                    tvd.tv_sec = tv0.tv_sec - tv1.tv_sec;
                    tvd.tv_usec = tv0.tv_usec - tv1.tv_usec;
                    if(tvd.tv_usec < 0) { tvd.tv_sec--; tvd.tv_usec += 1000000L; }
                    printf("Converted in %ld.%06ld sec\n", tvd.tv_sec, tvd.tv_usec);
#endif

savem3d:            if(dump >= 99) {
                        /* print out the structure we have in memory */
                        dump_cstruct(m3d);
                    } else {
                        /* save output */
                        if(doextract) {
                            if(verbose) printf("Extracting assets (%d)\n", m3d->numinlined);
                            for(k = 0; k < m3d->numinlined; k++) {
                                if(m3d->inlined[k].name && m3d->inlined[k].length && m3d->inlined[k].data) {
                                    infile = (char*)malloc(strlen(m3d->inlined[k].name) + 5);
                                    if(!infile) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                                    strcpy(infile, m3d->inlined[k].name);
                                    if(m3d->inlined[k].data[1] == 'P' && m3d->inlined[k].data[2] == 'N' &&
                                        m3d->inlined[k].data[3] == 'G')
                                        strcat(infile, ".png");
                                    if(verbose>1) printf("  Extracting '%s'\n", infile);
                                    f = fopen(infile, "wb");
                                    if(f) {
                                        fwrite(m3d->inlined[k].data, m3d->inlined[k].length, 1, f);
                                        fclose(f);
                                    } else {
                                        fprintf(stderr, "m3dconv: unable to write file: %s\n", infile);
                                    }
                                    free(infile);
                                }
                            }
                            if(quality == -1)
                                quality = m3d->vc_s == 1 ? 0 : (m3d->vc_s == 2 ? 1 : (m3d->vc_s == 8 ? 3 : 2));
                            m3d->numinlined = 0;
                            doinline = 0;
                        }
                        if(doinline && numinlined) {
                            if(verbose) printf("Inlining assets (%d)\n", numinlined);
                            m3d->inlined = (m3di_t*)realloc(m3d->inlined, (m3d->numinlined + numinlined) * sizeof(m3di_t));
                            memcpy(&m3d->inlined[m3d->numinlined], inlined, numinlined * sizeof(m3di_t));
                            m3d->numinlined += numinlined;
                        }
                        if(quality < 0) quality = m3d->numshape ? 2 : 0;
                        if(verbose) printf("Generating m3d (quality %d bits)\n", 1 << (quality + 3));
                        out = m3d_save(m3d, quality, (zip ? 0 : M3D_EXP_NOZLIB) | (ascii ? M3D_EXP_ASCII : 0) |
                            (doinline ? M3D_EXP_INLINE : 0) | M3D_EXP_NORECALC | (flip ? M3D_EXP_FLIPTXTCRD : 0) |
                            (withoutnorm ? M3D_EXP_NONORMAL : 0 ) | (withoutuv ? M3D_EXP_NOTXTCRD : 0) |
                            (domesh ? M3D_EXP_NOBONE | M3D_EXP_NOACTION : 0) | M3D_EXP_NOVRTMAX, &size);
                        if(verbose) {
                            ratio = size * 10000L / orig;
                            printf("Compression ratio %ld.%ld%%\n", ratio / 100, ratio % 100);
                        }
#ifdef PROFILING
                        gettimeofday(&tv1, NULL);
                        tvd.tv_sec = tv1.tv_sec - tv0.tv_sec;
                        tvd.tv_usec = tv1.tv_usec - tv0.tv_usec;
                        if(tvd.tv_usec < 0) { tvd.tv_sec--; tvd.tv_usec += 1000000L; }
                        printf("Exported in %ld.%06ld sec\n", tvd.tv_sec, tvd.tv_usec);
#endif
                        if(out) {
                            if(verbose) printf("Writing model (%d bytes)\n", size);
                            f = fopen(outfile, "wb");
                            if(f) {
                                fwrite(out, size, 1, f);
                                fclose(f);
                            } else {
                                fprintf(stderr, "m3dconv: unable to write file: %s\n", outfile);
                                ret = 2;
                            }
                        } else {
                            fprintf(stderr, "m3dconv: unable to create model binary\n");
                            ret = 2;
                        }
                    }
                } else {
                    fprintf(stderr, "m3dconv: unable to parse model: %s\n%s\n", infile, errstr ? errstr : "");
                    ret = 1;
                }
            }
        } else {
            fprintf(stderr, "m3dconv: unable to read file: %s\n", infile);
            ret = 1;
        }
    }
    /* clean up */
    if(m3d) {
        if(voxpal && voxpal->voxtype == m3d->voxtype)
            voxpal->voxtype = NULL;
        m3d_free(m3d);
    }
    if(voxpal) m3d_free(voxpal);
    if(data) free(data);
    if(out) free(out);
    if(inlined) free(inlined);
    return ret;
}
