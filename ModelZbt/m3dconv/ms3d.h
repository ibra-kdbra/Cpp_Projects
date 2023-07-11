/*
 * m3dconv/ms3d.h
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
 * @brief simple 3D model to M3D converter ASCII MilkShape importer
 * https://gitlab.com/bztsrc/model3d
 *
 */

/**
 * Get next valid (non-empty, non-comment) line
 */
char *ms3d_nextline(char *data)
{
    if(!*data) return data;
    do {
        data = _m3d_findnl(data);
    } while(*data == '/');
    return data;
}

/**
 * Add to animation frame
 */
void ms3d_addframe(m3d_t *m3d, uint32_t t, unsigned int bone, unsigned int pos, unsigned int ori)
{
    m3da_t *action;

    /* find action (there should be only one for MS3D) */
    if(!m3d->action) {
        m3d->action = action = (m3da_t*)malloc(sizeof(m3da_t));
        if(!m3d->action) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
        memset(m3d->action, 0, sizeof(m3da_t));
        m3d->action[0].name = (char*)malloc(5);
        if(!m3d->action[0].name) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
        strcpy(m3d->action[0].name, "Anim");
        m3d->numaction = 1;
    } else
        action = &m3d->action[m3d->numaction - 1];

    /* add to frame */
    _m3d_addframe(action, t, bone, pos, ori);
}

/**
 * Load a model and convert it's structures into a Model 3D in-memory format
 */
m3d_t *ms3d_ascii_load(char *data)
{
    m3d_t *m3d;
    m3dm_t *m;
    uint32_t j, n, num, nv, nn, nf;
    char *s, *e, *d, *path, *bn;
    unsigned int i, l;
    int mi, mt[4] = { m3dp_Ka, m3dp_Kd, m3dp_Ks, m3dp_Ke };
    float r,g,b,a, t,x,y,z;

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

    while(*data) {
        /* skip until chunk found */
        while(*data && (*data < 'A' || *data > 'Z'))
            data = ms3d_nextline(data);
        if(!*data) break;

        s = data;
        data = _m3d_findarg(data);
        data = _m3d_getint(data, &num);
        data = ms3d_nextline(data);

        if(!memcmp(s, "Meshes:", 7)) {
            for(n = 0; n < num; n++) {
                /* first line: name, flags, material index */
                for(++data; *data && *data != '\"'; data++);
                data = _m3d_findarg(data);
                data = _m3d_getint(data, (uint32_t*)&mi);
                data = ms3d_nextline(data);
                /* second: number of vertex */
                data = _m3d_getint(data, &nv);
                data = ms3d_nextline(data);
                m3d->vertex = (m3dv_t*)realloc(m3d->vertex, (m3d->numvertex + nv) * sizeof(m3dv_t));
                if(!m3d->vertex) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                memset(&m3d->vertex[m3d->numvertex], 0, nv * sizeof(m3dv_t));
                m3d->tmap = (m3dti_t*)realloc(m3d->tmap, (m3d->numtmap + nv) * sizeof(m3dti_t));
                if(!m3d->tmap) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                memset(&m3d->tmap[m3d->numtmap], 0, nv * sizeof(m3dti_t));
                for(i = 0; i < nv; i++) {
                    /* vertex: flags, x, y, z, u, v, bone index */
                    data = _m3d_findarg(data);
                    data = _m3d_getfloat(data, &m3d->vertex[m3d->numvertex + i].x);
                    data = _m3d_getfloat(data, &m3d->vertex[m3d->numvertex + i].y);
                    data = _m3d_getfloat(data, &m3d->vertex[m3d->numvertex + i].z);
                    data = _m3d_getfloat(data, &m3d->tmap[m3d->numtmap + i].u);
                    data = _m3d_getfloat(data, &x); m3d->tmap[m3d->numtmap + i].v = (M3D_FLOAT)1.0 - x;
                    data = _m3d_getint(data, &j); m3d->vertex[m3d->numvertex + i].skinid = j;
                    data = ms3d_nextline(data);
                }
                /* next: number of normals */
                data = _m3d_getint(data, &nn);
                data = ms3d_nextline(data);
                m3d->vertex = (m3dv_t*)realloc(m3d->vertex, (m3d->numvertex + nv + nn) * sizeof(m3dv_t));
                if(!m3d->vertex) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                memset(&m3d->vertex[m3d->numvertex + nv], 0, nn * sizeof(m3dv_t));
                for(i = 0; i < nn; i++) {
                    /* normal: x, y, z */
                    m3d->vertex[m3d->numvertex + nv + i].skinid = -1U;
                    data = _m3d_getfloat(data, &m3d->vertex[m3d->numvertex + nv + i].x);
                    data = _m3d_getfloat(data, &m3d->vertex[m3d->numvertex + nv + i].y);
                    data = _m3d_getfloat(data, &m3d->vertex[m3d->numvertex + nv + i].z);
                    data = ms3d_nextline(data);
                }
                /* next: number of faces */
                data = _m3d_getint(data, &nf);
                data = ms3d_nextline(data);
                m3d->face = (m3df_t*)realloc(m3d->face, (m3d->numface + nf) * sizeof(m3df_t));
                if(!m3d->face) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                memset(&m3d->face[m3d->numface], 255, nf * sizeof(m3df_t));
                for(i = 0; i < nf; i++) {
                    /* triangle: flags, v1, v2, v3, n1, n2, n3, smoothing group */
                    m3d->face[m3d->numface + i].materialid = mi;
                    data = _m3d_findarg(data);
                    data = _m3d_getint(data, &j);
                    data = _m3d_findarg(data);
                    m3d->face[m3d->numface + i].vertex[0] = m3d->numvertex + j;
                    m3d->face[m3d->numface + i].texcoord[0] = m3d->numtmap + j;
                    data = _m3d_getint(data, &j);
                    data = _m3d_findarg(data);
                    m3d->face[m3d->numface + i].vertex[2] = m3d->numvertex + j;
                    m3d->face[m3d->numface + i].texcoord[2] = m3d->numtmap + j;
                    data = _m3d_getint(data, &j);
                    data = _m3d_findarg(data);
                    m3d->face[m3d->numface + i].vertex[1] = m3d->numvertex + j;
                    m3d->face[m3d->numface + i].texcoord[1] = m3d->numtmap + j;
                    data = _m3d_getint(data, &j); m3d->face[m3d->numface + i].normal[0] = m3d->numvertex + nv + j;
                    data = _m3d_findarg(data);
                    data = _m3d_getint(data, &j); m3d->face[m3d->numface + i].normal[2] = m3d->numvertex + nv + j;
                    data = _m3d_findarg(data);
                    data = _m3d_getint(data, &j); m3d->face[m3d->numface + i].normal[1] = m3d->numvertex + nv + j;
                    data = ms3d_nextline(data);
                }
                m3d->numvertex += nv + nn;
                m3d->numtmap += nv;
                m3d->numface += nf;
            }
        } else
        if(!memcmp(s, "Materials:", 10)) {
            m3d->material = (m3dm_t*)realloc(m3d->material, (m3d->nummaterial + num) * sizeof(m3dm_t));
            if(!m3d->material) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
            memset(&m3d->material[m3d->nummaterial], 0, num * sizeof(m3dm_t));
            for(n = 0; n < num; n++) {
                /* first line: name */
                for(e = ++data; *e && *e != '\"'; e++);
                *e++ = 0;
                s = _m3d_safestr(data, 0);
                data = ms3d_nextline(e);
                m = &m3d->material[m3d->nummaterial++];
                m->name = s;
                m->numprop = 0;
                m->prop = (m3dp_t*)malloc(8 * sizeof(m3dp_t));
                if(!m->prop) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                memset(m->prop, 0, 8 * sizeof(m3dp_t));
                /* next 4 lines: colors */
                for(i = 0; i < 4; i++) {
                    r = g = b = a = 0.0f;
                    data = _m3d_getfloat(data, &r);
                    data = _m3d_getfloat(data, &g);
                    data = _m3d_getfloat(data, &b);
                    data = _m3d_getfloat(data, &a);
                    data = ms3d_nextline(data);
                    if(a > 0.0) {
                        m->prop[m->numprop].type = mt[i];
                        m->prop[m->numprop].value.color =
                            ((uint32_t)(a*255.0) << 24L) |
                            ((uint32_t)(b*255.0) << 16L) |
                            ((uint32_t)(g*255.0) <<  8L) |
                            ((uint32_t)(r*255.0) <<  0L);
                        m->numprop++;
                    }
                }
                /* next: shininess value */
                m->prop[m->numprop].type = m3dp_Ns;
                data = _m3d_getfloat(data, &m->prop[m->numprop++].value.fnum);
                data = ms3d_nextline(data);
                /* next: transparency, we don't use it, we have alpha in colors */
                data = ms3d_nextline(data);
                /* next: diffuse texture map */
                if(data[1] != '\"') {
                    for(e = ++data; *e && *e != '\"'; e++);
                    for(d = e; d > data && *d != '.'; d--);
                    *d = 0;
                    for(s = e; s[-1] != '\"' && s[-1] != '\\' && s[-1] != '/'; s--);
                    m->prop[m->numprop].type = m3dp_map_Kd;
                    e = s;
                    s = _m3d_safestr(s, 0);
                    for(i = 0, l = -1U; i < m3d->numtexture; i++) {
                        if(!strcmp(s, m3d->texture[i].name)) { l = i; free(s); break; }
                    }
                    if(l == -1U) {
                        if(storeinline) {
                            path = bn = (char*)malloc(strlen(e) + 5);
                            strcpy(path, e);
                            bn += strlen(e);
                            strcpy(bn, ".png");
                            if(!(readfile(path, &l))) {
                                strcpy(bn, ".PNG");
                                if(!(readfile(path, &l))) {
                                    for(l = strlen(path)-1; l > 0; l--)
                                        if(path[l] >= 'A' && path[l] <= 'Z') path[l] += 'a'-'A';
                                    readfile(path, &l);
                                }
                            }
                            if(l) {
                                if(verbose > 1) printf("  Inlining '%s'\n", s);
                            } else
                                if(verbose > 2) printf("  Texture '%s' not found\n", path);
                            free(path);
                        }
                        l = m3d->numtexture++;
                        m3d->texture = (m3dtx_t*)realloc(m3d->texture, m3d->numtexture * sizeof(m3dtx_t));
                        if(!m3d->texture) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                        m3d->texture[l].name = s;
                        m3d->texture[l].w = m3d->texture[l].h = 0;
                        m3d->texture[l].d = NULL;
                    }
                    *d = '.';
                    m->prop[m->numprop++].value.textureid = l;
                }
                data = ms3d_nextline(data);
                /* next: alpha texture map, we don't use it, we have alpha in diffuse texture */
                data = ms3d_nextline(data);
            }
        } else
        if(!memcmp(s, "Bones:", 6)) {
            m3d->bone = (m3db_t*)realloc(m3d->bone, (m3d->numbone + num) * sizeof(m3db_t));
            if(!m3d->bone) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
            memset(&m3d->bone[m3d->numbone], 0, num * sizeof(m3db_t));
            m3d->skin = (m3ds_t*)realloc(m3d->skin, (m3d->numskin + num) * sizeof(m3ds_t));
            if(!m3d->skin) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
            memset(&m3d->skin[m3d->numskin], 0, num * sizeof(m3ds_t));
            for(n = 0; n < num; n++) {
                m3d->skin[m3d->numskin + n].boneid[0] = m3d->numbone + n;
                m3d->skin[m3d->numskin + n].weight[0] = 1.0;
                for(i = 1; i < M3D_NUMBONE; i++) m3d->skin[m3d->numskin + n].boneid[i] = -1U;
                m3d->bone[m3d->numbone + n].parent = -1U;
                /* first line: name */
                for(e = ++data; *e && *e != '\"'; e++);
                *e++ = 0;
                m3d->bone[m3d->numbone + n].name = _m3d_safestr(data, 0);
                data = ms3d_nextline(e);
                /* second line: parent */
                for(e = ++data; *e && *e != '\"'; e++);
                *e++ = 0;
                bn = _m3d_safestr(data, 0);
                if(bn) {
                    /* find parent index by name */
                    for(i = 0; i < n; i++)
                        if(!strcmp(m3d->bone[m3d->numbone + i].name, bn)) {
                            m3d->bone[m3d->numbone + n].parent = i;
                            break;
                        }
                    free(bn);
                }
                data = ms3d_nextline(e);
                /* third line bind-pose: (flags) (x) (y) (z) (roll) (pitch) (yaw) */
                m3d->vertex = (m3dv_t*)realloc(m3d->vertex, (m3d->numvertex + 2) * sizeof(m3dv_t));
                if(!m3d->vertex) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                memset(&m3d->vertex[m3d->numvertex], 0, 2 * sizeof(m3dv_t));
                m3d->vertex[m3d->numvertex + 0].skinid = -1U;
                m3d->vertex[m3d->numvertex + 1].skinid = -2U;
                m3d->vertex[m3d->numvertex + 1].type = VT_QUATERN;
                data = _m3d_findarg(data);
                data = _m3d_getfloat(data, &m3d->vertex[m3d->numvertex].x);
                data = _m3d_getfloat(data, &m3d->vertex[m3d->numvertex].y);
                data = _m3d_getfloat(data, &m3d->vertex[m3d->numvertex].z);
                data = _m3d_getfloat(data, &r);
                data = _m3d_getfloat(data, &g);
                data = _m3d_getfloat(data, &y);
                _m3d_euler_to_quat(r, g, y, &m3d->vertex[m3d->numvertex + 1]);
                data = ms3d_nextline(data);
                m3d->bone[m3d->numbone + n].pos = m3d->numvertex;
                m3d->bone[m3d->numbone + n].ori = m3d->numvertex + 1;
                m3d->numvertex += 2;
                /* fourth line: number of animation keyframes (position) */
                data = _m3d_getint(data, &nv);
                data = ms3d_nextline(data);
                m3d->vertex = (m3dv_t*)realloc(m3d->vertex, (m3d->numvertex + nv) * sizeof(m3dv_t));
                if(!m3d->vertex) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                memset(&m3d->vertex[m3d->numvertex], 0, nv * sizeof(m3dv_t));
                for(i = 0; i < nv; i++) {
                    m3d->vertex[m3d->numvertex + i].skinid = -1U;
                    data = _m3d_getfloat(data, &t);
                    data = _m3d_getfloat(data, &m3d->vertex[m3d->numvertex + i].x);
                    data = _m3d_getfloat(data, &m3d->vertex[m3d->numvertex + i].y);
                    data = _m3d_getfloat(data, &m3d->vertex[m3d->numvertex + i].z);
                    ms3d_addframe(m3d, (uint32_t)(t*1000.0), m3d->numbone + n, m3d->numvertex + i, m3d->bone[m3d->numbone + n].ori);
                    data = ms3d_nextline(data);
                }
                m3d->numvertex += nv;
                /* next: number of animation keyframes (rotations) */
                data = _m3d_getint(data, &nv);
                data = ms3d_nextline(data);
                m3d->vertex = (m3dv_t*)realloc(m3d->vertex, (m3d->numvertex + nv) * sizeof(m3dv_t));
                if(!m3d->vertex) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                memset(&m3d->vertex[m3d->numvertex], 0, nv * sizeof(m3dv_t));
                for(i = 0; i < nv; i++) {
                    m3d->vertex[m3d->numvertex + i].skinid = -2U;
                    m3d->vertex[m3d->numvertex + i].type = VT_QUATERN;
                    data = _m3d_getfloat(data, &t);
                    data = _m3d_getfloat(data, &x);
                    data = _m3d_getfloat(data, &b);
                    data = _m3d_getfloat(data, &z);
                    _m3d_euler_to_quat(r + x, g + b, y + z, &m3d->vertex[m3d->numvertex + i]);
                    ms3d_addframe(m3d, (uint32_t)(t*1000.0), m3d->numbone + n, m3d->bone[m3d->numbone + n].pos, m3d->numvertex + i);
                    data = ms3d_nextline(data);
                }
                m3d->numvertex += nv;
            }
            m3d->numbone += num;
            m3d->numskin += num;
        } else
        if(verbose > 2) {
            for(e = s; *e && *e != ':' && *e != ' '; e++);
            if(*e) { *e = 0; printf("  Unknown chunk: '%s'\n", s); *e = ' '; }
        }
    }
    return m3d;
}

#if 0
/* MAYBE implement this. For now, binary MS3D supported via assimp */
/**
 * Load a model and convert it's structures into a Model 3D in-memory format
 */
m3d_t *ms3d_bin_load(unsigned char *data, unsigned int size)
{
    m3d_t *m3d;
    int mi, mt[4] = { m3dp_Ka, m3dp_Kd, m3dp_Ks, m3dp_Ke };

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

    return m3d;
}
#endif
