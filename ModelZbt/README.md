Model 3D File Format
====================

Check out the [Model 3D homepage](https://bztsrc.gitlab.io/model3d/) for .m3d file samples. If you find any problem with the
M3D SDK, please use the [issue tracker](https://gitlab.com/bztsrc/model3d/issues) and let me know.

## Rationale

In the last 30 years or so many file formats were invented to store 3D models. As a result, nowdays many formats are in use, but
unfortunatelly all of them lack one or more key features, and neither of them can be used to store a single 3D model conveniently.
Either they are too simple (only capable of storing static models, like .OBJ, .PLY, .STL etc.) or highly overcomplicated to store
entire scenes with multiple models and huge pile of unnecessary info (like .BLEND, .DAE, .X3D, .glTF, .FBX etc. Of course those
additional lightning, camera, physics etc. info are only unnecessary if you're just interested in loading a model, like in a game
engine for example). That complexity often leads to problem when interchaning models between software. Also existing scene formats
are often utilize a highly uneffective storage representation which makes them inefficient for model distribution (yes, that
clearly stands for .glTF too, despite the fact they claim otherwise). Also the most useful formats are poorly documented (.STEP,
.B3D, .BLEND etc.) or proprietary (like .FBX, .PBX, .MAX, etc.), so it is questionable if one can use those in their projects at
all and if so, how. Yes there are libraries to come around some of these obsticles, but most importantly they do not solve the
issue of missing capabilities nor distribution compactness in file formats, and they often introduce unnecessarity additional
complexity to the build-environment (like the .FBX SDK for example).

[This Model 3D format](https://gitlab.com/bztsrc/model3d/blob/master/docs/m3d_format.md) was designed to address all of the
shortcomings of the existing current formats, so that it can became a truely universal 3D model container and ease all developer's
and model designer's minds, consistently parsed by all software while being easy on network traffic at the same time:

### Features

 - the format is freely usable, MIT licensed (unlike .PBX, .MAX or .C4D)
 - also has an [ASCII](https://gitlab.com/bztsrc/model3d/blob/master/docs/a3d_format.md) representation for exchange and easy debugging (unlike .BLEND or .B3D, similar to .OBJ but with skeletal animation support)
 - stores exactly one model with all its aspects, but nothing else (unlike .FBX, .X3D, .BLEND; just like .PLY)
 - has the biggest information density of all the formats (a cube can be stored in as little as 119 bytes)
 - can store huge number of vertices in magnitude of billions (unlike .3DS)
 - can store color maps as well as texture maps and materials
 - can store inlined textures (ideal for model distribution)
 - can store precise normal vectors, not just smoothing groups (unlike .3DS)
 - can store [voxel](https://gitlab.com/bztsrc/model3d/blob/master/docs/voxel_format.md) images, polygon mesh, procedural surfaces and procedural textures*
 - can store parameterized mathematical surfaces, like [NURBS](https://gitlab.com/bztsrc/model3d/blob/master/docs/shape.md) (and other [CAD](https://gitlab.com/bztsrc/model3d/blob/master/docs/CAD.md) information)
 - can store skeleton based animations (unlike .MD3, which uses vertex morph only, or .MD5 which has separate files for anims)
 - can store actions, several named animation frames within the same file
 - can store model with absolute scaling factor and face thickness for 3D printing (is there any format that can do these?)
 - utilizes engine agnostic data structure, focuses on storing what's relevant for the model representation, not for the engines
 - supports material libraries in separate files if needed (using the same format)
 - expandable in nature, additional information can be easily added if needed without breaking compatibility with other applications
 - flexible data types (similar to .PLY, but much simpler, coordinate precision from 8 bits to 64 bits (*int8_t* to *double*))
 - compact file size, damn easy to interpret without third-party libraries** (~80 SLoC see below, unlike .FBX, .X3D, .glTF or .X)
 - the SDK follows the K.I.S.S. principle, MIT licensed just as the format itself, easy to integrate into any project

(* - you provide a script interpreter callback for the SDK, so you can [use any scripting language](https://gitlab.com/bztsrc/model3d/blob/master/docs/procedural.md) you like, eg. Lua, Python, whatever)

(** - the model's data may be stream compressed, however a) this is optional, and b) the SDK has a built-in decompressor. You won't
need any third-party libraries to use the SDK, like JSON or XML libraries, libpng, zlib etc. it is dependency-free.)

## Implementations

 - [m3d.h](https://gitlab.com/bztsrc/model3d/blob/master/m3d.h) - a standalone, single header ANSI C/C++ M3D SDK for the format (see the [API](https://gitlab.com/bztsrc/model3d/blob/master/docs/API.md) reference and the [manual](https://gitlab.com/bztsrc/model3d/blob/master/docs/usage.md))
 - [webgl-js](https://gitlab.com/bztsrc/model3d/tree/master/webgl-js) - an .M3D image polyfill written in vanilla Javascript with WebGL
 - [blender](https://gitlab.com/bztsrc/model3d/tree/master/blender) - an .M3D export (and eventually import) plugin for Blender written in Python
 - [goxel](https://gitlab.com/bztsrc/model3d/tree/master/goxel) - an .M3D import / export plugin for the Goxel voxel image editor, written in C

### Utilities

 - [m3dconv](https://gitlab.com/bztsrc/model3d/tree/master/m3dconv) - a small command line utility to convert common model formats into .M3D format and to create material libraries
 - [m3dview](https://gitlab.com/bztsrc/model3d/tree/master/m3dview) - an easily portable, simple .M3D file viewer also an example how to convert in-memory model into streaming VBO and EBO lists
 - [validator](https://gitlab.com/bztsrc/model3d/tree/master/validator) - a web based [Model 3D file validator](https://bztsrc.gitlab.io/model3d/validator)
 - [tests](https://gitlab.com/bztsrc/model3d/tree/master/tests) - test cases and API usage examples (sample models in the [models](https://gitlab.com/bztsrc/model3d/tree/master/models) directory)

## Overview

### Parsing

The format was designed in a way so that different engines can read different amount of data by skipping unsupported or unwanted
chunks. Each additional chunk builds on the data of the previous chunk, extending the information on the model. Therefore the model
can be reconstructed perfectly even with skipped chunks, although with less details. It is also possible to easily embed engine
specific information of the model without corrupting the file or breaking compatibility with other applications.

| Level                     | Chunks to be parsed                                                                         |
|---------------------------|---------------------------------------------------------------------------------------------|
| 1. Static Wireframe       | vertex, face                                                                                |
| 2. Static Colored Mesh    | vertex, color map and color indices too in face                                             |
| 3. Static Textured Mesh   | vertex, texture map, materials and texture coordinate indices too in face, inlined textures |
| 4. Animated Skeleton      | vertex, bones, action frames                                                                |
| 5. Animated Mesh          | vertex, bones with skin, face, action frames                                                |
| 6. Animated Colored Mesh  | vertex, bones, face, color map, action frames                                               |
| 7. Animated Textured Mesh | vertex, bones, face, texture map, materials, action frames, inlined textures                |

There's no reason really not to use the official [M3D SDK](https://gitlab.com/bztsrc/model3d/blob/master/docs/usage.md) (which can
parse all chunks, decodes PNGs, dependency-free and provided as a single header file), but if you insist, then parsing the format
on your own is allowed by the license, and reading a colored mesh is pretty easy, __less than 80 SloC__ in C/C++:
```c
#include <stdint.h>             /* int8_t, int16_t, int32_t, uint16_t, uint32_t */
#include <stdlib.h>             /* realloc */
#include <string.h>             /* memcmp */
/*#include <stb_image.h>*/      /* include only if you want to load stream compressed models too */
```
Note that including [stb_image](http://nothings.org/stb) is optional. However you probably might want to decode PNG textures, so
sooner or later you'll include it anyway. The only real dependency is standard integer types and realloc from libc (gcc can provide
both as built-ins).
```c
const unsigned char *data;      /* input buffer with binary .m3d, series of 4 byte magic, 4 byte length, length bytes data */

typedef struct {
    float x, y, z;
    uint32_t rgba;
} vertex_t;

int numvertex = 0;
vertex_t *vertex = NULL;        /* output buffer for vertices, to be used as VBO */

int numtriangle = 0;
int *triangle = NULL;           /* output buffer for model's face, to be used as EBO */
```
The global variables in this simple example, input and output buffers. Triangles will be stored in an int array, where each
element is indexing a vertex, and 3 subsequent elements give a triangle. The length of the triangle array is 3 times numtriangle.

After this little preface, without any further ado, here comes the loader:
```c
if(!memcmp(data, "3DMO", 4)) {                                          /* check file magic */
    uint32_t len = *((uint32_t*)(data + 4)) - 8;                        /* helper variable for buffer and chunk length */

    if(!memcmp(data + 8, "PRVW", 4)) {                                  /* skip over optional preview image chunk */
        len -= *((uint32_t*)(data + 12)); data += *((uint32_t*)(data + 12)); }

#if defined(STBI_INCLUDE_STB_IMAGE_H) && !defined(STBI_NO_ZLIB)
    unsigned char *ptr = !memcmp(data + 8, "HEAD", 4) ? data + 8 :      /* get a pointer to the first uncompressed chunk */
        stbi_zlib_decode_malloc_guesssize_headerflag((const char*)data + 8, len, 16384, &len, 1);
#else
    unsigned char *ptr = data + 8;                                      /* get a pointer to the first chunk */
#endif

    unsigned char *end = ptr + len;                                     /* get buffer end pointer (for safety) */
    uint32_t *cmap = NULL;                                              /* pointer to color map */
    int vc_s = 1 << ((ptr[12] >> 0) & 3);                               /* vertex coordinate size */
    int vi_s = 1 << ((ptr[12] >> 2) & 3);                               /* vertex index size */
    int si_s = 1 << ((ptr[12] >> 4) & 3);                               /* string offset size */
    int ci_s = 1 << ((ptr[12] >> 6) & 3);                               /* color index size */
    int ti_s = 1 << ((ptr[13] >> 0) & 3);                               /* texture map index size */
    int sk_s = 1 << ((ptr[13] >> 6) & 3);                               /* skin index size */
    float size_m = *((float*)(ptr + 8));                                /* get model metric size */
    data = ptr + *((uint32_t*)(ptr + 4));                               /* jump over model header chunk */

    while(data < end && memcmp(data, "OMD3", 4)) {                      /* iterate through chunks until we reach the last */
        ptr = data + 8;                                                 /* pointer to chunk data */
        len = *((uint32_t*)(data + 4));                                 /* get chunk length */

        if(!memcmp(data, "CMAP", 4)) { cmap = (uint32_t*)ptr; } else    /* found color map chunk (unique chunk) */

        if(!memcmp(data, "VRTS", 4)) {                                  /* found vertex chunk (unique chunk) */
            numvertex = (len - 8) / ((ci_s != 8 ? ci_s : 0) + (sk_s != 8 ? sk_s : 0) + 4 * vc_s);
            vertex = (vertex_t*)realloc(vertex, numvertex * sizeof(vertex_t));
            for(int i = 0; i < numvertex; i++) {                        /* read in data for each vertex */
                switch(vc_s) {                                          /* get coordinates */
                    case 1:
                        vertex[i].x = size_m * ((float)((int8_t)ptr[0]) / 127);
                        vertex[i].y = size_m * ((float)((int8_t)ptr[1]) / 127);
                        vertex[i].z = size_m * ((float)((int8_t)ptr[2]) / 127);
                        ptr += 4;
                    break;
                    case 2:
                        vertex[i].x = size_m * ((float)(*((int16_t*)(ptr+0))) / 32767);
                        vertex[i].y = size_m * ((float)(*((int16_t*)(ptr+2))) / 32767);
                        vertex[i].z = size_m * ((float)(*((int16_t*)(ptr+4))) / 32767);
                        ptr += 8;
                    break;
                    case 4:
                        vertex[i].x = size_m * (*((float*)(ptr+0)));
                        vertex[i].y = size_m * (*((float*)(ptr+4)));
                        vertex[i].z = size_m * (*((float*)(ptr+8)));
                        ptr += 16;
                    break;
                }
                switch(ci_s) {                                          /* get RGBA color if exists */
                    case 1:  vertex[i].rgba = cmap[ptr[0]]; ptr++; break;
                    case 2:  vertex[i].rgba = cmap[*((uint16_t*)ptr)]; ptr += 2; break;
                    case 4:  vertex[i].rgba = *((uint32_t*)ptr); ptr += 4; break;
                    default: vertex[i].rgba = 0xFF808080;               /* use a default gray color otherwise */
                }
                if(sk_s != 8) ptr += sk_s;                              /* skip over skin index if exists */
            }
        } else

        if(!memcmp(data, "MESH", 4)) {                                  /* found mesh chunk (could be more) */
            while(ptr < data + len) {
                int rmagic = *ptr++;                                    /* get record magic */
                int np = rmagic >> 4;
                if(!np) { ptr += si_s; continue; }                      /* skip over "use material" record */
                /* you should check if "np" is indeed 3, meaning triangle polygon */
                int i = numtriangle++;                                  /* increment number of triangles and allocate memory */
                triangle = (int*)realloc(triangle, numtriangle * np * sizeof(int));
                for(int j = 0; j < np; j++) {                           /* for each edge of the polygon, we do... */
                    switch(vi_s) {                                      /* get vertex index */
                        case 1: triangle[i * np + j] = ptr[0]; ptr++; break;
                        case 2: triangle[i * np + j] = *((uint16_t*)ptr); ptr += 2; break;
                        case 4: triangle[i * np + j] = *((uint32_t*)ptr); ptr += 4; break;
                    }
                    if(rmagic & 1) ptr += ti_s;                         /* skip over texture UV if exists */
                    if(rmagic & 2) ptr += vi_s;                         /* skip over normal vector if exists */
                    if(rmagic & 4) ptr += vi_s;                         /* skip over maximum vertex if exists */
                }
            }
        }

        data += len;                                                    /* jump to the next chunk */
    }
}
```
That's all! Now you can display a colored triangle mesh! Of course this is a simple example without proper error handling. It only
parses three of the chunks, see the [.M3D file format](https://gitlab.com/bztsrc/model3d/blob/master/docs/m3d_format.md) for the
description of all available chunks, or simply use the SDK which already supports all of them.

### Basic Concepts

To display a static 3D model, you need to get its face, which is defined by a triangle mesh. In the simplest form, you paint
all triangles with the same color (one could change the brightness of the color depending on the position of the light source).

By default, each point (a coordinate triplet or vertex) has an associated color code. Triangles can be rendered by creating a
gradient between the three points' colors. This allows rendering colored model without needing to parse any additional
material information. This is also extremely fast, so can be used on slow machines or when the model is rendered far away from
the viewer and contour with a rough shape is enough, no detail specifics needed.

Going further, a triangle can be associated with a material. Those material definitions have many interesting data, such as how
metallic it should be or how much it refracts light. Material definitions also contain texture image references, which (if parsed)
can be used to stretch an image on a triangle. Instead of the three color gradient, this allows many different colors and even
depth on the triangle, and thus give lots of details on the model's face.

Skeleton based models define several groups, so called bones, in a hierarchy. This allows to select a pre-defined set of triangles
at once and use a specific transformation on them. Note that a triangle can have points belonging to different bones, this is
intentional as it provides cheap streching skin effect when bones are moving. For more complex models, you can set up more bone
references with weights (up to 8, but usually 4 bones is enough) which is called skinning (or rigging).

Finally animated models have many frames, where each frame has exactly the same triangles, but with different coordinates.
Some formats simply store all of those coordinates for each frame, or even worse, separate face for each frame, but that
requires enormous storage space. Skeleton based animation on the other hand describes frames simply by applying specific
transformations on the bone hierarchy, therefore it stores bone modifications only, regardless how many points
and triangles are defined for that bone (or bones if it has children). Actions are animations, but the model usually does
not store one big animation with many frames, instead it has several smaller frame sets each identified by name (like "walking",
"attacking" etc.).

Now an animation takes a specific time. To see that fluent, we need at least 25 frames per second. Let's assume we want a
waving hand animation that takes 5 secs long, that would mean 125 frames. Model 3D does not store that many frames, only the
first and last frame, and maybe a couple few frames with midpoint bone setups in between. Therefore the renderer must
[interpolate](https://gitlab.com/bztsrc/slerp-opt) the bone positions and orientation for the missing frames. That's the reason
why unlike movie formats or animated GIFs, Model 3D frames does have an FPS value nor a frame delay stored with the model's
animation. Thus Model 3D animations are flexible, only the overall duration of an animation stored as guidance, but actually can
last as long as the engine wishes.

See the [SDK manual](https://gitlab.com/bztsrc/model3d/blob/master/docs/usage.md) to learn how the single header M3D SDK makes
all of this, but specially the skeletal animation easy for you.

See the [M3D file format specification](https://gitlab.com/bztsrc/model3d/blob/master/docs/m3d_format.md) for all the details.

## Known Issues

The converter utility, [m3dconv](https://gitlab.com/bztsrc/model3d/tree/master/m3dconv) is having a hard time figuring out
assimp's structures, specially animations. Therefore it cannot convert all models properly, I've seen models with skeletal
animation that also had a mesh transform (that obviosly cannot be described as a bone transform). Static meshes work for all
formats, and formats that only create bone references in aiNodeAnim records work perfectly.

The STEP format importer in m3dconv is work in progress. It can read, tokenize STEP files, it parses topology, but converting
geometry into shape commands is not finished as of yet.

The [assimp](https://gitlab.com/bztsrc/model3d/tree/master/assimp) plugin can import all mesh related chunks in M3D files, but not
shapes, like NURBS (that's an assimp limitation). The export is having the same animation issues as the converter and is limited
to static meshes only. It is recommended to use m3dconv instead (also incomplete, but at least tries to do its best and has some
heuristics on assimp's data, which provides fair results).

The [blender](https://gitlab.com/bztsrc/model3d/tree/master/blender) plugin needs importer functionality.

### The unaligned access issue

This has popped up multiple times now, so here's my brief answer: this isn't a bug, this is a feature :-) Not joking :-)

Long answer: in a compact bit-chunk such as the compressed model file proper alignment obviously cannot be guaranteed, because
padding with zeros would insanely increase the storage requirements. In order to avoid unaligned access, one could use countless
multiple byte accesses, logical masking and shifting operations and a casting on the result, but that would kill the performance.
Or one could copy the data parts (few bytes at a time) over and over again using memcpy into an aligned buffer, but that would
also kill the performance.

On the other hand, a single casted pointer de-reference like the ones I use, always compile into a *single Assembly instruction*
on all architectures, which is as fast as it possibly can be. Long gone the days when the CPUs couldn't handle unaligned accesses,
in all modern mainstream CPUs (x86, ARM, RISC-V, even in Java and WebAssembly VMs) this problem simply doesn't exists any more.

So the decision I had to make with the M3D SDK was: keep UBSan happy but write crappy and increadibly slow code; or don't care
about UBSan and take advantage of modern CPU features for increased performance. I've choosen the latter, sorry UBSan.

That's all,

bzt
