This is the old version of the viewer. It does generate a vertex buffer only if the model is animated (simply because you can't
animate a mesh without one), but even then it uses direct OpenGL commands. This is not as effective as the current version, but
I kept this too because the way how it displays faces is more readable and helps understanding M3D structures better.

```c
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
 * @brief simple portable Model 3D viewer - Simple OpenGL commands
 * https://gitlab.com/bztsrc/model3d
 *
 */

#include "viewer.h"
#include "font6.h"
#define M3D_IMPLEMENTATION
#define M3D_ASCII
#include <m3d.h>
#include <gl.h>
#include <unistd.h>

/* variables */
unsigned char *buff = NULL;
unsigned int size, actionid = 0, frame = -1U, fpsdivisor = 1, lastframe = 0;
char *wintitle = NULL, infostr[64];
m3d_t *model = NULL;
int screenw = 640, screenh = 480, doframe = 0;
int mousebtn = 0, mousemove = 0, mousex = 0, mousey = 0;
float mindist = 1, maxdist = 25, distance = 5.5, pitch = 0/*-35.264*/, yaw = 0/*45*/;
float light_position[4] = { -1, 2, 2, 0 }, light_color[4] = { 1, 1, 0.5, 1 }, shadow_color[4] = { 0, 0, 0, 0 };
unsigned int numvbo = 0, numtexture, texture[32];
m3dv_t *vbo = NULL;
unsigned char checker_data[4*128*128];

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
    /* read the file */
    buff = readfile(argv[1], &size);
    if(!buff) error("unable to load model file");
    /* decode bit-chunk into in-memory C structures */
    model = m3d_load(buff, readfile, free, NULL);
    if(!model) error("unable to parse model");
    /* set up window title */
    wintitle = (char*)malloc(strlen(model->name) + strlen(model->license) + 32);
    if(!wintitle) error("unable to allocate memory");
    strcpy(wintitle, "Model 3D Viewer: ");
    strcat(wintitle, model->name);
    if(model->license[0]) {
        strcat(wintitle, " (");
        strcat(wintitle, model->license);
        strcat(wintitle, ")");
    }
    sprintf(infostr, "%d triangles, %d vertices (%d bit), %d actions",
        model->numface, model->numvertex, model->vc_s << 3, model->numaction);
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
    float color[4];
    m3dm_t *m;

    if(mi == -1U || mi >= model->nummaterial) return;
    m = &model->material[mi];
    glEnd();
    /* reset GL material */
    glDisable(GL_TEXTURE_2D);
    glColor4f(0.5, 0.3, 0.1, 1);
    set_color(0, (float*)&color);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, (GLfloat*)&color);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, (GLfloat*)&color);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, (GLfloat*)&color);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.0);
    /* update with what we have in this new material */
    for(i = 0; i < m->numprop; i++) {
        switch(m->prop[i].type) {
            case m3dp_Kd:
                set_color(m->prop[i].value.color, (float*)&color);
                glColor4fv((GLfloat*)&color);
            break;
            case m3dp_Ka:
                set_color(m->prop[i].value.color, (float*)&color);
                glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, (GLfloat*)&color);
            break;
            case m3dp_Ks:
                set_color(m->prop[i].value.color, (float*)&color);
                glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, (GLfloat*)&color);
            break;
            case m3dp_Ke:
                set_color(m->prop[i].value.color, (float*)&color);
                glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, (GLfloat*)&color);
            break;
            case m3dp_Ns:
                glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, m->prop[i].value.fnum);
            break;
            case m3dp_map_Kd:
                if(numtexture) {
                    t = m->prop[i].value.textureid < numtexture ? m->prop[i].value.textureid + 1 : 0;
                    glEnable(GL_TEXTURE_2D);
                    glBindTexture(GL_TEXTURE_2D, texture[t]);
                }
            break;
        }
    }
    glBegin(GL_TRIANGLES);
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
    char *title, *name;

    doframe ^= 1;
    if(actionid < model->numaction) {
        if(frame == -1U) frame = model->action[actionid].numframe - 1;
        if(frame > model->action[actionid].numframe - 1) frame = 0;
    } else
        frame = 0;

    name = doframe ? (actionid < model->numaction ? model->action[actionid].name : "(bind-pose)") : model->name;

    title = (char*)malloc(strlen(name) + 64);
    if(title) {
        if(doframe)
            sprintf(title, "Model 3D Viewer: %s (frame %4d / %4d)", name,
                actionid < model->numaction ? frame + 1 : 1,
                actionid < model->numaction ? model->action[actionid].numframe : 1);
        else
            sprintf(title, "Model 3D Viewer: %s", model->name);
        set_title(title);
        free(title);
    }
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

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_NORMALIZE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.4, 0.4, 0.4, 1.0);
    glShadeModel(GL_SMOOTH);
    glFrontFace(GL_CCW);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_color);
    glLightfv(GL_LIGHT0, GL_AMBIENT, shadow_color);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glViewport(0, 0, screenw, screenh);
    /* set up vbo array */
    if(model->numface && model->numbone && model->numskin && model->numaction) {
        numvbo = model->numface * 3 /* triangle */ * 2 /* vertex + normal */;
        vbo = (m3dv_t*)malloc(numvbo * sizeof(m3dv_t));
        if(!vbo) error("unable to allocate memory");
        /* create separate vertex records in vbo, because animation may change differently
         * for different triangles that share vertex coordinates */
        for(i = k = 0; i < model->numface; i++)
            for(j = 0; j < 3; j++, k += 2) {
                memcpy(&vbo[k+0], &model->vertex[model->face[i].vertex[j]], sizeof(m3dv_t));
                memcpy(&vbo[k+1], &model->vertex[model->face[i].normal[j]], sizeof(m3dv_t));
                /* to avoid allocating extra memory, we store the original vertex id in vbo color */
                vbo[k+0].color = model->face[i].vertex[j];
                vbo[k+1].color = model->face[i].normal[j];
                /* copy vertex skinid to normal, normals usually are skin neutral in models */
                vbo[k+1].skinid = vbo[k+0].skinid;
            }
    }
    /* set up GL textures */
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, (GLint*)&numtexture);
    if(numtexture > 31) numtexture = 31;
    if(numtexture < 1) return;
    memset(texture, 0, sizeof(texture));
    glGenTextures(numtexture, (GLuint*)&texture);
    for (j = k = 0; j < 128; j++)
        for (i = 0; i < 128; i++) {
            checker_data[k++] = 127 + ((((i>>5) & 1) ^ ((j>>5) & 1)) << 5);
            checker_data[k++] = 85;
            checker_data[k++] = 25;
            checker_data[k++] = 255;
        }
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
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
            glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, model->texture[i].w, model->texture[i].h, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                model->texture[i].d);
        }
    }
}

/**
 * Animate the mesh
 */
void animate_model(unsigned int msec)
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

    /* don't regenerate if we have the same timestamp as last time */
    if(msec == lastframe) return;
    lastframe = msec;

    /* get the animation-pose skeleton*/
    animpose = m3d_pose(model, actionid, msec);
    /* convert mesh vertices from bind-pose into animation-pose */
    for(i = 0; i < numvbo; i++) {
        s = vbo[i].skinid;
        if(s != -1U) {
            vbo[i].x = vbo[i].y = vbo[i].z = 0;
            for(j = 0; j < M3D_NUMBONE && model->skin[s].weight[j] > 0.0; j++) {
                /* transfer from bind-pose model-space into bone-local space */
                vec3_mul_mat4(&tmp1, &model->vertex[vbo[i].color], (float*)&model->bone[ model->skin[s].boneid[j] ].mat4);
                /* transfer from bone-local space into animation-pose model-space */
                vec3_mul_mat4(&tmp2, &tmp1, (float*)&animpose[ model->skin[s].boneid[j] ].mat4);
                /* adjust with weight and accumulate */
                vbo[i].x += tmp2.x * model->skin[s].weight[j];
                vbo[i].y += tmp2.y * model->skin[s].weight[j];
                vbo[i].z += tmp2.z * model->skin[s].weight[j];
            }
        }
    }
    free(animpose);
}

/**
 * Display the model
 */
void display(unsigned int msec)
{
    unsigned int i, j, k, mi = -1U, c = 0, lc = 0;
    m3dv_t *v;
    float color[4] = { 0, 0, 0, 1 }, fov;

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
        glDisable(GL_LIGHTING);
        glBegin(GL_LINES);
        glColor4f(1, 0, 0, 1);
        glVertex3f(0, 0, 0); glVertex3f(1.001, 0, 0);
        glColor4f(0.5, 0.4, 0.4, 1);
        glVertex3f(0, 0, 0); glVertex3f(-1.001, 0, 0);
        glColor4f(0, 1, 0, 1);
        glVertex3f(0, 0, 0); glVertex3f(0, 1.001, 0);
        glColor4f(0.4, 0.5, 0.4, 1);
        glVertex3f(0, 0, 0); glVertex3f(0, -1.001, 0);
        glColor4f(0, 0, 1, 1);
        glVertex3f(0, 0, 0); glVertex3f(0, 0, 1.001);
        glColor4f(0.4, 0.4, 0.5, 1);
        glVertex3f(0, 0, 0); glVertex3f(0, 0, -1.001);
        glEnd();
    }
    glColor4f(0.5, 0.3, 0.1, 1);

    /* draw the mesh */
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    if(model->numtexture) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture[0]);
    }
    glAlphaFunc(GL_GREATER, 0.2);
    glEnable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);
    /* animate the mesh if we have a vertex buffer object */
    if(vbo)
        animate_model(msec);
    /* you should use glDrawArrays here. this is a simple, most portable viewer */
    glBegin(GL_TRIANGLES);
    for(i = k = 0; i < model->numface; i++) {
        /* if material changed */
        if(mi != model->face[i].materialid) {
            mi = model->face[i].materialid;
            set_material(mi);
            lc = 0;
        }
        /* we have triangles */
        for(j = 0; j < 3; j++, k += 2) {
            /* get normals and color */
            if(vbo) {
                v = &vbo[k+1];
                glNormal3f(v->x, v->y, v->z);
                v = &vbo[k];
                /* so save space, we've stored the original vertex id in vbo colors */
                c = model->vertex[vbo[k].color].color;
            } else {
                v = &model->vertex[model->face[i].normal[j]];
                glNormal3f(v->x, v->y, v->z);
                v = &model->vertex[model->face[i].vertex[j]];
                c = v->color;
            }
            /* if there's no material and color changed */
            if(mi == -1U && c != lc) {
                lc = c;
                set_color(c, (float*)&color);
                glColor4fv((GLfloat*)&color);
            }
            if(model->face[i].texcoord[j] != -1U) {
                glTexCoord2f(model->tmap[model->face[i].texcoord[j]].u, 1.0 - model->tmap[model->face[i].texcoord[j]].v);
            }
            /* add the vertex */
            glVertex3f(v->x, v->y, v->z);
        }
    }
    glEnd();

    glDisable(GL_BLEND);
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_DEPTH_TEST);
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
```
