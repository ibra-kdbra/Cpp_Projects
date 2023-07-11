/*
 * m3dconv/objnurbs.h
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
 * @brief simple 3D model to M3D converter OBJ (with NURBS) importer
 * https://gitlab.com/bztsrc/model3d
 *
 */

typedef struct {
    int type;
    unsigned int num;
    uint32_t *data;
} csmod_t;

/**
 * Parse material file
 */
void mtl_load(char *fn, m3d_t *m3d)
{
    unsigned int siz, i, j, k, l;
    char *s, *e, *data, *path, *bn;
    float r,g,b,a;
    m3dm_t *m;

    storeinline = 0;
    data = (char*)readfile(fn, &siz);
    storeinline = doinline;
    if(data) {
        if(verbose > 2) printf("  Material library '%s'\n", fn);
        while(*data) {
            while(*data && (*data == ' ' || *data == '\t' || *data == '\r' || *data == '\n')) data++;
            if(!*data) break;
            /* comments */
            if(*data == '#') { data = _m3d_findnl(data); continue; }
            /* material */
            if(!memcmp(data, "newmtl", 6)) {
                m = NULL;
                data = _m3d_findarg(data + 6);
                if(!*data || *data == '\r' || *data == '\n') goto skip;
                s = _m3d_safestr(data, 0);
                if(!s || !*s) goto skip;
                for(j = 0, i = -1U; j < m3d->nummaterial; j++)
                    if(!strcmp(s, m3d->material[j].name)) { i = j; free(s); break; }
                if(i == -1U) {
                    i = m3d->nummaterial++;
                    m3d->material = (m3dm_t*)realloc(m3d->material, m3d->nummaterial * sizeof(m3dm_t));
                    if(!m3d->material) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                    m = &m3d->material[i];
                    m->name = s;
                    m->numprop = 0;
                    m->prop = NULL;
                } else
                    m = &m3d->material[i];
            }
            if(m) {
                k = i = 256;
                if(!memcmp(data, "map_", 4)) {
                    k = m3dpf_map;
                    data += 4;
                }
                for(j = 0; j < sizeof(m3d_propertytypes)/sizeof(m3d_propertytypes[0]); j++)
                    if(!memcmp(data, m3d_propertytypes[j].key, strlen(m3d_propertytypes[j].key))) {
                        i = m3d_propertytypes[j].id;
                        if(k != m3dpf_map) k = m3d_propertytypes[j].format;
                        break;
                    }
                if(i != 256 && k != 256) {
                    data = _m3d_findarg(data);
                    if(!*data || *data == '\r' || *data == '\n') goto skip;
                    i += (k == m3dpf_map && i < 128 ? 128 : 0);
                    for(l = 0, j = -1U; l < m->numprop; l++)
                        if(m->prop[l].type == i) { j = l; break; }
                    if(j == -1U) {
                        j = m->numprop++;
                        m->prop = (m3dp_t*)realloc(m->prop, m->numprop * sizeof(m3dp_t));
                        if(!m->prop) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                        m->prop[j].type = i;
                    }
                    switch(k) {
                        case m3dpf_color:
                            r = g = b = a = 0.0f;
                            data = _m3d_getfloat(data, &r);
                            data = _m3d_getfloat(data, &g);
                            data = _m3d_getfloat(data, &b);
                            if(*data && *data != '\r' && *data != '\n')
                                data = _m3d_getfloat(data, &a);
                            else
                                a = 1.0f;
                            m->prop[j].value.color = ((uint8_t)(a*255) << 24L) |
                                ((uint8_t)(b*255) << 16L) |
                                ((uint8_t)(g*255) <<  8L) |
                                ((uint8_t)(r*255) <<  0L);
                        break;
                        case m3dpf_uint8:
                        case m3dpf_uint16:
                        case m3dpf_uint32: data = _m3d_getint(data, &m->prop[j].value.num); break;
                        case m3dpf_float:  data = _m3d_getfloat(data, &m->prop[j].value.fnum); break;
                        case m3dpf_map:
                            s = e = _m3d_findnl(data);
                            while(e > data && (e[-1] == ' ' || e[-1] == '\t' || e[-1] == '\r' || e[-1] == '\n')) e--;
                            for(i = 0; s > data && s[-1] != ' ' && s[-1] != '/' && s[-1] != '\\'; s--)
                                if(!i && *s == '.') { i = 1; *s = 0; }
                            data = e;
                            e = s;
                            s = _m3d_safestr(s, 0);
                            if(!s || !*s) goto skip;
                            for(i = 0, l = -1U; i < m3d->numtexture; i++) {
                                if(!strcmp(s, m3d->texture[i].name)) { l = i; free(s); break; }
                            }
                            if(l == -1U) {
                                if(storeinline) {
                                    path = (char*)malloc(strlen(fn) + strlen(e) + 5);
                                    strcpy(path, fn);
                                    bn = strrchr(path, '/');
                                    if(!bn) bn = strrchr(path, '\\');
                                    if(!bn) bn = path; else bn++;
                                    strcpy(bn, e);
                                    bn += strlen(e);
                                    strcpy(bn, ".png");
                                    if(!readfile(path, &l)) {
                                        strcpy(bn, ".PNG");
                                        if(!readfile(path, &l)) {
                                            for(l = strlen(path)-1; l > 0 && path[l]!='/' && path[l]!='\\'; l--)
                                                if(path[l] >= 'A' && path[l] <= 'Z') path[l] += 'a'-'A';
                                            readfile(path, &l);
                                        }
                                    }
                                    if(l && verbose>1) printf("  Inlining '%s'\n", s);
                                }
                                l = m3d->numtexture++;
                                m3d->texture = (m3dtx_t*)realloc(m3d->texture, m3d->numtexture * sizeof(m3dtx_t));
                                if(!m3d->texture) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                                m3d->texture[l].name = s;
                                m3d->texture[l].w = m3d->texture[l].h = 0;
                                m3d->texture[l].d = NULL;
                            }
                            m->prop[j].value.textureid = l;
                        break;
                    }
                }
            }
skip:       data = _m3d_findnl(data);
        }
    }
}

/**
 * Load a model and convert it's structures into a Model 3D in-memory format
 */
m3d_t *obj_load(char *data, char *fn)
{
    unsigned int i, j, sidx = 0, cidx = 0, numv = 0, numn = 0, nump = 0, numm = 0, numc, numpu, numpv, numsp, mi = -1U;
    unsigned int boneid = -1U, maxv = 0, maxn = 0, maxp = 0, maxuv = 0, maxl = 0;
    uint32_t cstype = 0, degu, degv, rangeu, rangev, csidx = 0, *sp = NULL, *ctrl = NULL, *cidxs = NULL, *sidxs = NULL;
    int idx;
    char *s, *an, *bn, *path, name[6];
    float r,g,b,a, ures, vres, maxlen, maxdist, maxangle, *paru = NULL, *parv = NULL;
    m3d_t *m3d;
    m3dv_t *v, *vertex = NULL, *normal = NULL, *param = NULL;
    m3dh_t *h;
    csmod_t *mod = NULL;

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
        while(*data && (*data == ' ' || *data == '\t' || *data == '\r' || *data == '\n')) data++;
        if(!*data) break;

        /* comments */
        if(*data == '#') { data = _m3d_findnl(data); continue; }

        /* annotation label, not in the spec, but I needed it. This is the only statement I've added
         * "a (vertexid) (utf-8 string without control characters)"
         */
        if(data[0] == 'a' && data[1] == ' ') {
            data = _m3d_findarg(data + 1);
            if(!*data || *data == '\r' || *data == '\n') goto skip;
            data = _m3d_getint(data, (unsigned int*)&idx);
            if(idx > 0) idx--; else idx = numv - idx;
            data = _m3d_findarg(data);
            if(!*data || *data == '\r' || *data == '\n') goto skip;
            s = _m3d_safestr(data, 2);
            if(!s || !*s) goto skip;
            i = m3d->numlabel++;
            if(m3d->numlabel >= maxl) {
                maxl += 256;
                m3d->label = (m3dl_t*)realloc(m3d->label, maxl * sizeof(m3dl_t));
                if(!m3d->label) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
            }
            m3d->label[i].name = NULL;
            m3d->label[i].lang = NULL;
            m3d->label[i].color = 0;
            m3d->label[i].vertexid = idx + m3d->numvertex;
            m3d->label[i].text = s;
        } else

        /* object name and groups */
        if(data[1] == ' ' && (data[0] == 'o' || data[0] == 'g')) {
            s = data;
            data = _m3d_findarg(data + 1);
            if(!*data || *data == '\r' || *data == '\n') goto skip;
            /* there can be several "o " lines, but we only use the first as model name */
            if(s[0] == 'o' && !m3d->name) m3d->name = _m3d_safestr(data, 2);
            /* create a bone for each group */
            boneid = -1U;
            an = _m3d_safestr(data, 0);
            if(an) {
                for(i = 0; i < m3d->numbone; i++)
                    if(!strcmp(an, m3d->bone[i].name)) { boneid = i; break; }
                if(boneid == -1U) {
                    boneid = m3d->numbone++;
                    m3d->bone = (m3db_t*)realloc(m3d->bone, m3d->numbone * sizeof(m3db_t));
                    if(!m3d->bone) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                    memset(&m3d->bone[boneid], 0, sizeof(m3db_t));
                    m3d->bone[boneid].parent = (M3D_INDEX)-1U;
                    m3d->bone[boneid].name = an;
                    m3d->bone[boneid].pos = 0;
                    m3d->bone[boneid].ori = 1;
                } else
                    free(an);
            }
            csidx = 0;
        } else

        /* material library */
        if(!memcmp(data, "mtllib", 6)) {
            data = _m3d_findarg(data + 6);
            if(!*data || *data == '\r' || *data == '\n') goto skip;
            s = bn = _m3d_findnl(data);
            while(bn > data && (bn[-1] == ' ' || bn[-1] == '\t' || bn[-1] == '\r' || bn[-1] == '\n')) bn--;
            while(s > data && s[-1] != '/' && s[-1] != '\\') s--;
            *bn++ = 0;
            data = bn;
            path = (char*)malloc(strlen(fn) + strlen(s) + 5);
            if(!path) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
            strcpy(path, fn);
            bn = strrchr(path, '/');
            if(!bn) bn = strrchr(path, '\\');
            if(!bn) bn = path; else bn++;
            strcpy(bn, s);
            mtl_load(path, m3d);
            free(path);
            continue;
        } else

        /* use material */
        if(!memcmp(data, "usemtl", 6)) {
            data = _m3d_findarg(data + 6);
            if(!*data || *data == '\r' || *data == '\n') goto skip;
            mi = (M3D_INDEX)-1U;
            if(*data && *data != '\r' && *data != '\n') {
                s = _m3d_safestr(data, 0);
                if(!s || !*s) goto skip;
                for(i = 0; i < m3d->nummaterial; i++)
                    if(!strcmp(s, m3d->material[i].name)) {
                        mi = (M3D_INDEX)i;
                        break;
                    }
                free(s);
            }
        } else

        /* vertex lists */
        if(data[0] == 'v' && (data[1] == ' ' || (data[2] == ' ' && (data[1] == 'n' || data[1] == 'p')))) {
            s = data + 1;
            data = _m3d_findarg(data);
            if(!*data || *data == '\r' || *data == '\n') goto skip;
            v = NULL;
            switch(*s) {
                case 'n':
                    i = numn++;
                    if(numn >= maxn) {
                        maxn += 1024;
                        normal = (m3dv_t*)realloc(normal, maxn * sizeof(m3dv_t));
                        if(!normal) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                    }
                    v = &normal[i];
                break;
                case 'p':
                    i = nump++;
                    if(nump >= maxp) {
                        maxp += 1024;
                        param = (m3dv_t*)realloc(param, maxp * sizeof(m3dv_t));
                        if(!param) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                    }
                    v = &param[i];
                break;
                default:
                    i = numv++;
                    if(numv >= maxv) {
                        maxv += 1024;
                        vertex = (m3dv_t*)realloc(vertex, maxv * sizeof(m3dv_t));
                        if(!vertex) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                    }
                    v = &vertex[i];
                break;
            }
            if(!v) goto skip;
            memset(v, 0, sizeof(m3dv_t));
            if(*s == 'n') v->type = VT_NORMAL;
            v->skinid = (M3D_INDEX)-1U;
            v->w = (M3D_FLOAT)1.0;
            data = _m3d_getfloat(data, &v->x);
            if(*data && *data != '\r' && *data != '\n') {
                data = _m3d_getfloat(data, &v->y);
                if(*data && *data != '\r' && *data != '\n') {
                    data = _m3d_getfloat(data, &v->z);
                    if((*s == ' ' || *s == 'p') && *data && *data != '\r' && *data != '\n') {
                        data = _m3d_getfloat(data, &v->w);
                        if(*s == ' ' && *data && *data != '\r' && *data != '\n') {
                            r = g = b = a = 0.0f;
                            data = _m3d_getfloat(data, &r);
                            data = _m3d_getfloat(data, &g);
                            if(!*data || *data == '\r' || *data == '\n') {
                                a = 1.0f; b = g; g = r; r = v->w;
                                v->w = (M3D_FLOAT)1.0;
                            } else {
                                data = _m3d_getfloat(data, &b);
                                if(*data && *data != '\r' && *data != '\n')
                                    data = _m3d_getfloat(data, &a);
                                else
                                    a = 1.0f;
                            }
                            v->color = ((uint8_t)(a*255) << 24L) |
                                ((uint8_t)(b*255) << 16L) |
                                ((uint8_t)(g*255) <<  8L) |
                                ((uint8_t)(r*255) <<  0L);
                        }
                    }
                }
            }
        } else

        /* texture map lists */
        if(data[0] == 'v' && data[1] == 't' && data[2] == ' ') {
            data = _m3d_findarg(data + 2);
            if(!*data || *data == '\r' || *data == '\n') goto skip;
            i = m3d->numtmap++;
            if(m3d->numtmap >= maxuv) {
                maxuv += 1024;
                m3d->tmap = (m3dti_t*)realloc(m3d->tmap, maxuv * sizeof(m3dti_t));
                if(!m3d->tmap) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
            }
            m3d->tmap[i].u = m3d->tmap[i].v = (M3D_FLOAT)0.0;
            data = _m3d_getfloat(data, &m3d->tmap[i].u);
            if(*data && *data != '\r' && *data != '\n')
                data = _m3d_getfloat(data, &m3d->tmap[i].v);
        } else

        /* mesh face */
        if(data[0] == 'f' && data[1] == ' ') {
            data = _m3d_findarg(data + 1);
            if(!*data || *data == '\r' || *data == '\n') goto skip;

            bn = _m3d_findnl(data);
            i = 3 * ((int)(bn - data) / 2) * sizeof(uint32_t);
            ctrl = (uint32_t*)realloc(ctrl, i);
            if(!ctrl) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
            memset(ctrl, 255, i);
            /* get the polygon */
            for(numc = 0; *data && *data != '\r' && *data != '\n'; numc += 3) {
                data = _m3d_getint(data, (unsigned int*)&idx);
                if(idx > 0) idx--; else idx = numv - idx;
                ctrl[numc] = idx + m3d->numvertex;
                if(*data == '/') {
                    data++;
                    if(*data != '/') {
                        data = _m3d_getint(data, (unsigned int*)&idx);
                        if(idx > 0) idx--; else idx = m3d->numtmap - idx;
                        ctrl[numc + 1] = idx;
                    }
                    if(*data == '/') {
                        data = _m3d_getint(data + 1, (unsigned int*)&idx);
                        if(idx > 0) idx--; else idx = numn - idx;
                        ctrl[numc + 2] = idx + numv + m3d->numvertex;
                    }
                }
                data = _m3d_findarg(data);
            }
            if(numc < 9) goto skip;
            /* triangulate, assuming CCW */
            numm = (numc / 3) - 2;
            i = m3d->numface; m3d->numface += numm;
            m3d->face = (m3df_t*)realloc(m3d->face, m3d->numface * sizeof(m3df_t));
            if(!m3d->face) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
            memset(&m3d->face[i], 255, sizeof(m3df_t)); /* set all index to -1 by default */
            for(j = 6; j < numc; j += 3, i++) {
                m3d->face[i].materialid = mi;
                m3d->face[i].vertex[0] = (M3D_INDEX)ctrl[0];
                m3d->face[i].texcoord[0] = (M3D_INDEX)ctrl[1];
                m3d->face[i].normal[0] = (M3D_INDEX)ctrl[2];
                m3d->face[i].vertex[1] = (M3D_INDEX)ctrl[j - 3];
                m3d->face[i].texcoord[1] = (M3D_INDEX)ctrl[j - 2];
                m3d->face[i].normal[1] = (M3D_INDEX)ctrl[j - 1];
                m3d->face[i].vertex[2] = (M3D_INDEX)ctrl[j + 0];
                m3d->face[i].texcoord[2] = (M3D_INDEX)ctrl[j + 1];
                m3d->face[i].normal[2] = (M3D_INDEX)ctrl[j + 2];
            }
        } else
        if(!domesh) {
            /* curves and surfaces */
            if(!memcmp(data, "cstype", 6)) {
                data = _m3d_findarg(data + 6);
                if(!memcmp(data, "rat", 3)) data = _m3d_findarg(data);
                if(!*data || *data == '\r' || *data == '\n') goto skip;
                cstype = 0;
                if(!memcmp(data, "bezier", 6)) cstype = 1; else
                if(!memcmp(data, "bspline", 7)) cstype = 4; else {
                    if(verbose > 1) {
                        for(s = data; *s && *s != '\r' && *s != '\n'; s++);
                        *s = 0;
                        printf("  Unknown curve / surface type '%s'\n", data);
                        data = s + 1;
                    }
                    while(*data && memcmp(data, "end", 3)) data++;
                    goto skip;
                }
                degu = degv = numc = numpv = numpu = numsp = numm = 0;
                rangeu = rangev = -1U;
                ures = vres = maxlen = maxdist = maxangle = 1.0f;
            } else
            if(cstype) {
                /* arguments for curves / surfaces */
                if(!memcmp(data, "deg", 3)) {
                    data = _m3d_findarg(data + 3);
                    if(!*data || *data == '\r' || *data == '\n') goto skip;
                    data = _m3d_getint(data, &degu);
                    if(*data && *data != '\r' && *data != '\n')
                        data = _m3d_getint(data, &degv);
                    else
                        degv = degu;
                } else
                if((data[0] == 'c' && data[1] == 's') && !memcmp(data + 1, "tech", 4)) {
                    data = _m3d_findarg(data + 5);
                    if(!*data || *data == '\r' || *data == '\n') goto skip;
                    if(!memcmp(data, "cparm", 5)) {
                        s = data + 5;
                        data = _m3d_findarg(data + 5);
                        if(!*data || *data == '\r' || *data == '\n') goto skip;
                        data = _m3d_getfloat(data, &ures);
                        if(*s == 'b' && *data && *data != '\r' && *data != '\n')
                            data = _m3d_getfloat(data, &vres);
                        else
                            vres = ures;
                    } else
                    if(!memcmp(data, "cspace", 6)) {
                        data = _m3d_findarg(data + 6);
                        if(!*data || *data == '\r' || *data == '\n') goto skip;
                        data = _m3d_getfloat(data, &maxlen);
                    } else
                    if(!memcmp(data, "curv", 4)) {
                        data = _m3d_findarg(data + 4);
                        if(!*data || *data == '\r' || *data == '\n') goto skip;
                        data = _m3d_getfloat(data, &maxdist);
                        if(*data && *data != '\r' && *data != '\n')
                            data = _m3d_getfloat(data, &maxangle);
                    }
                } else
                if(!memcmp(data, "parm", 4)) {
                    data = _m3d_findarg(data + 4);
                    if(!*data || *data == '\r' || *data == '\n') goto skip;
                    bn = _m3d_findnl(data);
                    s = data;
                    data = _m3d_findarg(data);
                    if(!*data || *data == '\r' || *data == '\n') goto skip;
                    if(*s != 'v') {
                        paru = (float*)realloc(paru, ((int)(bn - data) / 2) * sizeof(float));
                        if(!paru) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                        for(numpu = 0; *data && *data != '\r' && *data != '\n'; numpu++)
                            data = _m3d_getfloat(data, &paru[numpu]);
                    } else {
                        parv = (float*)realloc(parv, ((int)(bn - data) / 2) * sizeof(float));
                        if(!parv) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                        for(numpv = 0; *data && *data != '\r' && *data != '\n'; numpv++)
                            data = _m3d_getfloat(data, &parv[numpv]);
                    }
                } else
                if(!memcmp(data, "sp", 2)) {
                    data = _m3d_findarg(data + 2);
                    bn = _m3d_findnl(data);
                    sp = (uint32_t*)realloc(sp, ((int)(bn - data) / 2) * sizeof(uint32_t));
                    if(!sp) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                    for(numsp = 0; *data && *data != '\r' && *data != '\n'; numsp++) {
                        data = _m3d_getint(data, (unsigned int*)&idx);
                        if(idx > 0) idx--; else idx = nump - idx;
                        sp[numsp] = idx + numv + numn;
                        data = _m3d_findarg(data);
                    }
                } else
                if(!memcmp(data, "trim", 4) || !memcmp(data, "hole", 4) || !memcmp(data, "scrv", 4)) {
                    i = numm++;
                    mod = (csmod_t*)realloc(mod, numm * sizeof(uint32_t));
                    if(!mod) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                    switch(*data) {
                        case 't': mod[i].type = m3dc_trim; break;
                        case 'h': mod[i].type = m3dc_hole; break;
                        case 's': mod[i].type = m3dc_scrv; break;
                    }
                    mod[i].num = 0;
                    s = data;
                    data = _m3d_findarg(data + 4);
                    bn = _m3d_findnl(data);
                    mod[i].data = (uint32_t*)malloc(((int)(bn - data) / 6) * 2 * sizeof(uint32_t));
                    if(!mod[i].data) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                    for(mod[i].num = 0; *data && *data != '\r' && *data != '\n';) {
                        j = m3d->numtmap++;
                        if(m3d->numtmap >= maxuv) {
                            maxuv += 1024;
                            m3d->tmap = (m3dti_t*)realloc(m3d->tmap, maxuv * sizeof(m3dti_t));
                            if(!m3d->tmap) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                        }
                        m3d->tmap[j].u = m3d->tmap[j].v = (M3D_FLOAT)0.0;
                        data = _m3d_getfloat(data, &m3d->tmap[j].u);
                        if(*data && *data != '\r' && *data != '\n') {
                            data = _m3d_getfloat(data, &m3d->tmap[j].v);
                            mod[i].data[mod[i].num++] = j;
                            if(*data && *data != '\r' && *data != '\n' && !cidxs) {
                                data = _m3d_getint(data, (unsigned int*)&idx);
                                if(idx > 0) idx--; else idx = cidx - idx;
                                mod[i].data[mod[i].num++] = cidxs[idx];
                                data = _m3d_findarg(data);
                            }
                        }
                    }
                } else
                /* control points */
                if(!memcmp(data, "curv2", 5)) {
                    cstype++;
                    data = _m3d_findarg(data + 5);
                    bn = _m3d_findnl(data);
                    ctrl = (uint32_t*)realloc(ctrl, ((int)(bn - data) / 2) * sizeof(uint32_t));
                    if(!ctrl) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                    for(numc = 0; *data && *data != '\r' && *data != '\n'; numc++) {
                        data = _m3d_getint(data, (unsigned int*)&idx);
                        if(idx > 0) idx--; else idx = nump - idx;
                        ctrl[numc] = idx + numv + numn + m3d->numvertex;
                        data = _m3d_findarg(data);
                    }
                } else
                if(!memcmp(data, "curv", 4)) {
                    data = _m3d_findarg(data + 4);
                    i = m3d->numtmap++;
                    if(m3d->numtmap >= maxuv) {
                        maxuv += 1024;
                        m3d->tmap = (m3dti_t*)realloc(m3d->tmap, maxuv * sizeof(m3dti_t));
                        if(!m3d->tmap) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                    }
                    m3d->tmap[i].u = m3d->tmap[i].v = (M3D_FLOAT)0.0;
                    data = _m3d_getfloat(data, &m3d->tmap[i].u);
                    if(*data && *data != '\r' && *data != '\n')
                        data = _m3d_getfloat(data, &m3d->tmap[i].v);
                    rangeu = i;
                    bn = _m3d_findnl(data);
                    ctrl = (uint32_t*)realloc(ctrl, ((int)(bn - data) / 2) * sizeof(uint32_t));
                    if(!ctrl) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                    for(numc = 0; *data && *data != '\r' && *data != '\n'; numc++) {
                        data = _m3d_getint(data, (unsigned int*)&idx);
                        if(idx > 0) idx--; else idx = numv - idx;
                        ctrl[numc] = idx;
                        data = _m3d_findarg(data);
                    }
                } else
                if(!memcmp(data, "surf", 4)) {
                    cstype += 2;
                    data = _m3d_findarg(data + 4);
                    i = m3d->numtmap++;
                    if(m3d->numtmap >= maxuv) {
                        maxuv += 1024;
                        m3d->tmap = (m3dti_t*)realloc(m3d->tmap, maxuv * sizeof(m3dti_t));
                        if(!m3d->tmap) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                    }
                    m3d->tmap[i].u = m3d->tmap[i].v = (M3D_FLOAT)0.0;
                    data = _m3d_getfloat(data, &m3d->tmap[i].u);
                    if(*data && *data != '\r' && *data != '\n')
                        data = _m3d_getfloat(data, &m3d->tmap[i].v);
                    rangeu = i;
                    i = m3d->numtmap++;
                    if(m3d->numtmap >= maxuv) {
                        maxuv += 1024;
                        m3d->tmap = (m3dti_t*)realloc(m3d->tmap, maxuv * sizeof(m3dti_t));
                        if(!m3d->tmap) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                    }
                    m3d->tmap[i].u = m3d->tmap[i].v = (M3D_FLOAT)0.0;
                    data = _m3d_getfloat(data, &m3d->tmap[i].u);
                    if(*data && *data != '\r' && *data != '\n')
                        data = _m3d_getfloat(data, &m3d->tmap[i].v);
                    rangev = i;
                    bn = _m3d_findnl(data);
                    ctrl = (uint32_t*)realloc(ctrl, 3 * ((int)(bn - data) / 2) * sizeof(uint32_t));
                    if(!ctrl) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                    for(numc = 0; *data && *data != '\r' && *data != '\n';) {
                        data = _m3d_getint(data, (unsigned int*)&idx);
                        if(idx > 0) idx--; else idx = numv - idx;
                        ctrl[numc++] = idx + m3d->numvertex;
                        if(*data == '/') {
                            data++;
                            if(*data != '/') {
                                data = _m3d_getint(data, (unsigned int*)&idx);
                                if(idx > 0) idx--; else idx = m3d->numtmap - idx;
                                ctrl[numc++] = idx;
                                cstype |= 0x40;
                            }
                            if(*data == '/') {
                                data = _m3d_getint(data + 1, (unsigned int*)&idx);
                                if(idx > 0) idx--; else idx = numn - idx;
                                ctrl[numc++] = idx + numv + m3d->numvertex;
                                cstype |= 0x80;
                            }
                        }
                        data = _m3d_findarg(data);
                    }
                } else
                /* on block end, convert everything into M3D shape commands */
                if(!memcmp(data, "end", 3)) {
                    if(numc && degu) {
                        /* add a new shape if we must */
                        if(!csidx) {
                            i = m3d->numshape++;
                            m3d->shape = (m3dh_t*)realloc(m3d->shape, m3d->numshape * sizeof(m3dh_t));
                            if(!m3d->shape) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                            h = &m3d->shape[i];
                            h->numcmd = 0;
                            h->cmd = NULL;
                            h->group = boneid;
                            sprintf(name, "s%04X", i);
                            h->name = _m3d_safestr((char*)&name, 0);
                        }
                        /* count how many commands we need */
                        j = 1 + numm;
                        if(ures != 1.0f || vres != 1.0f) j++;
                        if(maxlen != 1.0f) j++;
                        if(maxdist != 1.0f || maxangle != 1.0f) j++;
                        if(degu || degv) j++;
                        if(rangeu != -1U || rangev != -1U) j++;
                        if(numpu) j++;
                        if(numpv) j++;
                        i = h->numcmd; h->numcmd += j;
                        /* allocate commands */
                        h->cmd = (m3dc_t*)realloc(h->cmd, h->numcmd * sizeof(m3dc_t));
                        if(!h->cmd) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                        /* add commands */
                        if(ures != 1.0f || vres != 1.0f) {
                            if(ures == vres) {
                                h->cmd[i].type = m3dc_div;
                                h->cmd[i].arg = (uint32_t*)malloc(1 * sizeof(uint32_t));
                                if(!h->cmd[i].arg) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                                memcpy(&h->cmd[i].arg[0], &ures, sizeof(uint32_t));
                            } else {
                                h->cmd[i].type = m3dc_sub;
                                h->cmd[i].arg = (uint32_t*)malloc(2 * sizeof(uint32_t));
                                if(!h->cmd[i].arg) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                                memcpy(&h->cmd[i].arg[0], &ures, sizeof(uint32_t));
                                memcpy(&h->cmd[i].arg[1], &vres, sizeof(uint32_t));
                            }
                            i++;
                        }
                        if(maxlen != 1.0f) {
                            h->cmd[i].type = m3dc_len;
                            h->cmd[i].arg = (uint32_t*)malloc(1 * sizeof(uint32_t));
                            if(!h->cmd[i].arg) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                            memcpy(&h->cmd[i].arg[0], &maxlen, sizeof(uint32_t));
                            i++;
                        }
                        if(maxdist != 1.0f || maxangle != 1.0f) {
                            h->cmd[i].type = m3dc_dist;
                            h->cmd[i].arg = (uint32_t*)malloc(2 * sizeof(uint32_t));
                            if(!h->cmd[i].arg) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                            memcpy(&h->cmd[i].arg[0], &maxdist, sizeof(uint32_t));
                            memcpy(&h->cmd[i].arg[1], &maxangle, sizeof(uint32_t));
                            i++;
                        }
                        if(degu || degv) {
                            if(degu == degv) {
                                h->cmd[i].type = m3dc_degu;
                                h->cmd[i].arg = (uint32_t*)malloc(1 * sizeof(uint32_t));
                                if(!h->cmd[i].arg) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                                h->cmd[i].arg[0] = degu;
                            } else {
                                h->cmd[i].type = m3dc_deg;
                                h->cmd[i].arg = (uint32_t*)malloc(2 * sizeof(uint32_t));
                                if(!h->cmd[i].arg) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                                h->cmd[i].arg[0] = degu;
                                h->cmd[i].arg[1] = degv;
                            }
                            i++;
                        }
                        if(rangeu != -1U || rangev != -1U) {
                            if(rangev == -1U) {
                                h->cmd[i].type = m3dc_rangeu;
                                h->cmd[i].arg = (uint32_t*)malloc(1 * sizeof(uint32_t));
                                if(!h->cmd[i].arg) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                                h->cmd[i].arg[0] = rangeu;
                            } else {
                                h->cmd[i].type = m3dc_range;
                                h->cmd[i].arg = (uint32_t*)malloc(2 * sizeof(uint32_t));
                                if(!h->cmd[i].arg) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                                h->cmd[i].arg[0] = rangeu;
                                h->cmd[i].arg[1] = rangev;
                            }
                            i++;
                        }
                        if(numpu) {
                            h->cmd[i].type = m3dc_paru;
                            h->cmd[i].arg = (uint32_t*)malloc((numpu + 1) * sizeof(uint32_t));
                            if(!h->cmd[i].arg) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                            h->cmd[i].arg[0] = numpu;
                            memcpy(&h->cmd[i].arg[1], &paru, numpu * sizeof(uint32_t));
                            i++;
                        }
                        if(numpv) {
                            h->cmd[i].type = m3dc_parv;
                            h->cmd[i].arg = (uint32_t*)malloc((numpv + 1) * sizeof(uint32_t));
                            if(!h->cmd[i].arg) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                            h->cmd[i].arg[0] = numpv;
                            memcpy(&h->cmd[i].arg[1], &parv, numpv * sizeof(uint32_t));
                            i++;
                        }
                        for(j = 0; j < numm; j++) {
                            h->cmd[i].type = mod[j].type;
                            h->cmd[i].arg = (uint32_t*)malloc((mod[j].num + 1) * sizeof(uint32_t));
                            if(!h->cmd[i].arg) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                            h->cmd[i].arg[0] = mod[j].num;
                            memcpy(&h->cmd[i].arg[1], &mod[j].data, mod[j].num * sizeof(uint32_t));
                            free(mod[j].data);
                            i++;
                        }
                        j = (cstype >> 6) & 3;
                        if((cstype & 0x3F) == 3 || (cstype & 0x3F) == 6) {
                            sidxs = (uint32_t*)realloc(sidxs, (sidx+1) * sizeof(uint32_t));
                            sidxs[sidx] = csidx;
                        } else {
                            cidxs = (uint32_t*)realloc(cidxs, (cidx+1) * sizeof(uint32_t));
                            cidxs[cidx] = csidx;
                        }
                        switch(cstype & 0x3F) {
                            case 1: j = m3dc_bez1; cidx++; break;
                            case 2: j = m3dc_bez2; cidx++; break;
                            case 3: j += m3dc_bez; sidx++; break;
                            case 4: j = m3dc_bsp1; cidx++; break;
                            case 5: j = m3dc_bsp2; cidx++; break;
                            case 6: j += m3dc_nurbs; sidx++; break;
                        }
                        h->cmd[i].type = j;
                        h->cmd[i].arg = (uint32_t*)malloc((numc + 1) * sizeof(uint32_t));
                        if(!h->cmd[i].arg) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                        h->cmd[i].arg[0] = (cstype & 0xC0) == 0xC0 ? numc / 3 : (cstype & 0xC0 ? numc / 2 : numc);
                        memcpy(&h->cmd[i].arg[1], &ctrl, numc * sizeof(uint32_t));
                        csidx++;
                    }
                    cstype = 0;
                }
            } else
            if(data[0] == 'l' && data[1] == ' ') {
                data = _m3d_findarg(data + 1);
                if(!*data || *data == '\r' || *data == '\n') goto skip;
                /* add a new shape if we must */
                if(!csidx) {
                    i = m3d->numshape++;
                    m3d->shape = (m3dh_t*)realloc(m3d->shape, m3d->numshape * sizeof(m3dh_t));
                    if(!m3d->shape) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                    h = &m3d->shape[i];
                    h->numcmd = 0;
                    h->cmd = NULL;
                    h->group = boneid;
                    sprintf(name, "s%04X", i);
                    h->name = _m3d_safestr((char*)&name, 0);
                }
                i = h->numcmd++;
                h->cmd = (m3dc_t*)realloc(h->cmd, h->numcmd * sizeof(m3dc_t));
                if(!h->cmd) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                bn = _m3d_findnl(data);
                h->cmd[i].arg = (uint32_t*)malloc((((int)(bn - data) / 2) + 1) * sizeof(uint32_t));
                if(!h->cmd[i].arg) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                h->cmd[i].type = m3dc_line;
                for(j = 0; *data && *data != '\r' && *data != '\n'; j++) {
                    data = _m3d_getint(data, (unsigned int*)&idx);
                    if(idx > 0) idx--; else idx = numv - idx;
                    h->cmd[i].arg[j + 1] = idx;
                    data = _m3d_findarg(data);
                }
                h->cmd[i].arg[0] = j;
                csidx++;
            } else
            if(!memcmp(data, "con", 3) && h) {
                data = _m3d_findarg(data + 3);
                if(!*data || *data == '\r' || *data == '\n' || !cidxs || !sidxs) goto skip;

                /* allocate command and its arguments */
                i = h->numcmd++;
                h->cmd = (m3dc_t*)realloc(h->cmd, h->numcmd * sizeof(m3dc_t));
                if(!h->cmd) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                h->cmd[i].arg = (uint32_t*)malloc(6 * sizeof(uint32_t));
                if(!h->cmd[i].arg) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                h->cmd[i].type = m3dc_conn;

                /* get arguments */
                data = _m3d_getint(data, (unsigned int*)&idx);
                if(idx > 0) idx--; else idx = sidx - idx;
                h->cmd[i].arg[0] = sidxs[idx];
                data = _m3d_findarg(data);
                if(!*data || *data == '\r' || *data == '\n') { free(h->cmd[i].arg); h->numcmd--; goto skip; }

                j = m3d->numtmap++;
                if(m3d->numtmap >= maxuv) {
                    maxuv += 1024;
                    m3d->tmap = (m3dti_t*)realloc(m3d->tmap, maxuv * sizeof(m3dti_t));
                    if(!m3d->tmap) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                }
                m3d->tmap[j].u = m3d->tmap[j].v = (M3D_FLOAT)0.0;
                data = _m3d_getfloat(data, &m3d->tmap[j].u);
                if(!*data || *data == '\r' || *data == '\n') { free(h->cmd[i].arg); h->numcmd--; goto skip; }
                data = _m3d_getfloat(data, &m3d->tmap[j].v);
                if(!*data || *data == '\r' || *data == '\n') { free(h->cmd[i].arg); h->numcmd--; goto skip; }
                h->cmd[i].arg[1] = j;

                data = _m3d_getint(data, (unsigned int*)&idx);
                if(idx > 0) idx--; else idx = cidx - idx;
                h->cmd[i].arg[2] = cidxs[idx];
                data = _m3d_findarg(data);
                if(!*data || *data == '\r' || *data == '\n') { free(h->cmd[i].arg); h->numcmd--; goto skip; }

                data = _m3d_getint(data, (unsigned int*)&idx);
                if(idx > 0) idx--; else idx = sidx - idx;
                h->cmd[i].arg[3] = sidxs[idx];
                data = _m3d_findarg(data);
                if(!*data || *data == '\r' || *data == '\n') { free(h->cmd[i].arg); h->numcmd--; goto skip; }

                j = m3d->numtmap++;
                if(m3d->numtmap >= maxuv) {
                    maxuv += 1024;
                    m3d->tmap = (m3dti_t*)realloc(m3d->tmap, maxuv * sizeof(m3dti_t));
                    if(!m3d->tmap) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                }
                m3d->tmap[j].u = m3d->tmap[j].v = (M3D_FLOAT)0.0;
                data = _m3d_getfloat(data, &m3d->tmap[j].u);
                if(!*data || *data == '\r' || *data == '\n') { free(h->cmd[i].arg); h->numcmd--; goto skip; }
                data = _m3d_getfloat(data, &m3d->tmap[j].v);
                if(!*data || *data == '\r' || *data == '\n') { free(h->cmd[i].arg); h->numcmd--; goto skip; }
                h->cmd[i].arg[4] = j;

                data = _m3d_getint(data, (unsigned int*)&idx);
                if(idx > 0) idx--; else idx = cidx - idx;
                h->cmd[i].arg[5] = cidxs[idx];
            }
        }
skip:   data = _m3d_findnl(data);
    }
    i = numv + numn + nump;
    if(i) {
        m3d->vertex = (m3dv_t*)realloc(m3d->vertex, (m3d->numvertex + i) * sizeof(m3dv_t));
        if(!m3d->vertex) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
        if(numv) memcpy(&m3d->vertex[m3d->numvertex], vertex, numv * sizeof(m3dv_t));
        if(numn) memcpy(&m3d->vertex[m3d->numvertex + numv], normal, numn * sizeof(m3dv_t));
        if(nump) memcpy(&m3d->vertex[m3d->numvertex + numv + numn], param, nump * sizeof(m3dv_t));
        m3d->numvertex += i;
    }
    if(m3d->numtmap) {
        m3d->tmap = (m3dti_t*)realloc(m3d->tmap, m3d->numtmap * sizeof(m3dti_t));
        if(!m3d->tmap) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
    }
    if(m3d->numlabel) {
        m3d->label = (m3dl_t*)realloc(m3d->label, m3d->numlabel * sizeof(m3dl_t));
        if(!m3d->label) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
    }
    if(vertex) free(vertex);
    if(normal) free(normal);
    if(param) free(param);
    if(cidxs) free(cidxs);
    if(sidxs) free(sidxs);
    if(ctrl) free(ctrl);
    if(paru) free(paru);
    if(parv) free(parv);
    if(mod) free(mod);
    if(sp) free(sp);

    return m3d;
}
