/*
 * m3dconv/step.h
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
 * @brief simple 3D model to M3D converter STEP (ISO-10303-21-4 and ISO-10303-24-2) importer (Work In Progress)
 * https://gitlab.com/bztsrc/model3d
 *
 */

#include <assimp/cimport.h>

/* this code does not need to be fast. Being correct is more important. the standard is seriously fucked up
 * and NOT documented properly. You'll find dead links and password encrypted sites when searching for the spec...
 *
 * I'd like to say thanks to D. Sreeramulu, C.S.P. Rao, National Institute of Technologu, Warangal and
 * Vangipurapu, Naga Malleswari at Andhra University for their algorithm and thesis being the only readable and
 * understandable source on STEP files and extracting geometry.
 *
 * https://en.wikipedia.org/wiki/ISO_10303-21
 * https://www.iso.org/standard/76142.html
 * https://scolarworks.waldenu.edu/cgi/viewcontent.cgi?article=1054&context=ijamt
 * http://shodhganga.inflibnet.ac.in/bitstream/10603/14116/11/11_chapter_3.pdf
 * https://link.springer.com/content/pdf/10.1007%2F978-0-387-35490-3_9.pdf
 */

/* we took 4 bits from 32, that leaves us 256 million unique strings and again that much unique numbers.
 * If that's not enough for your control points, then you're doing the parameterized surfaces thing wrong. */
#define ST_TOKEN(t, i)    (((i) << 4) | (t))
#define ST_TYPE(t)        ((t) & 15)
#define ST_PARAM(t)       ((t) >> 4)
#define ST_ISENTITY(t, n) (ST_TYPE(t) == ST_ID && lines[ST_PARAM(t)] && lines[ST_PARAM(t)][0] > (n))

/* STEP tokens */
#define ST_END     0  /* end of statement */
#define ST_COMMA   1  /* , attribute separator */
#define ST_PAROP   2  /* ( */
#define ST_PARCL   3  /* ) */
#define ST_DOLLAR  4  /* $ */
#define ST_ASTERIX 5  /* * */
#define ST_ID      6  /* entity reference */
#define ST_ENUM    7  /* .enum. */
#define ST_STRING  8  /* string literal */
#define ST_NUM     9  /* number literal */
#define ST_LABEL  10  /* some label for an unrecognized entity, probably function name if followed by ( */
#define ST_FUNC   11  /* recognized entity */
/* list of recognized entitites, their tokens and labels */
#define ST_PRODUCT                                  ST_TOKEN(ST_FUNC, 0)
#define ST_ADVANCED_BREP_SHAPE_REPRESENTATION       ST_TOKEN(ST_FUNC, 1)
#define ST_MANIFOLD_SURFACE_SHAPE_REPRESENTATION    ST_TOKEN(ST_FUNC, 2)
#define ST_FACETED_BREP_SHAPE_REPRESENTATION        ST_TOKEN(ST_FUNC, 3)
#define ST_SHELL_BASED_SURFACE_MODEL                ST_TOKEN(ST_FUNC, 4)
#define ST_MANIFOLD_SOLID_BREP                      ST_TOKEN(ST_FUNC, 5)
#define ST_FACETED_BREP                             ST_TOKEN(ST_FUNC, 6)
#define ST_CLOSED_SHELL                             ST_TOKEN(ST_FUNC, 7)
#define ST_AXIS2_PLACEMENT_3D                       ST_TOKEN(ST_FUNC, 8)
#define ST_CARTESIAN_POINT                          ST_TOKEN(ST_FUNC, 9)
#define ST_DIRECTION                                ST_TOKEN(ST_FUNC,10)
#define ST_VECTOR                                   ST_TOKEN(ST_FUNC,11)
#define ST_FACE_SURFACE                             ST_TOKEN(ST_FUNC,12)
#define ST_ADVANCED_FACE                            ST_TOKEN(ST_FUNC,13)
#define ST_FACE_OUTER_BOUND                         ST_TOKEN(ST_FUNC,14)
#define ST_FACE_BOUND                               ST_TOKEN(ST_FUNC,15)
#define ST_BOUNDED_SURFACE                          ST_TOKEN(ST_FUNC,16)
#define ST_PLANE                                    ST_TOKEN(ST_FUNC,17)
#define ST_CYLINDRICAL_SURFACE                      ST_TOKEN(ST_FUNC,18)
#define ST_SPHERICAL_SURFACE                        ST_TOKEN(ST_FUNC,19)
#define ST_POLY_LOOP                                ST_TOKEN(ST_FUNC,20)
#define ST_EDGE_LOOP                                ST_TOKEN(ST_FUNC,21)
#define ST_ORIENTED_EDGE                            ST_TOKEN(ST_FUNC,22)
#define ST_EDGE_CURVE                               ST_TOKEN(ST_FUNC,23)
#define ST_VERTEX_POINT                             ST_TOKEN(ST_FUNC,24)
#define ST_CIRCLE                                   ST_TOKEN(ST_FUNC,25)
#define ST_LINE                                     ST_TOKEN(ST_FUNC,26)
char *labels[] = { "PRODUCT", "ADVANCED_BREP_SHAPE_REPRESENTATION", "MANIFOLD_SURFACE_SHAPE_REPRESENTATION",
    "FACETED_BREP_SHAPE_REPRESENTATION", "SHELL_BASED_SURFACE_MODEL", "MANIFOLD_SOLID_BREP", "FACETED_BREP",
    "CLOSED_SHELL", "AXIS2_PLACEMENT_3D", "CARTESIAN_POINT", "DIRECTION", "VECTOR", "FACE_SURFACE", "ADVANCED_FACE",
    "FACE_OUTER_BOUND", "FACE_BOUND", "BOUNDED_SURFACE", "PLANE", "CYLINDRICAL_SURFACE", "SPHERICAL_SURFACE",
    "POLY_LOOP", "EDGE_LOOP", "ORIENTED_EDGE", "EDGE_CURVE", "VERTEX_POINT", "CIRCLE", "LINE"};

/* closed shells in STEP file */
typedef struct {
    unsigned int id, type;
    char *name;
    struct aiMatrix4x4 transform;
} steproot_t;
unsigned int numclosedshells = 0;
steproot_t *closedshells = NULL;

/* cache for tokenized lines */
unsigned int numlines = 0;
unsigned int **lines = NULL;
/* cache for unique strings */
unsigned int numstrings = 0, maxstrings = 0;
char **strings = NULL;
/* cache for unique numbers */
unsigned int numnum = 0, maxnum = 0;
float *num = NULL;

/**
 * Dump a STEP entity record recursively
 */
void step_dump(unsigned int id, int lvl, int max)
{
    unsigned int i;

    for(i = 0; (int)i < lvl; i++) printf("  ");
    printf("#%d ", id);
    if(!lines || id >= numlines || !lines[id] || !lines[id][0])
        printf("*not defined*\n");
    else {
        for(i = 1; i < lines[id][0] && lines[id][i] != ST_END; i++)
            switch(ST_TYPE(lines[id][i])) {
                case ST_COMMA: printf(", "); break;
                case ST_PAROP: printf("( "); break;
                case ST_PARCL: printf(") "); break;
                case ST_DOLLAR: printf("$ "); break;
                case ST_ASTERIX: printf("* "); break;
                case ST_ID: printf("#%d ", ST_PARAM(lines[id][i])); break;
                case ST_ENUM: printf("%s ", strings[ST_PARAM(lines[id][i])]); break;
                case ST_STRING: printf("\"%s\" ", strings[ST_PARAM(lines[id][i])]); break;
                case ST_NUM: printf("%g ", num[ST_PARAM(lines[id][i])]); break;
                case ST_LABEL: printf("%s ", strings[ST_PARAM(lines[id][i])]); break;
                case ST_FUNC: printf("%s ", labels[ST_PARAM(lines[id][i])]); break;
            }
        printf("\n");
        if(lvl >= max) return;
        for(i = 1; i < lines[id][0] && lines[id][i] != ST_END; i++)
            if(ST_TYPE(lines[id][i]) == ST_ID)
                step_dump(ST_PARAM(lines[id][i]), lvl + 1, max);
    }
}

/**
 * Convert an axis2_placement_3d into a transformation matrix
 */
void step_placement(unsigned int id, struct aiMatrix4x4 *m)
{
    aiIdentityMatrix4(m);
    /* huh. anybody has the slightest doubt that STEP files are insane? */
    if(lines[id] && lines[id][0] > 9 && lines[id][1] == ST_AXIS2_PLACEMENT_3D &&
        ST_ISENTITY(lines[id][5], 10) &&
        lines[ST_PARAM(lines[id][5])][1] == ST_CARTESIAN_POINT && ST_TYPE(lines[ST_PARAM(lines[id][5])][6]) == ST_NUM &&
        ST_TYPE(lines[ST_PARAM(lines[id][5])][8]) == ST_NUM && ST_TYPE(lines[ST_PARAM(lines[id][5])][10]) == ST_NUM &&
        ST_ISENTITY(lines[id][7], 10) &&
        lines[ST_PARAM(lines[id][7])][1] == ST_DIRECTION && ST_TYPE(lines[ST_PARAM(lines[id][7])][6]) == ST_NUM &&
        ST_TYPE(lines[ST_PARAM(lines[id][7])][8]) == ST_NUM && ST_TYPE(lines[ST_PARAM(lines[id][7])][10]) == ST_NUM &&
        ST_ISENTITY(lines[id][9], 10) &&
        lines[ST_PARAM(lines[id][9])][1] == ST_DIRECTION && ST_TYPE(lines[ST_PARAM(lines[id][9])][6]) == ST_NUM &&
        ST_TYPE(lines[ST_PARAM(lines[id][9])][8]) == ST_NUM && ST_TYPE(lines[ST_PARAM(lines[id][9])][10]) == ST_NUM) {
            /* position */
            m->a4 = num[ST_PARAM(lines[ST_PARAM(lines[id][5])][6])];
            m->b4 = num[ST_PARAM(lines[ST_PARAM(lines[id][5])][8])];
            m->c4 = num[ST_PARAM(lines[ST_PARAM(lines[id][5])][10])];
            /* rotation */
            /* TODO: check if this is actually a transposed rotation matrix */
            /* X */
            m->a1 = num[ST_PARAM(lines[ST_PARAM(lines[id][7])][6])];
            m->a2 = num[ST_PARAM(lines[ST_PARAM(lines[id][7])][8])];
            m->a3 = num[ST_PARAM(lines[ST_PARAM(lines[id][7])][10])];
            /* Z */
            m->c1 = num[ST_PARAM(lines[ST_PARAM(lines[id][9])][6])];
            m->c2 = num[ST_PARAM(lines[ST_PARAM(lines[id][9])][8])];
            m->c3 = num[ST_PARAM(lines[ST_PARAM(lines[id][9])][10])];
            /* Y = X * Z */
            m->b1 = m->a2 * m->c3 - m->a3 * m->c2;
            m->b2 = m->a3 * m->c1 - m->a1 * m->c3;
            m->b3 = m->a1 * m->c2 - m->a2 * m->c1;
            _assimp_fixmat(m);
    }
}

/**
 * Recursively parse face geometry into a shape
 */
m3d_t *step_geom(unsigned int id, unsigned int type, _unused struct aiMatrix4x4 *m, _unused m3dh_t *shape, m3d_t *m3d, unsigned int lvl)
{
/*
    unsigned int i, ok = 0;
*/
    if(lvl > 64) return m3d;
    if(lines[id] && lines[id][0] > 5) {
        step_dump(type, 0, 0);
        step_dump(id, 0, 6);
        printf("\n");
        fprintf(stderr,"m3dconv: parsing STEP geometry is under development.\n");
/*
        if(ST_TYPE(lines[id][1]) == ST_FUNC) {
            switch(lines[id][1]) {
                case ST_FACE_OUTER_BOUND:
                case ST_FACE_BOUND:
                case ST_POLY_LOOP:
                case ST_EDGE_LOOP:
                case ST_ORIENTED_EDGE:
                case ST_EDGE_CURVE:
                    for(i = 5; i < lines[id][0]; i++)
                        if(ST_ISENTITY(lines[id][i], 5))
                            step_geom(ST_PARAM(lines[id][i]), type, m, shape, m3d, lvl + 1);
                    ok = 1;
                break;
                case ST_VERTEX_POINT: ok = 1; break;
            }
        } else
        if(lines[id][1] == ST_PAROP && lines[id][2] == ST_BOUNDED_SURFACE) {
        }
*/
    }
/*
    if(!ok) {
        fprintf(stderr, "m3dconv: unrecognized geometry entity #%d\n", id);
        step_dump(type, lvl, lvl);
        step_dump(id, lvl, lvl + 2);
    }
*/
    return m3d;
}

/**
 * The actual STEP parser
 *
 * We don't need to link with opencascade neither do we need that monstrocity called NIST SCL just to parse a
 * text file for some coordinates... we are only interested in the part 21-4 and 24-2 schemas, and we only need
 * shells and faces, nothing else (maybe annotations perhaps). This function reads STEP tokens from lines[].
 */
m3d_t *step_parse(m3d_t *m3d)
{
    struct aiMatrix4x4 nm, tm;
    unsigned int i, j, k, l, m, n, b, s, t, ni, *shids = NULL;
    char name[6];

    if(!lines) return m3d;

    /* get the closed shell root node(s). There can be more roots in a STEP file... both vertically and horizontally */
    for(i = 0; i < numlines; i++)
        if(lines[i] && lines[i][0] > 8 && ST_TYPE(lines[i][1]) == ST_FUNC && lines[i][2] == ST_PAROP &&
            ST_TYPE(lines[i][3]) == ST_STRING) {
            /* use product name as model name */
            if(!m3d->name && lines[i][1] == ST_PRODUCT)
                    m3d->name = _m3d_safestr(strings[ST_PARAM(lines[i][3])], 2);
            /* get the closed shell with name and transformation matrix */
            if((lines[i][1] == ST_ADVANCED_BREP_SHAPE_REPRESENTATION ||
                lines[i][1] == ST_MANIFOLD_SURFACE_SHAPE_REPRESENTATION ||
                lines[i][1] == ST_FACETED_BREP_SHAPE_REPRESENTATION) &&
                ST_ISENTITY(lines[i][6], 4)) {
                for(m = -1U, ni = t = 0, l = 6; l < 9 && lines[i][l] != ST_PARCL; l++) {
                    k = ST_PARAM(lines[i][l]);
                    if(ST_ISENTITY(lines[i][l], 4) && ST_TYPE(lines[k][1]) == ST_FUNC) {
                        if( lines[k][1] == ST_SHELL_BASED_SURFACE_MODEL ||
                            lines[k][1] == ST_MANIFOLD_SOLID_BREP ||
                            lines[k][1] == ST_FACETED_BREP) {
                                t = lines[k][1];
                                shids = (unsigned int*)realloc(shids, (lines[k][0] - 4) * sizeof(unsigned int));
                                if(!shids) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                                for(j = 5; j < lines[k][0] && lines[k][j] != ST_PARCL; j++)
                                    if(ST_ISENTITY(lines[k][j], 5) &&
                                        ST_TYPE(lines[ST_PARAM(lines[k][j])][3]) == ST_STRING &&
                                        lines[ST_PARAM(lines[k][j])][1] == ST_CLOSED_SHELL)
                                            shids[ni++] = ST_PARAM(lines[k][j]);
                        }
                        if(lines[k][1] == ST_AXIS2_PLACEMENT_3D && ST_TYPE(lines[k][5]) == ST_ID &&
                            ST_TYPE(lines[k][7]) == ST_ID && ST_TYPE(lines[k][9]) == ST_ID)
                                m = k;
                    }
                }
                if(ni) {
                    l = numclosedshells; numclosedshells += ni;
                    closedshells = (steproot_t*)realloc(closedshells, numclosedshells * sizeof(steproot_t));
                    if(!closedshells) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                    for(j = 0; j < ni; j++, l++) {
                        /* add closed shells */
                        closedshells[l].id = shids[j];
                        closedshells[l].type = t;
                        /* find out its name */
                        if(ST_PARAM(lines[shids[j]][3]) && strings[ST_PARAM(lines[shids[j]][3])][0]) {
                            closedshells[l].name = _m3d_safestr(strings[ST_PARAM(lines[shids[j]][3])], 0);
                        } else {
                            sprintf(name, "b%04x", l);
                            closedshells[l].name = _m3d_safestr(name, 0);
                        }
                        /* closed shells in shell based surface records share the same matrix which is conceptually wrong, and
                         * a clear sign that the people who designed STEP were absent from school when they learned geometry */
                        if(m != -1U) {
                            /* convert axis2_placement_3d in lines[m] to transformation matrix */
                            step_placement(m, &closedshells[l].transform);
                        } else
                            aiIdentityMatrix4(&closedshells[l].transform);
                    }
                }
            }
        }
    if(shids) free(shids);
    /* if the above failed, simply look for closed shells */
    if(!numclosedshells)
        for(i = 0; i < numlines; i++)
            if(lines[i] && lines[i][0] > 5 && lines[i][2] == ST_PAROP && ST_TYPE(lines[i][3]) == ST_STRING &&
                lines[i][1] == ST_CLOSED_SHELL) {
                    j = numclosedshells++;
                    closedshells = (steproot_t*)realloc(closedshells, numclosedshells * sizeof(steproot_t));
                    if(!closedshells) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                    closedshells[j].id = i;
                    closedshells[j].type = ST_MANIFOLD_SOLID_BREP;
                    if(ST_PARAM(lines[i][3]) && strings[ST_PARAM(lines[i][3])][0])
                        closedshells[j].name = _m3d_safestr(strings[ST_PARAM(lines[i][3])], 0);
                    else {
                        sprintf(name, "b%04x", j);
                        closedshells[j].name = _m3d_safestr(name, 0);
                    }
                    /* no transformation */
                    aiIdentityMatrix4(&closedshells[j].transform);
            }
    /* parse them (if we have found any) into M3D shapes */
    if(numclosedshells) {
        /* add shells as bones */
        b = m3d->numbone; m3d->numbone += numclosedshells;
        m3d->bone = (m3db_t*)realloc(m3d->bone, m3d->numbone * sizeof(m3db_t));
        if(!m3d->bone) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
        memset(&m3d->bone[b], 0, numclosedshells * sizeof(m3db_t));
        /* add faces as shapes */
        for(i = 0, s = m3d->numshape; i < numclosedshells; i++)
            for(l = 6, k = closedshells[i].id; l < lines[k][0] && lines[k][l] != ST_PARCL; l++)
                if(ST_ISENTITY(lines[k][l], 5)) m3d->numshape++;
        m3d->shape = (m3dh_t*)realloc(m3d->shape, m3d->numshape * sizeof(m3dh_t));
        if(!m3d->shape) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
        memset(&m3d->shape[s], 0, (m3d->numshape - s) * sizeof(m3dh_t));
        /* parse geometry for each face */
        for(i = 0; i < numclosedshells; i++, b++) {
            m3d->bone[b].parent = (M3D_INDEX)-1U;
            m3d->bone[b].name = closedshells[i].name;
            m3d->vertex = _assimp_addspace(m3d->vertex, &m3d->numvertex, &closedshells[i].transform, VT_WORLD, &k);
            m3d->bone[b].pos = (M3D_INDEX)k;
            m3d->bone[b].ori = (M3D_INDEX)(k + 1);
            /* we don't need the scaling part */
            m3d->numvertex--;
            k = closedshells[i].id;
            for(l = 6; l < lines[k][0] && lines[k][l] != ST_PARCL; l++) {
                j = ST_PARAM(lines[k][l]);
                if(ST_ISENTITY(lines[k][l], 5)) {
                    memcpy(&nm, &closedshells[i].transform, sizeof(struct aiMatrix4x4));
                    for(m = t = -1U, n = 6; m == -1U && n < lines[j][0]; n++) {
                        t = ST_PARAM(lines[j][n]);
                        if(ST_ISENTITY(lines[j][n], 5) && ST_ISENTITY(lines[t][5], 5) &&
                            lines[ST_PARAM(lines[t][5])][1] == ST_AXIS2_PLACEMENT_3D &&
                            ST_TYPE(lines[ST_PARAM(lines[t][5])][5]) == ST_ID && ST_TYPE(lines[ST_PARAM(lines[t][5])][7]) == ST_ID &&
                            ST_TYPE(lines[ST_PARAM(lines[t][5])][9]) == ST_ID)
                                    m = ST_PARAM(lines[t][5]);
                    }
                    if(m != -1U) {
                        step_placement(m, &tm);
                        aiMultiplyMatrix4(&nm, &tm);
                    }
                    if(t == -1U || !lines[t] || !lines[t][0] || ST_TYPE(lines[t][1]) != ST_FUNC) {
                        fprintf(stderr, "m3dconv: unknown surface type in #%d\n", j);
                        step_dump(j, 0, 1);
                        continue;
                    }
                    if(ST_TYPE(lines[j][3]) == ST_STRING && ST_PARAM(lines[j][3]) && strings[ST_PARAM(lines[j][3])][0]) {
                        m3d->shape[s].name = _m3d_safestr(strings[ST_PARAM(lines[j][3])], 0);
                    } else {
                        sprintf(name, "s%04x", s);
                        m3d->shape[s].name = _m3d_safestr(name, 0);
                    }
                    m3d->shape[s].group = b;
                    for(n = 6; n < lines[j][0]; n++)
                        if(ST_ISENTITY(lines[j][n], 5) && ST_PARAM(lines[j][n]) != t)
                            m3d = step_geom(ST_PARAM(lines[j][n]), t, &nm, &m3d->shape[s], m3d, 0);
                    if(m3d->shape[s].numcmd) s++;
                }
            }
        }
        m3d->numshape = s;
    }

    /* free cache */
    for(i = 0; i < numlines; i++)
        if(lines[i]) free(lines[i]);
    for(i = 0; i < numstrings; i++)
        if(strings[i]) free(strings[i]);
    if(lines) { free(lines); lines = NULL; }
    if(strings) { free(strings); strings = NULL; }
    if(num) { free(num); num = NULL; }
    if(closedshells) { free(closedshells); closedshells = NULL; }
    numlines = numstrings = numnum = numclosedshells = 0;

    return m3d;
}

/**
 * Add a string to a unique list
 */
unsigned int step_addstr(char *s, char *e)
{
    unsigned int i, l;
    if(!s) return 0;
    if(e) l = (unsigned int)(e - s); else l = strlen(s);
    if(!l || !memcmp(s, "none\'", 5) || !memcmp(s, "NONE\'", 5) || !memcmp(s, "void\'", 5) || !memcmp(s, "VOID\'", 5) ||
        !memcmp(s, "nil\'", 4) || !memcmp(s, "NIL\'", 4) || !memcmp(s, "null\'", 5) || !memcmp(s, "NULL\'", 5)) return 0;
    for(i = 0; i < numstrings; i++)
        if(strlen(strings[i]) == l && !memcmp(s, strings[i], l)) return i;
    if(numstrings >= 0x0FFFFFFF) { fprintf(stderr, "m3dconv: too many unique strings\n"); exit(1); }
    i = numstrings++;
    if(numstrings >= maxstrings) {
        maxstrings += 1024;
        strings = (char**)realloc(strings, maxstrings * sizeof(char*));
        if(!strings) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
    }
    strings[i] = (char*)malloc(l + 1);
    if(!strings[i]) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
    memcpy(strings[i], s, l);
    strings[i][l] = 0;
    return i;
}

/**
 * Add a number to a unique list
 */
unsigned int step_addnum(float f)
{
    unsigned int i;
    for(i = 0; i < numnum; i++)
        if(num[i] == f) return i;
    if(numnum >= 0x0FFFFFFF) { fprintf(stderr, "m3dconv: too many unique numbers\n"); exit(1); }
    i = numnum++;
    if(numnum >= maxnum) {
        maxnum += 1024;
        num = (float*)realloc(num, maxnum * sizeof(float));
        if(!num) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
    }
    num[i] = f;
    return i;
}

/**
 * Load a model and convert it's structures into a Model 3D in-memory format
 */
m3d_t *step_load(char *data)
{
    unsigned int idx, i, j, k, line = 1;
    char *s, *e, np;
    m3d_t *m3d;
    float f = 0.0f;

    m3d = (m3d_t*)malloc(sizeof(m3d_t));
    if(!m3d) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
    memset(m3d, 0, sizeof(m3d_t));
    m3d->flags = M3D_FLG_FREESTR;

    /* parse header */
    while(*data && memcmp(data, "ENDSEC;", 7)) {
        if(data[0] == '/' && data[1] == '*') {
            for(data += 2; *data && (data[-1] != '/' || data[-2] != '*'); data++)
                if(data[0] == '\n') line++;
            continue;
        }
        if(!memcmp(data, "FILE_NAME", 9)) {
            for(data += 9, np = 0; *data && *data != ';'; data++) {
                if(data[0] == '/' && data[1] == '*') {
                    for(data += 2; *data && (data[0] != '/' || data[-1] != '*'); data++)
                        if(data[0] == '\n') line++;
                    continue;
                }
                if(data[0] == '\'') {
                    data++;
                    for(s = data; data[0] && data[0] != '\''; data++);
                    if(np == 2) { *data++ = 0; m3d->author = _m3d_safestr(s, 2); }
                }
                if(*data == ',') np++;
            }
        }
        if(!memcmp(data, "FILE_DESCRIPTION", 16)) {
            data += 16;
            while(*data && *data != '\'') {
                if(data[0] == '/' && data[1] == '*') {
                    for(data += 2; *data && (data[-1] != '/' || data[-2] != '*'); data++)
                        if(data[0] == '\n') line++;
                    continue;
                }
                data++;
            }
            if(*data == '\'') {
                data++;
                for(s = data; data[0] && data[0] != '\''; data++);
                *data++ = 0;
                s = _m3d_safestr((char*)s, 2);
                if(s && *s) {
                    m3d->desc = (char*)malloc(strlen(s) + 56);
                    if(!m3d->desc) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                    sprintf(m3d->desc, "ISO-10303\nGENERATOR=m3dconv-1.0.0\nFILE_DESCRIPTION=%s\n", s);
                    free(s);
                }
            }
        }
        if(data[0] == '\n') line++;
        data++;
    }
    if(*data) data++;

    /* parse data */
    step_addstr("(null)", NULL);
    while(*data) {
        /* find data section blocks */
        while(*data && memcmp(data, "DATA;", 5)) {
            /* skip comments and keep track of the actual line number */
            if(data[0] == '/' && data[1] == '*') {
                for(data += 2; *data && (data[-1] != '/' || data[-2] != '*'); data++)
                    if(data[0] == '\n') line++;
                continue;
            }
            if(data[0] == '\n') line++;
            data++;
        }
        /* clean up STEP statements. This is a nightmare, some idiots must have designed the spec */
        while(*data && memcmp(data, "ENDSEC;", 7)) {
            data = _m3d_findnl(data);
            line++;
            for(; *data == ' ' || *data == '\t' || *data == '\r' || *data == '\n' || *data == '/' || *data == ';'; data++) {
                if(data[0] == '/' && data[1] == '*') {
                    for(data += 2; *data && (data[0] != '/' || data[-1] != '*'); data++)
                        if(data[0] == '\n') line++;
                }
                if(data[0] == '\n') line++;
            }
            /* first, read the lines into a clean tokenized form. We must read *EVERYTHING* in advance, because identifier
             * references can be backward and forward as well... Even worse, lines can be in *ANY* order... What kind of fool
             * designed this file format??? This supposed to be a declerative language only, not a fully blown, insane
             * programming language structure! */
            if(*data == '#') {
                data = _m3d_getint(data + 1, &idx);
                for(; *data == ' ' || *data == '\t' || *data == '\r' || *data == '\n' || *data == '/' || *data == '='; data++) {
                    if(data[0] == '/' && data[1] == '*') {
                        for(data += 2; *data && (data[0] != '/' || data[-1] != '*'); data++)
                            if(data[0] == '\n') line++;
                    }
                    if(data[0] == '\n') line++;
                }
                if(idx >= numlines) {
                    i = numlines; numlines += 512;
                    if(numlines >= 0x0FFFFFFF) { fprintf(stderr, "m3dconv: too many entities\n"); exit(1); }
                    lines = (unsigned int**)realloc(lines, numlines * sizeof(unsigned int*));
                    if(!lines) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                    memset(&lines[i], 0, (numlines - i) * sizeof(unsigned int*));
                }
                /* find the actual end of the statement... don't increment line yet */
                for(s = data, i = 0; *data && (i || *data != ';'); data++) {
                    if(data[0] == '/' && data[1] == '*') {
                        for(data += 2; *data && (data[0] != '/' || data[-1] != '*'); data++);
                        continue;
                    }
                    if(data[0] == '\'') {
                        for(data++; *data && data[0] != '\''; data++)
                            if(data[0] == '\\') data++;
                        continue;
                    }
                    if(*data == '(') i++;
                    if(*data == ')') i--;
                }
                /* assume every character is a different token */
                lines[idx] = (unsigned int*)realloc(lines[idx], ((int)(data - s) + 1) * sizeof(unsigned int));
                if(!lines[idx]) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                /* tokenize statement */
                for(i = 1; s < data;) {
                    if(s[0] == '/' && s[1] == '*') {
                        for(s += 2; s < data && *s && (s[-1] != '/' || s[-2] != '*'); s++)
                            if(s[0] == '\n') line++;
                        continue;
                    } else
                    if(*s == ' ' || *s == '\r' || *s == '\n' || *s == '\t' || *s == ';') {
                        if(s[0] == '\n') line++;
                        s++;
                    } else
                    if(*s == ',') { lines[idx][i++] = ST_COMMA; s++; } else
                    if(*s == '(') { lines[idx][i++] = ST_PAROP; s++; } else
                    if(*s == ')') { lines[idx][i++] = ST_PARCL; s++; } else
                    if(*s == '$') { lines[idx][i++] = ST_DOLLAR; s++; } else
                    if(*s == '*') { lines[idx][i++] = ST_ASTERIX; s++; } else
                    if(*s == '.') {
                        for(e = s + 1; e < data && *e && *e != '.'; e++);
                        e++;
                        lines[idx][i++] = ST_TOKEN(ST_ENUM, step_addstr(s, e));
                        s = e;
                    } else
                    if(*s == '\'') {
                        for(s++, e = s; e < data && *e && *e != '\''; e++) { if(*e == '\\') e++; }
                        lines[idx][i++] = ST_TOKEN(ST_STRING, step_addstr(s, e));
                        s = e + 1;
                    } else
                    if((*s >= 'a' && *s <= 'z') || (*s >= 'A' && *s <= 'Z')) {
                        for(e = s; e < data && *e && *e != '\'' && *e != '\"' && *e != '/' && *e != '(' && *e != ')' &&
                            *e != ',' && *e != ';' && *e != ' ' && *e != '\t' && *e != '\r' && *e != '\n'; e++);
                        for(k = 0, j = -1U; k < (unsigned int)(sizeof(labels)/sizeof(labels[0])); k++)
                            if(strlen(labels[k]) == (unsigned int)(e - s) && !memcmp(labels[k], s, e - s)) { j = k; break; }
                        if(j == -1U)
                            lines[idx][i++] = ST_TOKEN(ST_LABEL, step_addstr(s, e));
                        else
                            lines[idx][i++] = ST_TOKEN(ST_FUNC, j);
                        s = e;
                    } else if(*s == '#') {
                        s = _m3d_getint(s + 1, &j);
                        lines[idx][i++] = ST_TOKEN(ST_ID, j);
                    } else if(*s == '\"') {
                        /* FIXME: WTF is unused bits indicator? is this BE really or sometimes LE? Why couldn't they
                         * just simply use the well-known 0x1234 format like everybody else on the entire planet??? */
                        /*
                        idx = 0;
                        s = _m3d_gethex(s + 2, &j);
                        ((unsigned char*)&f)[3] = ((unsigned char*)&j)[0];
                        ((unsigned char*)&f)[2] = ((unsigned char*)&j)[1];
                        ((unsigned char*)&f)[1] = ((unsigned char*)&j)[2];
                        ((unsigned char*)&f)[0] = ((unsigned char*)&j)[3];
                        lines[idx][i++] = ST_TOKEN(ST_NUM, step_addnum(f));
                        */
                        fprintf(stderr, "m3dconv: warning, unparsed hex value '%c%c%c%c...' in line %d\n",
                            s[0], s[1], s[2], s[3], line);
                        for(; s < data && *s && *s != '\"'; s++);
                        s++;
                    } else if(*s == '-' || (*s >= '0' && *s <= '9')) {
                        for(e = s; *e == '-' || *e == '+' || *e == '.' || (*e >= '0' && *e <= '9') || *e == 'e' || *e == 'E'; e++);
                        f = (float)strtod(s, &s);
                        lines[idx][i++] = ST_TOKEN(ST_NUM, step_addnum(f));
                    } else {
                        /* should never reach this */
                        fprintf(stderr, "m3dconv: warning, unrecognized token '%c%c%c%c...' in line %d\n",
                            s[0], s[1], s[2], s[3], line);
                        s++;
                    }
                }
                lines[idx][0] = i;
                lines[idx][i++] = ST_END;
                /* free extra memory */
                lines[idx] = (unsigned int*)realloc(lines[idx], i * sizeof(unsigned int));
                if(!lines[idx]) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
            }
        }
        /* when we reached endsec, then we have all the identifiers, so it is time to parse the tokens */
        m3d = step_parse(m3d);
    }
    if(!m3d->numshape)
        fprintf(stderr, "m3dconv: no closed shell b-rep was found in STEP file.\n");

    return m3d;
}
