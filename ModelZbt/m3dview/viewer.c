/*
 * m3dview/viewer.c
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
 * @brief simple portable Model 3D viewer - VBO version
 * https://gitlab.com/bztsrc/model3d
 *
 */

#include "viewer.h"
#include "font6.h"
#define M3D_IMPLEMENTATION
#define M3D_ASCII
#ifdef PROFILING
#include <sys/time.h>
#define M3D_PROFILING
#endif
#include <m3d.h>
#include <gl.h>
#include <unistd.h>

/* variables */
int screenw = 800, screenh = 600;

unsigned char *buff = NULL;
unsigned int size, actionid = 0, frame = -1U, fpsdivisor = 1, lastframe = 0;
char *wintitle = NULL, infostr[128];
m3d_t *model = NULL;
int mousebtn = 0, mousemove = 0, mousex = 0, mousey = 0, doframe = 0, domesh = 1, doskel = 0;
float mindist = 1, maxdist = 25, distance = 5.5, pitch = /*0*/-35.264, yaw = /*0*/45;
float light_position[4] = { -1, 2, 2, 0 }, light_color[4] = { 0.8, 0.8, 0.8, 1 }, shadow_color[4] = { 0, 0, 0, 0 };
unsigned int numtexture, texture[32];
unsigned char checker_data[4*128*128];
uint32_t default_color = 0xFF235580;
void initmodel();

#ifdef _MSC_VER
#pragma pack(push)
#pragma pack(1)
#endif
/* vertex attributes. Packing shouldn't be an issue, but be safe than sorry */
typedef struct {
    float vertex_x;
    float vertex_y;
    float vertex_z;
    float normal_x;
    float normal_y;
    float normal_z;
    float tex_u;
    float tex_v;
    uint32_t color;
    uint32_t vertexid;      /* needed for animation */
    uint32_t normalid;
}
#ifdef _MSC_VER
#pragma pack(pop)
#else
__attribute__((packed))
#endif
vbo_t;

vbo_t *vbo = NULL;
/* integer triplets, material index, vbo index and vbo size */
unsigned int numvbo, numvboidx = 0, *vboidx = NULL;

/**
 * Load model from buffer
 */
void loadbuf(unsigned char *data)
{
    model = m3d_load(data, NULL, NULL, NULL);
    if(!model) error("unable to parse model");
    initmodel();
}

/**
 * File reader callback for the M3D SDK, also used to load the model
 */
unsigned char *readfile(char *fn, unsigned int *size)
{
    FILE *f;
    unsigned char *ret = NULL;

    *size = 0;
    f = fopen(fn, "rb");
    if(f) {
        fseek(f, 0L, SEEK_END);
        *size = (unsigned int)ftell(f);
        fseek(f, 0L, SEEK_SET);
        ret = (unsigned char*)malloc(*size + 1);
        if(ret) {
            fread(ret, *size, 1, f);
            ret[*size] = 0;
        } else
            *size = 0;
        fclose(f);
    }
    return ret;
}

/**
 * Parse command line and load a model
 */
void load(int argc, char **argv)
{
    char *fn;
#ifdef PROFILING
    struct timeval tv0, tv1, tvd;
#endif

    /* check arguments */
    if(argc < 2) {
        printf("Model 3D Viewer by bzt Copyright (C) 2019 MIT license\n\n"
           "./m3dview <m3d file>"
#ifdef PREVIEW
           " [out.png]"
#endif
           "\n\n");
        exit(0);
    }
#ifdef PROFILING
    gettimeofday(&tv0, NULL);
#endif
    /* read the file */
    buff = readfile(argv[1], &size);
    if(!buff) error("unable to load model file");
#ifdef PROFILING
    gettimeofday(&tv1, NULL);
    tvd.tv_sec = tv1.tv_sec - tv0.tv_sec;
    tvd.tv_usec = tv1.tv_usec - tv0.tv_usec;
    if(tvd.tv_usec < 0) { tvd.tv_sec--; tvd.tv_usec += 1000000L; }
    printf("File read in time %ld.%06ld sec\n", tvd.tv_sec, tvd.tv_usec);
#endif
    /* decode bit-chunk into in-memory C structures */
    model = m3d_load(buff, readfile, free, NULL);
    if(!model) error("unable to parse model");
    /* set up window title */
    fn = strrchr(argv[1], '/'); if(!fn) fn = strrchr(argv[1], '\\');
    if(!fn) fn = argv[1]; else fn++;
    wintitle = (char*)malloc(strlen(fn) + strlen(model->name) + strlen(model->license) + 32);
    if(!wintitle) error("unable to allocate memory");
    strcpy(wintitle, "Model 3D Viewer: ");
    strcat(wintitle, fn);
    strcat(wintitle, " ");
    strcat(wintitle, model->name);
    if(model->license[0]) {
        strcat(wintitle, " (");
        strcat(wintitle, model->license);
        strcat(wintitle, ")");
    }
#ifdef PROFILING
    gettimeofday(&tv0, NULL);
    tvd.tv_sec = tv0.tv_sec - tv1.tv_sec;
    tvd.tv_usec = tv0.tv_usec - tv1.tv_usec;
    if(tvd.tv_usec < 0) { tvd.tv_sec--; tvd.tv_usec += 1000000L; }
    printf("Model parsed in %3ld.%06ld sec\n", tvd.tv_sec, tvd.tv_usec);
#endif
    /* initialize info string and vbo */
    initmodel();
}

/**
 * Initialize loaded model
 */
void initmodel()
{
    unsigned int i, j, k, l, last = -2U;
    uint32_t diffuse_color = default_color;
#ifdef PROFILING
    struct timeval tv0, tv1, tvd;
    gettimeofday(&tv0, NULL);
#endif
    sprintf(infostr, "%d triangles, %d vertices (%d bit), scale %g, %d actions",
        model->numface, model->numvertex, model->vc_s << 3, model->scale, model->numaction);

    /* count how many times we switch material context */
    numvbo = model->numface * 3;
    for(i = 0; i < model->numface; i++) {
        if(last != model->face[i].materialid) {
            last = model->face[i].materialid;
            numvboidx++;
        }
    }
    vboidx = (unsigned int*)malloc(numvboidx * 3 * sizeof(numvboidx));
    if(!vboidx) error("unable to allocate memory");
    /* set up vbo array */
    vbo = (vbo_t*)malloc(numvbo * sizeof(vbo_t));
    if(!vbo) error("unable to allocate memory");
    memset(vbo, 0, numvbo * sizeof(vbo_t));
    for(i = k = l = 0, last = -2U; i < model->numface; i++) {
        /* if we change material, record it in vboidx and set diffuse color in vbo.color */
        if(last != model->face[i].materialid) {
            last = model->face[i].materialid;
            diffuse_color = default_color;
            if(last < model->nummaterial)
                for(j = 0; j < model->material[last].numprop; j++)
                    if(model->material[last].prop[j].type == m3dp_Kd) {
                        diffuse_color = model->material[last].prop[j].value.color;
                        break;
                    }
            if(l)
                vboidx[l-1] = k - vboidx[l-2];
            vboidx[l] = last;
            vboidx[l + 1] = k;
            l += 3;
        }
        for(j = 0; j < 3; j++, k++) {
            /* fill up VBO records */
            memcpy(&vbo[k].vertex_x, &model->vertex[model->face[i].vertex[j]].x, 3 * sizeof(float));
            memcpy(&vbo[k].normal_x, &model->vertex[model->face[i].normal[j]].x, 3 * sizeof(float));
            if(model->tmap && model->face[i].texcoord[j] < model->numtmap) {
                vbo[k].tex_u = model->tmap[model->face[i].texcoord[j]].u;
                vbo[k].tex_v = 1.0f - model->tmap[model->face[i].texcoord[j]].v;
            } else
                vbo[k].tex_u = vbo[k].tex_v = 0.0f;
            /* if there's no material, use vertex color for vbo.color, may change for every vertex */
            vbo[k].color = (model->face[i].materialid != -1U ? diffuse_color : (
                model->vertex[model->face[i].vertex[j]].color ? model->vertex[model->face[i].vertex[j]].color : default_color))
#ifdef PREVIEW
                & 0xFFFEFEFE
#endif
                ;
            if(!vbo[k].color) vbo[k].color = default_color;
            vbo[k].vertexid = model->face[i].vertex[j];
            vbo[k].normalid = model->face[i].normal[j];
        }
    }
    if(l)
        vboidx[l-1] = k - vboidx[l-2];
#ifdef PROFILING
    gettimeofday(&tv1, NULL);
    tvd.tv_sec = tv1.tv_sec - tv0.tv_sec;
    tvd.tv_usec = tv1.tv_usec - tv0.tv_usec;
    if(tvd.tv_usec < 0) { tvd.tv_sec--; tvd.tv_usec += 1000000L; }
    printf("VBO generated in %2ld.%06ld sec\n", tvd.tv_sec, tvd.tv_usec);
#endif
}

/**
 * Multiply a vertex with a transformation matrix
 */
void vec3_mul_mat4(m3dv_t *out, m3dv_t *v, float *mat)
{
    out->x = mat[ 0] * v->x + mat[ 1] * v->y + mat[ 2] * v->z + mat[ 3];
    out->y = mat[ 4] * v->x + mat[ 5] * v->y + mat[ 6] * v->z + mat[ 7];
    out->z = mat[ 8] * v->x + mat[ 9] * v->y + mat[10] * v->z + mat[11];
}

/**
 * Multiply a vertex with a rotation matrix
 */
void vec3_mul_mat3(m3dv_t *out, m3dv_t *v, float *mat)
{
    out->x = mat[ 0] * v->x + mat[ 1] * v->y + mat[ 2] * v->z;
    out->y = mat[ 4] * v->x + mat[ 5] * v->y + mat[ 6] * v->z;
    out->z = mat[ 8] * v->x + mat[ 9] * v->y + mat[10] * v->z;
}

/**
 * Multiply a vertex with a quaternion
 */
void vec3_mul_quat(float *out, float *q)
{
    float vx = out[0], vy = out[1], vz = out[2];
    float qx = q[0], qy = q[1], qz = q[2], qw = q[3];

    out[0] = vx*(qx*qx + qw*qw - qy*qy - qz*qz) + vy*(2*qx*qy - 2*qw*qz) + vz*(2*qx*qz + 2*qw*qy);
    out[1] = vx*(2*qw*qz + 2*qx*qy) + vy*(qw*qw - qx*qx + qy*qy - qz*qz) + vz*(-2*qw*qx + 2*qy*qz);
    out[2] = vx*(-2*qw*qy + 2*qx*qz) + vy*(2*qw*qx + 2*qy*qz)+ vz*(qw*qw - qx*qx - qy*qy + qz*qz);
}

/**
 * Multiply quaternions
 */
void quat_mul_quat(float *out, float *q)
{
    float qax = q[0], qay = q[1], qaz = q[2], qaw = q[3];
    float qbx = out[0], qby = out[1], qbz = out[2], qbw = out[3];

    out[0] = qax*qbw + qaw*qbx + qay*qbz - qaz*qby;
    out[1] = qay*qbw + qaw*qby + qaz*qbx - qax*qbz;
    out[2] = qaz*qbw + qaw*qbz + qax*qby - qay*qbx;
    out[3] = qaw*qbw - qax*qbx - qay*qby - qaz*qbz;
}

/**
 * Convert a standard uint32_t color into OpenGL color
 */
void set_color(uint32_t c, float *f) {
    if(!c) {
        f[0] = f[1] = f[2] = 0.0; f[3] = 1.0;
    } else {
        f[3] = ((float)((c >> 24)&0xff)) / 255;
#ifndef PREVIEW
        f[2] = ((float)((c >> 16)&0xff)) / 255;
        f[1] = ((float)((c >>  8)&0xff)) / 255;
        f[0] = ((float)((c >>  0)&0xff)) / 255;
#else
        f[2] = ((float)((c >> 16)&0xff)) / 256;
        f[1] = ((float)((c >>  8)&0xff)) / 256;
        f[0] = ((float)((c >>  0)&0xff)) / 256;
#endif
    }
}

/**
 * Set material to use.
 */
void set_material(unsigned int mi)
{
    unsigned int i, t;
    float material[13];
    m3dm_t *m;

    /* reset material */
    memset(&material, 0, sizeof(material));
    material[3] = material[7] = material[11] = 1.0f; /* alpha values */
    glDisable(GL_TEXTURE_2D);
    /* update with what we have in this new material */
    if(mi < model->nummaterial) {
        m = &model->material[mi];
        for(i = 0; i < m->numprop; i++) {
            switch(m->prop[i].type) {
                case m3dp_Kd: /* this is already copied into the VBO */ break;
                case m3dp_Ka: set_color(m->prop[i].value.color, (float*)&material[0]); break;
                case m3dp_Ks: set_color(m->prop[i].value.color, (float*)&material[4]); break;
                case m3dp_Ke: set_color(m->prop[i].value.color, (float*)&material[8]); break;
                case m3dp_Ns: material[12] = m->prop[i].value.fnum; break;
                case m3dp_map_Kd:
                    if(numtexture) {
                        t = m->prop[i].value.textureid < numtexture ? m->prop[i].value.textureid + 1 : 0;
                        glEnable(GL_TEXTURE_2D);
                        glBindTexture(GL_TEXTURE_2D, texture[t]);
                    }
                break;
            }
        }
    }
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, (GLfloat*)&material[0]);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, (GLfloat*)&material[4]);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, (GLfloat*)&material[8]);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material[12]);
    return;
}

/**
 * Set FPS divisor for animation debugging
 */
void fpsdiv(int idx)
{
    switch(idx) {
        case 1: fpsdivisor = 2; break;
        case 2: fpsdivisor = 3; break;
        case 3: fpsdivisor = 5; break;
        case 4: fpsdivisor = 10; break;
        case 5: fpsdivisor = 15; break;
        case 6: fpsdivisor = 20; break;
        case 7: fpsdivisor = 25; break;
        case 8: fpsdivisor = 30; break;
        case 9: fpsdivisor = 60; break;
        default: fpsdivisor = 1; break;
    }
}

/**
 * Switch to next frame
 */
void nextframe(void)
{
    doframe = 0; frame++;
    continous();
}

/**
 * Switch to previous frame
 */
void prevframe(void)
{
    doframe = 0; frame--;
    continous();
}

/**
 * Toggle continous playback
 */
void continous(void)
{
    doframe ^= 1;
    if(actionid < model->numaction) {
        if(frame == -1U) frame = model->action[actionid].numframe - 1;
        if(frame > model->action[actionid].numframe - 1) frame = 0;
    } else
        frame = 0;
}

/**
 * Zoom in
 */
void zoomin(void)
{
    distance -= 0.01 * distance;
    if(distance < 0.000001) distance = 0.000001;
}

/**
 * Zoom out
 */
void zoomout(void)
{
    distance += 0.01 * distance;
    if(distance > 100000) distance = 100000;
}

/**
 * Print an UTF-8 string
 */
void glPrint(char *s)
{
    unsigned int c;
    while(*s) {
        if((*s & 128) != 0) {
            if(!(*s & 32)) { c = ((*s & 0x1F)<<6)|(*(s+1) & 0x3F); s++; } else
            if(!(*s & 16)) { c = ((*s & 0xF)<<12)|((*(s+1) & 0x3F)<<6)|(*(s+2) & 0x3F); s += 2; } else
            if(!(*s & 8)) { c = ((*s & 0x7)<<18)|((*(s+1) & 0x3F)<<12)|((*(s+2) & 0x3F)<<6)|(*(s+3) & 0x3F); *s += 3; }
            else c = 0;
        } else c = *s;
        s++;
        if(c >= (unsigned int)(sizeof(font)/sizeof(font[0]))) c = 0;
        glBitmap(FONT_WIDTH, FONT_HEIGHT, 0, 0, (float)FONT_WIDTH, 0.0, ((unsigned char*)&font + c * FONT_HEIGHT));
    }
}

/**
 * Set up OpenGL context
 */
void setupgl(void)
{
    unsigned int i, j, k;

    glEnable(GL_NORMALIZE);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.4, 0.4, 0.4, 1.0);
    glShadeModel(GL_SMOOTH);
    glFrontFace(GL_CCW);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_color);
    glLightfv(GL_LIGHT0, GL_AMBIENT, shadow_color);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glViewport(0, 0, screenw, screenh);

    glVertexPointer(3, GL_FLOAT, sizeof(vbo_t), &vbo[0].vertex_x);
    glNormalPointer(GL_FLOAT, sizeof(vbo_t), &vbo[0].normal_x);
    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(vbo_t), &vbo[0].color);
    glTexCoordPointer(2, GL_FLOAT, sizeof(vbo_t), &vbo[0].tex_u);

    /* set up GL textures */
#ifdef GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, (GLint*)&numtexture);
    if(numtexture > 31) numtexture = 31;
    if(numtexture < 1) return;
#else
    numtexture = 32;
#endif
    memset(texture, 0, sizeof(texture));
    glGenTextures(numtexture, (GLuint*)&texture);
    for (j = k = 0; j < 128; j++)
        for (i = 0; i < 128; i++, k += 4) {
            memcpy(&checker_data[k], &default_color, 4);
            checker_data[k] += ((((i>>5) & 1) ^ ((j>>5) & 1)) << 5);
        }
    glBindTexture(GL_TEXTURE_2D, texture[0]);
#ifdef GL_GENERATE_MIPMAP
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
#endif
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 128, 128, 0, GL_RGBA, GL_UNSIGNED_BYTE, checker_data);
    /* add textures from model */
    if(model->numtexture) {
        for(i = 0; i < numtexture && i < model->numtexture; i++) {
            if(!model->texture[i].w || !model->texture[i].h || !model->texture[i].d) {
                fprintf(stderr, "m3dview: unable to load texture '%s'\n", model->texture[i].name);
                texture[1 + i] = texture[0];
                continue;
            }
            glBindTexture(GL_TEXTURE_2D, texture[1 + i]);
#ifdef GL_GENERATE_MIPMAP
            glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
#endif
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            switch(model->texture[i].f) {
                case 1: k = GL_LUMINANCE; break;
                case 2: k = GL_LUMINANCE_ALPHA; break;
                case 3: k = GL_RGB; break;
                default: k = GL_RGBA; break;
            }
            glTexImage2D(GL_TEXTURE_2D, 0, k, model->texture[i].w, model->texture[i].h, 0, k, GL_UNSIGNED_BYTE,
                model->texture[i].d);
        }
    }
}

/**
 * Animate the mesh
 */
m3db_t *animate_model(unsigned int msec)
{
    unsigned int i, j, s;
    m3db_t *animpose;
    m3dv_t tmp1, tmp2;

    if(actionid < model->numaction && doframe) {
        /* if we are frame debugging, use the exact timestamp of the frame as msec */
        msec = model->action[actionid].frame[frame].msec;
    } else
        /* otherwise modify by the debugging fps divisor (is 1 by default) */
        msec /= fpsdivisor;

    /* get the animation-pose skeleton*/
    animpose = m3d_pose(model, actionid, msec);

    /* don't regenerate if we have the same timestamp as last time */
    if(msec == lastframe) return animpose;
    lastframe = msec;

    /* convert mesh vertices from bind-pose into animation-pose */
    for(i = 0; i < numvbo; i++) {
        s = model->vertex[vbo[i].vertexid].skinid;
        if(s != -1U) {
            /* we assume that vbo_t is packed and normals follow vertex coordinates */
            memset(&vbo[i].vertex_x, 0, 6 * sizeof(float));
            for(j = 0; j < M3D_NUMBONE && model->skin[s].weight[j] > 0.0; j++) {
                /* transfer from bind-pose model-space into bone-local space */
                vec3_mul_mat4(&tmp1, &model->vertex[vbo[i].vertexid], (float*)&model->bone[ model->skin[s].boneid[j] ].mat4);
                /* transfer from bone-local space into animation-pose model-space */
                vec3_mul_mat4(&tmp2, &tmp1, (float*)&animpose[ model->skin[s].boneid[j] ].mat4);
                /* multiply with weight and accumulate */
                vbo[i].vertex_x += tmp2.x * model->skin[s].weight[j];
                vbo[i].vertex_y += tmp2.y * model->skin[s].weight[j];
                vbo[i].vertex_z += tmp2.z * model->skin[s].weight[j];
                /* now again for the normal vector */
                vec3_mul_mat3(&tmp1, &model->vertex[vbo[i].normalid], (float*)&model->bone[ model->skin[s].boneid[j] ].mat4);
                vec3_mul_mat3(&tmp2, &tmp1, (float*)&animpose[ model->skin[s].boneid[j] ].mat4);
                vbo[i].normal_x += tmp2.x * model->skin[s].weight[j];
                vbo[i].normal_y += tmp2.y * model->skin[s].weight[j];
                vbo[i].normal_z += tmp2.z * model->skin[s].weight[j];
            }
        }
    }
    return animpose;
}

/**
 * Display the model
 */
void display(unsigned int msec)
{
    m3db_t *animpose = NULL, *bones;
    unsigned int i, j;
    float fov, *skel = NULL, s;
    char tmp[64];

    /* handle model rotation */
    if(mousemove) {
        yaw -= mousex * 0.3;
        pitch -= mousey * 0.2;
        if (pitch < -90) pitch = -90;
        if (pitch > 90) pitch = 90;
        if (yaw < 0) yaw += 360;
        if (yaw > 360) yaw -= 360;
        mousemove = 0;
    }
    /* switch action to animate */
    if(actionid == -1U) actionid = model->numaction;
    else if((unsigned int)actionid > model->numaction) actionid = 0;
    /* animate the mesh */
    if(model->numaction)
        animpose = animate_model(msec);

    /* display model */
#ifdef PREVIEW
    if(!msec)
        glClearColor(1.0, 1.0, 1.0, 1.0);
#endif
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    fov = 0.221694 * (mindist/5);
    glFrustum(-fov, fov, -fov, fov, mindist/5, maxdist*5);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_DEPTH_TEST);
    glTranslatef(0, 0, -distance*1.1);
    glRotatef(-pitch, 1, 0, 0);
    glRotatef(-yaw, 0, 1, 0);
    glTranslatef(0, 0, 0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    /* draw the coordinate system */
    if(msec) {
        glBegin(GL_LINES);
        glColor4f(1.0, 0.0, 0.0, 1.0); glVertex3f(0, 0, 0); glVertex3f( 1.001, 0.0,   0.0);
        glColor4f(0.5, 0.4, 0.4, 1.0); glVertex3f(0, 0, 0); glVertex3f(-1.001, 0.0,   0.0);
        glColor4f(0.0, 1.0, 0.0, 1.0); glVertex3f(0, 0, 0); glVertex3f( 0.0,   1.001, 0.0);
        glColor4f(0.4, 0.5, 0.4, 1.0); glVertex3f(0, 0, 0); glVertex3f( 0.0,  -1.001, 0.0);
        glColor4f(0.0, 0.0, 1.0, 1.0); glVertex3f(0, 0, 0); glVertex3f( 0.0,   0.0,   1.001);
        glColor4f(0.4, 0.4, 0.5, 1.0); glVertex3f(0, 0, 0); glVertex3f( 0.0,   0.0,  -1.001);
        glEnd();
    }

    /* draw the skeleton */
    if(doskel && model->numbone) {
        bones = animpose ? animpose : model->bone;
        /* the format stores parent local bones. We need to convert those to model space if we want to draw a skeleton */
        skel = (float*)malloc(model->numbone * 7 * sizeof(float));
        if(skel) {
            glColor4f(1.0, 0.0, 0.0, 1.0);
            for(i = 0; i < model->numbone; i++) {
                skel[i * 7 + 0] = model->vertex[bones[i].pos].x;
                skel[i * 7 + 1] = model->vertex[bones[i].pos].y;
                skel[i * 7 + 2] = model->vertex[bones[i].pos].z;
                skel[i * 7 + 3] = model->vertex[bones[i].ori].x;
                skel[i * 7 + 4] = model->vertex[bones[i].ori].y;
                skel[i * 7 + 5] = model->vertex[bones[i].ori].z;
                skel[i * 7 + 6] = model->vertex[bones[i].ori].w;
                if(bones[i].parent != M3D_UNDEF) {
                    quat_mul_quat(&skel[i * 7 + 3], &skel[bones[i].parent * 7 + 3]);
                    vec3_mul_quat(&skel[i * 7], &skel[bones[i].parent * 7 + 3]);
                    skel[i * 7 + 0] += skel[bones[i].parent * 7 + 0];
                    skel[i * 7 + 1] += skel[bones[i].parent * 7 + 1];
                    skel[i * 7 + 2] += skel[bones[i].parent * 7 + 2];
                    /* draw a line between child and parent bone */
                    glBegin(GL_LINES);
                    glVertex3f(skel[bones[i].parent * 7 + 0], skel[bones[i].parent * 7 + 1], skel[bones[i].parent * 7 + 2]);
                    glVertex3f(skel[i * 7 + 0], skel[i * 7 + 1], skel[i * 7 + 2]);
                    glEnd();
                }
                /* draw a cube at joint */
                glPushMatrix();
                glTranslatef(skel[i * 7 + 0], skel[i * 7 + 1], skel[i * 7 + 2]);
                glBegin(GL_TRIANGLES);
                s = 0.025f;
                glVertex3f(-s, -s, +s); glVertex3f(+s, -s, +s); glVertex3f(-s, +s, +s);
                glVertex3f(+s, +s, +s); glVertex3f(-s, +s, +s); glVertex3f(+s, -s, +s);
                glVertex3f(-s, -s, -s); glVertex3f(-s, +s, -s); glVertex3f(+s, -s, -s);
                glVertex3f(+s, +s, -s); glVertex3f(+s, -s, -s); glVertex3f(-s, +s, -s);
                glVertex3f(-s, +s, -s); glVertex3f(-s, +s, +s); glVertex3f(+s, +s, +s);
                glVertex3f(+s, +s, -s); glVertex3f(-s, +s, -s); glVertex3f(+s, +s, +s);
                glVertex3f(-s, -s, -s); glVertex3f(+s, -s, +s); glVertex3f(-s, -s, +s);
                glVertex3f(+s, -s, -s); glVertex3f(+s, -s, +s); glVertex3f(-s, -s, -s);
                glVertex3f(+s, -s, -s); glVertex3f(+s, +s, -s); glVertex3f(+s, +s, +s);
                glVertex3f(+s, -s, +s); glVertex3f(+s, -s, -s); glVertex3f(+s, +s, +s);
                glVertex3f(-s, -s, -s); glVertex3f(-s, +s, +s); glVertex3f(-s, +s, -s);
                glVertex3f(-s, -s, +s); glVertex3f(-s, +s, +s); glVertex3f(-s, -s, -s);
                glEnd();
                glPopMatrix();
            }
            free(skel);
        }
    }
    if(animpose) {
        free(animpose);
        animpose = NULL;
    }

    /* draw the mesh */
    if(domesh) {
        glEnable(GL_COLOR_MATERIAL);
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glAlphaFunc(GL_GREATER, 0.2);
        glEnable(GL_ALPHA_TEST);
        glEnable(GL_BLEND);
        for(i = j = 0; i < numvboidx; i++, j += 3) {
            set_material(vboidx[j + 0]);
            glDrawArrays(GL_TRIANGLES, vboidx[j + 1], vboidx[j + 2]);
        }
        glDisable(GL_BLEND);
        glDisable(GL_ALPHA_TEST);
        glDisable(GL_LIGHTING);
        glDisable(GL_COLOR_MATERIAL);
    }
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);

    /* on screen text */
    if(msec) {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, screenw, screenh, 0, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glColor4f(1, 1, 1, 1);
        glPushClientAttrib( GL_CLIENT_PIXEL_STORE_BIT );
        glPixelStorei( GL_UNPACK_SWAP_BYTES,  GL_FALSE );
        glPixelStorei( GL_UNPACK_LSB_FIRST,   GL_FALSE );
        glPixelStorei( GL_UNPACK_ROW_LENGTH,  0        );
        glPixelStorei( GL_UNPACK_SKIP_ROWS,   0        );
        glPixelStorei( GL_UNPACK_SKIP_PIXELS, 0        );
        glPixelStorei( GL_UNPACK_ALIGNMENT,   1        );
        glRasterPos2f(0.0, (float)FONT_HEIGHT);
        glPrint(model->name);
        glPrint(" (");
        glPrint(model->license[0] ? model->license : "no license");
        glPrint(", ");
        glPrint(model->author[0] ? model->author : "no author");
        glPrint(")");
        glRasterPos2f(0.0, (float)(FONT_HEIGHT*2));
        glPrint(infostr);
        if(model->numaction) {
            glPrint(", current: ");
            glPrint(actionid < model->numaction ? model->action[actionid].name : "(bind-pose)");
        }
        if(doframe) {
            sprintf(tmp, "frame %4d / %4d",
                actionid < model->numaction ? frame + 1 : 1,
                actionid < model->numaction ? model->action[actionid].numframe : 1);
            glRasterPos2f(0.0, (float)(FONT_HEIGHT*3));
            glPrint(tmp);
        }
        glRasterPos2f(0.0, (float)(screenh - 2));
        glPrint("[M]: toggle mesh, [S]: toggle skeleton, [PgUp]/[PgDn]: change action");
        glPopClientAttrib();
    }
}

/**
 * Print an error message and quit
 */
void error(char *msg)
{
    fprintf(stderr, "m3dview: %s\n", msg);
    cleanup();
    exit(1);
}

/**
 * Clean up on exit
 */
void cleanup()
{
    if(model) m3d_free(model);
    if(vbo) free(vbo);
    if(buff) free(buff);
}
