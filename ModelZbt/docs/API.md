Model 3D Importer / Exporter Application Interface
==================================================

This is the API reference. For more explained usage instructions and source examples, see the SDK
[usage manual](https://gitlab.com/bztsrc/model3d/blob/master/docs/usage.md).

The [M3D](https://gitlab.com/bztsrc/model3d/blob/master/docs/m3d_format.md) SDK is provided as a
[single ANSI C header file](https://gitlab.com/bztsrc/model3d/blob/master/m3d.h), with no dependencies (other than `libc`). In
exactly one of your source files, define `M3D_IMPLEMENTATION` before you include the header. The header also provides a C++ wrapper
class around the standard C API. __No linkage needed.__

Because the SDK is totally dependency-free, it has inlined portions of [stb_image](http://nothings.org/stb) for decompressing PNG
textures. However those are not used if you include the stb_image.h header before m3d.h in the source file where you've defined
`M3D_IMPLEMENTATION`. This serves two purposes: first, you can avoid code duplication. Second, you can replace the built-in
version with a newer picture decoder should there be any bug.
```c
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define M3D_IMPLEMENTATION
#include <m3d.h>
```
Including stb_image is optional, m3d.h can load PNG files on its own. Be aware, that if you include stb_image _after_ m3d, then
there'll be code duplication, as ifdef guards in m3d can't work if the stb_image defines are created _after_ the ifdef check
(obviously).

**WARNING**: the stb_image integration only works if STB_IMAGE_IMPLEMENTATION is set. This is an issue with stb_image, because
it declares stb__png_load as static (so not accessible from external compilation objects).

By default, only the C API is used. To include the C++ wrapper class, use
```c
#define M3D_IMPLEMENTATION
#define M3D_CPPWRAPPER
#include <m3d.h>
```

[[_TOC_]]

Configuration
-------------

| Define               | Description                                                               |
|----------------------|---------------------------------------------------------------------------|
| `M3D_IMPLEMENTATION` | controls if the header should include the implementation too              |
| `M3D_CPPWRAPPER`     | define this to include the C++ wrapper                                    |
| `M3D_EXPORTER`       | define this to also include the Model 3D exporter's implementation        |
| `M3D_NOIMPORTER`     | define this to avoid including the Model 3D importer implementation       |
| `M3D_ASCII`          | define this to support the ASCII Model 3D variant too (import / export)   |
| `M3D_NOANIMATION`    | define this to skip animation API and pre-calculated bone matrices        |
| `M3D_NONORMALS`      | don't calculate smooth normals if they are not loaded from file           |
| `M3D_NOWEIGHTS`      | don't calculate weight list per bones                                     |
| `M3D_NOVOXELS`       | don't convert voxels to vertices and mesh face                            |
| `M3D_LOG(s)`         | specify a call to your logger function. Argument "s" is a C string        |
| `M3D_DOUBLE`         | define to use double precision coordinates in the in-memory structures    |
| `M3D_SMALLINDEX`     | cut in half the index size if you're fine with 65534 (see below)          |
| `M3D_NUMBONE`        | specify maximum number of bones per vertex: 1, 2, 4 or 8; defaults to 4   |
| `M3D_BONEMAXLEVEL`   | specify the maximum height in bone hierarchy; defaults to 8               |
| `M3D_VERTEXTYPE`     | if defined, then adds a byte to each vertex to be used by the application |
| `M3D_VERTEXMAX`      | if defined, then adds a parameter and maximum verteces to face records    |
| `M3D_TX_INTERP`      | name of a function to interpret texture scripts                           |
| `M3D_PR_INTERP`      | name of a function to interpret procedural surface scripts                |

The precision, index size and maximum number of bones per vertex greatly influences how much memory an in-memory model requires.
Precision `double` is only needed by a very few special scientific application, and you also have to modify your shader's input
accordingly for double precision.

Post-processing configuration is done with `M3D_NOANIMATION` (don't calculate bone matrices), `M3D_NONORMALS` (don't calculate
smooth normals if they are not loaded from the model), `M3D_NOWEIGHTS` (don't calculate weight lists) and `M3D_NOVOXELS` (don't
convert voxels into vertices and mesh). With all the `M3D_NOx` defines, the loading will be much faster, but you may lack some
features.

Voxels are small 3D cubes aligned on a regular grid. These are loaded into a sperate structure, but if `M3D_NOVOXELS` is not
defined, then they are also converted into a triangle mesh on-the-fly. This conversion was never designed to be effective, just
to be correct. For a game engine you should convert and optimize voxels into cubes yourself depending on the optimizations your
engine uses.

Normally indices use `uint32_t`, but with `M3D_SMALLINDEX` defined, then `uint16_t`. This means if you're sure that no list in
the loaded models will exceed 65534 (no more vertex, no more UV pairs, no more textures, no more materials, no more bones, etc.)
then you can drastically shrink the in-memory model's memory requirement. Small index also shrinks the number of possible voxel
types from `uint16_t` to `uint8_t`.

`M3D_NUMBONE` defines that maximum how many bones can influence a vertex. 4 is a common industry standard.

If `M3D_BONEMAXLEVEL` is 2, then you can have a root bone and one level of sub-bones, but not sub-sub-bones. 8 is a reasonable
default, allowing the full chain of body - shoulder - upperarm - lowerarm - hand - fingerupper - fingermiddle - fingerlower bones.
Increasing this does not influence the in-memory size, and it is only important for the ASCII format.

The `M3D_VERTEXTYPE` define adds an unit8_t to each vertex. This type is not stored in the model files, it is in the in-memory
model only, to be used by the application.

Normally each polygon point contains only one vertex. With `M3D_VERTEXMAX` all points will have two: a minimum vertex and a
maximum vertex coordinate, and the actual coordinate will be in between those, specified by an additional parameter.

For `M3D_TX_INTERP` and `M3D_PR_INTERP`, see [procedural models](https://gitlab.com/bztsrc/model3d/blob/master/docs/procedural.md)
documentation.

Model Import
------------

This function is only available if the header was included without `M3D_NOIMPORTER`.
In the definitions, the first line contains the C API prototype, the second (and subsequent) is the C++ wrapper.

### Create In-Memory Representation from Distribution Format

```c
m3d_t *m3d_load(unsigned char *data, m3dread_t readfilecb, m3dfree_t freecb, m3d_t *mtllib);
model = new M3D::Model(std::string &data, m3dread_t *readfilecb, m3dfree_t *freecb, M3D::Model mtllib);
model = new M3D::Model(std::vector<unsigned char> data, m3dread_t *readfilecb, m3dfree_t *freecb, M3D::Model mtllib);
model = new M3D::Model(unsigned char *data, m3dread_t *readfilecb, m3dfree_t *freecb, M3D::Model mtllib);
model = new M3D::Model();
```
Parse a mode into in-memory structure.

#### Parameters

| Name       | Description                                                                                        |
|------------|----------------------------------------------------------------------------------------------------|
| data       | pointer to a memory buffer                                                                         |
| readfilecb | `unsigned char *(*m3dread_t)(char *filename, unsigned int *size)` a callback to load assets        |
| freecb     | `void (*m3dfree_t)(void *buffer)` a callback to free the buffer returned by readfilecb             |
| mtllib     | pointer to another model if you want shared material libraries (see [manual](https://gitlab.com/bztsrc/model3d/blob/master/docs/usage.md) for details) |

#### Return Value

A pointer to a newly allocated `m3d_t` struct. On error returns a `NULL`. The C++ version returns a `M3D::Model` wrapper object.

### Free an In-Memory Representation

This function is only available if the header was included without `M3D_NOIMPORTER` or with `M3D_EXPORTER`.

```c
void m3d_free(m3d_t *model);
delete model;
```
Free the in-memory structure of the model. This does not free the binary data buffer, if you have allocated that, you should
call free manually on that buffer too.

#### Parameters

| Name   | Description                    |
|--------|--------------------------------|
| model  | pointer to the model structure |

Model Export
------------

This function is only available if the header was included with `M3D_EXPORTER`.

### Convert In-Memory Representation into Distribution Format

```c
unsigned char *m3d_save(m3d_t *model, int quality, int flags, unsigned int *size);
std::vector<unsigned char> M3D::Model.Save(int quality, int flags);
```
This function allocates a new buffer and encodes the in-memory model into a distributable format.

#### Parameters

| Name       | Description                                                          |
|------------|----------------------------------------------------------------------|
| model      | pointer to a m3d_t struct                                            |
| quality    | coordinate precision, 0 = int8_t, 1 = int16_t, 2 = float, 3 = double |
| flags      | ORed options what to save into the output, see below                 |
| size       | the size of the binary data returned                                 |

| Quality            | Description                                  |
|--------------------|----------------------------------------------|
| M3D_EXP_INT8       | export with -128 to 127 coordinate precision |
| M3D_EXP_INT16      | export with -32768 to 32767 precision        |
| M3D_EXP_FLOAT      | export with 32 bit floating point precision  |
| M3D_EXP_DOUBLE     | export with 64 bit floatint point precision  |

| Flags              | Description                     |
|--------------------|---------------------------------|
| M3D_EXP_NOCMAP     | don't export color map          |
| M3D_EXP_NOMATERIAL | don't export materials          |
| M3D_EXP_NOFACE     | don't export model face         |
| M3D_EXP_NONORMAL   | don't export normal vectors     |
| M3D_EXP_NOTXTCRD   | don't export texture UV         |
| M3D_EXP_NOVRTMAX   | don't export maximum vertex     |
| M3D_EXP_FLIPTXTCRD | flip V in texture UVs           |
| M3D_EXP_NORECALC   | don't recalculate coordinates   |
| M3D_EXP_IDOSUCK    | the input is left-handed        |
| M3D_EXP_NOBONE     | don't export skeleton           |
| M3D_EXP_NOACTION   | no animation nor skeleton saved |
| M3D_EXP_INLINE     | inline assets into the model    |
| M3D_EXP_EXTRA      | export unknown chunks too       |
| M3D_EXP_NOZLIB     | export into uncompressed binary |
| M3D_EXP_ASCII      | export into ASCII format        |

#### Return Value

A pointer to a newly allocated buffer, and buffer's size in `size`. On error returns a `NULL` and size of 0. If the header
was included with `M3D_ASCII` define, and `M3D_EXP_ASCII` was passed in flags, then it returns a zero terminated UTF-8 string.
Otherwise the output is a binary bit-chunk.

Model Animation
---------------

### Getting Skeleton for Animation Poses

The animation API provides two functions if included without `M3D_NOIMPORTER`. This one is available even if included with the
`M3D_NOANIMATION` define.

```c
m3dtr_t *m3d_frame(m3d_t *model, uint32_t actionid, uint32_t frameid, m3dtr_t *skeleton);
std::vector<m3dtr_t> M3D::Model.getActionFrame(int actionid, int frameid, std::vector<m3dtr_t> skeleton);
```
It may be a bit difficult to understand how to get the animation-pose skeleton from the bones for a particular action's certain
frame. This function helps you with that, and returns a working copy of the skeleton.

#### Parameters

| Name       | Description                                   |
|------------|-----------------------------------------------|
| model      | pointer to the model structure                |
| actionid   | index of an action (use -1 for the bind-pose) |
| frameid    | index of a frame of that action               |
| skeleton   | NULL, or a previously returned skeleton       |

#### Return Value

When called with NULL as `skeleton`, then a pointer to a newly allocated `m3dtr_t` array which has model->numbone elements.
Otherwise it returns `skeleton` with modified elements. On error sets a `M3D_ERR_UNKFRAME` error code and returns the skeleton
unchanged. The C++ version returns a standard vector of `m3dtr_t`.

### Getting Interpolated Animation Poses

This function is only available if included without `M3D_NOIMPORTER` and without `M3D_NOANIMATION` defines.

```c
m3db_t *m3d_pose(m3d_t *model, uint32_t actionid, uint32_t msec);
std::vector<m3db_t> M3D::Model.getActionPose(int actionid, uint32_t msec);
```

Returns the interpolated animation-pose skeleton (the bind-pose or rest-pose skeleton can be simply accessed in the `bone` array).

In a game engine, where you have to animate several models at once, you should not use this function, as its primary goal is
simplicity not effectiveness.

#### Parameters

| Name       | Description                                   |
|------------|-----------------------------------------------|
| model      | pointer to the model structure                |
| actionid   | index of an action (use -1 for the bind-pose) |
| msec       | millisecond timestamp                         |

#### Return Value

A newly allocated `m3db_t` array (same structure as model->bone), or `NULL` on error. If called with invalid `actionid` parameter,
returns the bind-pose skeleton. The length of the array is model->numbone.

Error Reporting
---------------

The functions may return `NULL`, which in general means memory allocation error. Otherwise the `m3d_t.errcode` field contains the
last integer error code, and enumerates as:

| Error Code         | Description                                       |
|--------------------|---------------------------------------------------|
| M3D_SUCCESS        | operation was successful                          |
| M3D_ERR_ALLOC      | memory allocation error                           |
| M3D_ERR_BADFILE    | malformed text file (ASCII import only)           |
| M3D_ERR_UNIMPL     | unimplemented interpreter (procedural surface)    |
| M3D_ERR_UNKPROP    | unknown material property record                  |
| M3D_ERR_UNKMESH    | unknown mesh record (only triangles supported)    |
| M3D_ERR_UNKIMG     | unknown image format (PNG only)                   |
| M3D_ERR_UNKFRAME   | unknown action or frame or missing bones          |
| M3D_ERR_UNKCMD     | unknown shape command record                      |
| M3D_ERR_UNKVOX     | no voxel dimension or voxel size defined          |
| M3D_ERR_TRUNC      | either the precision or number of bones truncated |
| M3D_ERR_CMAP       | too many or no chunk of this kind                 |
| M3D_ERR_TMAP       | too many or no chunk of this kind                 |
| M3D_ERR_VRTS       | too many or no chunk of this kind                 |
| M3D_ERR_BONE       | too many or no chunk of this kind                 |
| M3D_ERR_MTRL       | too many or no chunk of this kind                 |
| M3D_ERR_SHPE       | too many or no chunk of this kind                 |
| M3D_ERR_VOXT       | too many or no chunk of this kind                 |

The importer tries to load as much as possible. Not all errors are fatal; for example if a material property can't be read, the
importer will continue parsing with other chunks. Therefore `m3d_t.errcode` contains only the very last error code. Non fatal
errors have smaller codes than -65. There's a `M3D_ERR_ISFATAL(x)` macro to tell you if an error was fatal or not.

The file format can store points, lines, triangles, quads and other polygons up to 15 sides. The importer and exporter however
only supports triangle meshes for now, and it will return `M3D_ERR_UNKMESH` for other polygons.

If a model has procedural surface, then you have to bind a face script interpreter to the library. Without one, `M3D_ERR_UNIMPL`
will be returned.

If a model has procedural texture, and you haven't bound a texture script interpreter, then `M3D_ERR_UNKIMG` will be returned.

If the model can't be loaded with proper precision (like it has double coordinates but the loader was compiled with floats, or
there are more bones per vertex records in skin definition than what's configured), then `M3D_ERR_TRUNC` will be set.

Traversing the In-Memory Model Struct
-------------------------------------

The C API allows you the read and modify the struct directly. Since it does not contain anything else than struct arrays, it makes
no sense to create functions for them, and accessing arrays directly is very-very fast. Performance was more crutial here than to
tie the programmer's hands. The C++ wrapper on the other hand provides getter/setter methods which convert those arrays into
std::vector.

The arrays may be `NULL`, which means that the corresponding chunk was not defined in the model. There's no mandatory chunk, all
of them are optional. It is possible that one model contains only material definitions, and another only model surface.

For C++ objects, there's an extra getter to get the C struct. This returns the same pointer as `m3d_load()`, so that you can use
the C traversing method in C++.
```c
m3d_t *M3D::Model.getCStruct();
```

### Get Model Meta Information

```c
char *m3d_t.name
std::string M3D::Model.getName()
```
Returns the unique name of the model in an UTF-8 encoded string.

```c
char *m3d_t.license
std::string M3D::Model.getLicense()
```
Returns the usage condition or license of the model, like "MIT", "LGPL" or "BSD-3clause"

```c
char *m3d_t.author
std::string M3D::Model.getAuthor()
```
Returns the nickname, email, homepage or github URL etc. of the author in an UTF-8 string.

```c
char *m3d_t.desc
std::string M3D::Model.getDescription()
```
Returns the description of the model in an UTF-8 string. Unlike the others, this string may contain '\n' newline character.

```c
float m3d_t.scale
float M3D::Model.getScale()
```
Returns the size of half the model's bounding cube in SI meters. In other words, 1.0 in model-space equals to this scale
in meters in world-space.

```c
m3di_t *m3d_t.preview
std::vector<unsigned char> M3D::Model.getPreview()
```
Returns the uncomressed PNG preview image of the model if exists, or NULL.

### Get Vertex and Material Data

```c
uint32_t m3d_t.numcmap / uint32_t *m3d_t.cmap
std::vector<uint32_t> M3D::Model.getColorMap()
```
Returns the color map or NULL. Usually you don't need this, as colors in vertex list and materials are already translated in the
arrays.

```c
typedef struct {
    float u;
    float v;
} m3dti_t;

uint32_t m3d_t.numtmap / m3dti_t *m3d_t.tmap
std::vector<m3dt_t> M3D::Model.getTextureMap()
```
Returns the texture map UV coordinates map or NULL.

```c
typedef struct {
    char *name;                 /* texture name */
    uint8_t *d;                 /* pixels data */
    uint16_t w;                 /* width */
    uint16_t h;                 /* height */
    uint8_t f;                  /* format, 1 = grayscale, 2 = grayscale+alpha, 3 = rgb, 4 = rgba */
} m3dtx_t;

uint32_t m3d_t.numtexture / m3dtx_t *m3d_t.texture
std::vector<m3dtx_t> M3D::Model.getTextures()
std::string M3D::Model.getTextureName(int idx)
```
Returns the uncompressed / generated textures or NULL.

```c
typedef struct {
    float x;                    /* 3D coordinates and weight */
    float y;
    float z;
    float w;
    uint32_t color;             /* default vertex color */
    uint32_t skinid;            /* skin index */
} m3dv_t;

uint32_t m3d_t.numvertex / m3dv_t *m3d_t.vertex
std::vector<m3dv_t> M3D::Model.getVertices()
```
Returns the unique vertices or NULL. Skinid can be -1 (not bone related vertex) or -2 (encodes an orientation quaternion).

```c
typedef struct {
    char *name;                 /* name of the material */
    uint32_t numprop;           /* number of properties */
    m3dp_t *prop;               /* properties array */
} m3dm_t;

uint32_t m3d_t.nummaterial / m3dm_t *m3d_t.material
std::vector<m3dm_t> M3D::Model.getMaterials()
std::string M3D::Model.getMaterialName(int idx)
```
Returns list of materials or NULL. There's an extra method for C++ to return the material's name as std::string.

```c
typedef struct {
    uint8_t type;               /* property type, see "m3dp_*" enumeration */
    union {
        uint32_t color;         /* if value is a color, m3dpf_color */
        uint32_t num;           /* if value is a number, m3dpf_uint8, m3pf_uint16, m3dpf_uint32 */
        float    fnum;          /* if value is a floating point number, m3dpf_float */
        uint32_t textureid;     /* if value is a texture, m3dpf_map */
    } value;
} m3dp_t;

uint32_t m3d_t.material[i].numprop / m3dp_t *m3d_t.material[i].prop
uint32_t M3D::Model.getMaterialPropertyColor(int materialidx, int type)
int M3D::Model.getMaterialPropertyInt(int materialidx, int type)
float M3D::Model.getMaterialPropertyFloat(int materialidx, int type)
m3dtx_t* M3D::Model.getMaterialPropertyMap(int materialidx, int type)
```
Return a material property. The C++ version has four methods, for types greater or equal than 128 (or other words for enums which
start with `m3dp_map_*`) the getMaterialProperty *Map* () variant must be used. That returns a pointer to a texture map. Others
must use the getMaterialProperty *Int* (), getMaterialProperty *Color* () or getMaterialProperty *Float* () variants. If the
property not found, they return NULL or -1 (0xFFFFFFFF) respectively. The property definitons are expandable, see
`m3d_propertytypes` static array in m3d.h. The first column specifies the format, the second corresponds to the m3dp_t.type field,
and the third one is used in the text files.

```c
typedef struct {
    char *name;                 /* texture name (same pointer as in texture[].name) */
    uint32_t length;            /* compressed data length */
    uint8_t *data;              /* compressed texture data */
} m3di_t;

uint32_t m3d_t.numinlined / m3di_t *m3d_t.inlined
std::vector<m3di_t> M3D::Model.getInlinedAssets()
```
Returns inlined assets or NULL. Normally you don't need to access inlined assets, as they are used transparently when the
model is loaded.

### Get Model Face

```
typedef struct {
    uint32_t materialid;        /* material index */
    uint32_t vertex[3];         /* 3D points of the triangle in CCW order */
    uint32_t normal[3];         /* normal vectors */
    uint32_t texcoord[3];       /* UV coordinates */
    uint32_t paramid;           /* parameter index, only with M3D_VERTEXMAX */
    uint32_t vertmax[3];        /* maximum 3D points of the triangle, only with M3D_VERTEXMAX */
} m3df_t;

uint32_t m3d_t.numface / m3df_t *m3d_t.face
std::vector<m3df_t> M3D::Model.getFace()
```
Returns the triangles which makes up the model's face. Material id of -1 (or 0xFFFFFFFF) denotes no material, revert to vertex
colors. Otherwise it indexes an element from the m3d_t.material array. The same stands for all the other indices, -1 means unset,
other values index an element from their corresponding arrays. The mesh is the primary face if shapes are not defined
(`m3d_t.numshape` is zero).

(Note: there's no numproc / proc because those scripts generate into numface / face.)

```
typedef struct {
    uint16_t count;
    char *name;
} m3dvi_t;

typedef struct {
    char *name;                 /* technical name of the voxel */
    uint8_t rotation;           /* rotation info */
    uint16_t voxshape;          /* voxel shape */
    M3D_INDEX materialid;       /* material index */
    uint32_t color;             /* default voxel color */
    M3D_INDEX skinid;           /* skin index */
    uint8_t numitem;            /* number of sub-voxels */
    m3dvi_t *item;              /* list of sub-voxels */
} m3dvt_t;

uint16_t m3d_t.numvoxtype / m3dvt_t *m3d_t.voxtype
std::vector<m3dvt_t> getVoxelTypes()
std::string getVoxelTypeName(int idx)
std::vector<m3dvi_t> getVoxelTypeItems(int idx)
```
Returns information on voxel types, indexed by `M3D_VOXEL` in voxel block's data.
```
typedef struct {
    char *name;                 /* name of the block */
    int32_t x, y, z;            /* position */
    uint32_t w, h, d;           /* dimension */
    uint8_t uncertain;          /* probability */
    uint8_t groupid;            /* block group id */
    M3D_VOXEL *data;            /* voxel data, indices to voxel type */
} m3dvx_t;

uint32_t m3d_t.numvoxel / m3dvx_t *m3d_t.voxel
std::vector<m3dvx_t> getVoxelBlocks()
std::string getVoxelBlockName(int idx)
std::vector<M3D_VOXEL> getVoxelBlockData(int idx)
```
When the model is made up of standard sized cubes aligned on a regular 3D grid then the model is stored more efficiently using
voxels. These can be queried with these arrays / methods, however voxels are automatically converted to numface / face if
`M3D_NOVOXELS` is not defined. Voxels can be mixed with mesh face.

```
typedef struct {
    char *name;                 /* name of the mathematical shape */
    uint32_t group;             /* group this shape belongs to or -1 */
    uint32_t numcmd;            /* number of commands */
    m3dc_t *cmd;                /* commands array */
} m3dh_t;

uint32_t m3d_t.numshape / m3dh_t *m3d_t.shape
std::vector<m3dh_t> M3D::Model.getShape()
std::string M3D::Model.getShapeName(int idx)
unsigned int M3D::Model.getShapeGroup(int idx)
```
Returns the parameterized mathematical formulas to generate the model's face. Each command looks like:
```
typedef struct {
    uint16_t type;              /* shape type */
    uint32_t *arg;              /* arguments array */
} m3dc_t;

uint32_t m3d_t.shape[idx].numcmd / m3dc_t *m3d_t.shape[idx].cmd
std::vector<m3dc_t> M3D::Model.getShapeCommands(int idx)
```
The recognized commands are defined in the `m3d_commandtypes[]` static array. Because the argument list is universal, you
have to cast its elements according to the command specified by type. These commands describe for example NURBS surfaces.
If `m3d_t.numshape` is greater than zero, then the primary face of the model is the shape list, and not the mesh (there's a
command for including the mesh into shape-space).

### Get Annotations

```
typedef struct {
    char *name;                 /* name of the annotation layer or NULL */
    char *lang;                 /* language code or NULL */
    char *text;                 /* the label text */
    uint32_t color;             /* color */
    uint32_t vertexid;          /* the vertex the label refers to */
} m3dl_t;

uint32_t m3d_t.numlabel / m3dl_t *m3d_t.label
std::vector<m3dl_t> M3D::Model.getAnnotationLabels()
```
Returns the annotation labels list or NULL.

### Get Action Animations

```c
typedef struct {
    uint32_t boneid[M3D_NUMBONE];
    float    weight[M3D_NUMBONE];
} m3ds_t;

uint32_t m3d_t.numskin / m3ds_t *m3d_t.skin
std::vector<m3ds_t> M3D::Model.getSkin()
```
Returns the skin definition, bone id and weight pairs or NULL. The skinid is included in the vertex list.

```c
typedef struct {
    M3D_INDEX vertexid;
    M3D_FLOAT weight;
} m3dw_t;

typedef struct {
    uint32_t parent;            /* parent bone index */
    char *name;                 /* name of the bone */
    uint32_t pos;               /* vertex index position */
    uint32_t ori;               /* vertex index orientation (quaternion) */
    uint32_t numweight;         /* number of controlled vertices */
    m3dw_t *weight;             /* weights for those vertices */
    M3D_FLOAT mat4[16];         /* transformation matrix */
} m3db_t;

uint32_t m3d_t.numbone / m3db_t *m3d_t.bone
std::vector<m3db_t> M3D::Model.getBones()
std::string M3D::Model.getBoneName(int idx)
```
Returns the bone hierarchy and the bind-pose skeleton or NULL. Note: bones are indexed, and there's an explicit bone -1
(or 0xFFFFFFFF), the bounding cube which is not included here. Top level bones has the parent of -1. Matrix is coloumn
major, and transforms from model-space into bone-local space. See also `m3d_pose()` and `M3D::Model.getActionPose()` below.

```c
typedef struct {
    char *name;                 /* name of the action */
    uint32_t durationmsec;      /* duration in millisec (1/1000 sec) */
    uint32_t numframe;          /* number of frames in this animation */
    m3dfr_t *frame;             /* frames array */
} m3da_t;

uint32_t m3d_t.numaction / m3da_t *m3d_t.action
std::vector<m3dm_t> M3D::Model.getActions()
std::string M3D::Model.getActionName(int aidx)
unsigned int M3D::Model.getActionDurationMsec(int aidx)
std::vector<m3dfr_t> M3D::Model.getActionFrames(int aidx)
```
Returns list of actions or NULL. Extra methods for C++ to return the action's name as std::string and it's duration
in millisec, as well as the frames in a vector. You can also query the frame's timestamp and the list of transformations.

```c
typedef struct {
    uint32_t boneid;            /* selects a node in bone hierarchy */
    uint32_t pos;               /* vertex index new position */
    uint32_t ori;               /* vertex index new orientation (quaternion) */
} m3dtr_t;

typedef struct {
    uint32_t msec;              /* frame's position on the timeline, timestamp */
    uint32_t numtransform;      /* number of transformations in this frame */
    m3dtr_t *transform;         /* transformations */
} m3dfr_t;

uint32_t m3d_t.action[aidx].numframe / m3dfr_t *m3d_t.action[aidx].frame
uint32_t m3d_t.action[aidx].frame[fidx].numtransform / m3dtr_t *m3d_t.action[aidx].frame[fidx].transform
unsigned int M3D::Model.getActionFrameTimestamp(int aidx, int fidx)
std::vector<m3dtr_t> M3D::Model.getActionFrameTransforms(int aidx, int fidx)
```
Each frame in turn is a list of transformations, where each transformation contains a bone id and a new position and orientation.
If bone id refers to a bone which has children in the hierarchy, then the transformation applies to those children too.

The API provides two more functions / methods for converting frame transformations into animated-pose skeletons:
```c
m3dtr_t *m3d_frame(m3d_t *model, uint32_t actionid, uint32_t frameid, m3dtr_t *skeleton);
std::vector<m3dtr_t> M3D::Model.getActionFrame(int aidx, int fidx, std::vector<m3dtr_t> skeleton);
m3db_t *m3d_pose(m3d_t *model, uint32_t actionid, uint32_t msec);
std::vector<m3db_t> M3D::Model.getActionPose(int aidx, unsigned int msec);
```
The m3d_frame() / getActionFrame() method receives a working copy of the skeleton (or NULL / nullptr on first call), and
calculates the animation-pose skeleton for that action's given frame. The m3d_pose() / getActionPose() method returns the same
bone list as bone / getBones(), but in an animated pose and with opposite direction matricies. For both functions you must free
the returned arrays (simple free() or delete, no special threatment needed.)

### Get Application / Engine Specific Data Chunks

```c
typedef struct {
    char magic[4];
    uint32_t length;
} m3dchunk_t;

uint32_t m3d_t.numextra / m3dchunk_t **m3d_t.extra
std::vector<std::unique_ptr<m3dchunk_t>> M3D::Model.getExtras()
```
Return extra (unparsed) chunks. The first character, magic\[0] should *not* be an uppercase Latin letter (a-z, symbols etc. only),
to avoid accidental collision with standardized chunks. If a chunks turns out to be useful, it may be included in the later editions
of the Model 3D specification with full uppercase magic. The (length - 8) bytes long data which follows the header is totally up
to the engine to parse. Most probably it should cast these chunks with some struct typedef depending on their magic value.

Non-Public API
--------------

These are private functions, you should not call them directly. However if you're writing a model converter or exporter, they
can be very handy.

```c
char *_m3d_safestr(char *in, int morelines);
```
Allocates a new buffer for a string, and saves sanitized string into it. Morelines tells how:
- 0: no line breaks, special characters and spaces replaced, resulting string is usable for an identifier or for a filename
- 1: line break allowed and converted into "\n", like description in binary files
- 2: no line breaks, but special characters allowed, like license and author's name with urls
- 3: line break allowed and converted into "\r\n", like description in ASCII files

```c
uint32_t _m3d_gettx(m3d_t *model, m3dread_t readfilecb, m3dfree_t freecb, char *fn);
```
Reads in and decodes a texture (or executes a script to generate it), storing the uncompressed pixel buffer into the
model->texture array and returning it's index.

```c
void _m3d_getpr(m3d_t *model, m3dread_t readfilecb, m3dfree_t freecb, char *fn);
```
Reads in and executes a procedural surface script, which should add vertices and faces to the model.

```c
typedef struct {
    char *str;
    uint32_t offs;
} m3dstr_t;
m3dstr_t *_m3d_addstr(m3dstr_t *str, uint32_t *numstr, char *s);
```
Adds a string to the list, keeping the elements unique.

```c
m3dhdr_t *_m3d_addhdr(m3dhdr_t *h, m3dstr_t *s);
```
Adds a string to the model header chunk, calculating it's string table offset.

```c
uint32_t _m3d_stridx(m3dstr_t *str, uint32_t numstr, char *s);
```
Returns the offset of a string in string table.

```c
uint32_t *_m3d_addcmap(uint32_t *cmap, uint32_t *numcmap, uint32_t color);
```
Adds a color to the color map, making sure that it's unique and the color map kept sorted by HSV value.

```c
uint32_t _m3d_cmapidx(uint32_t *cmap, uint32_t numcmap, uint32_t color);
```
Returns the index of a color in the color map.

```c
char *_m3d_prtbone(char *ptr, m3d_t *model, uint32_t parent, uint32_t level);
```
Recursively generate the UTF-8 string representation of the bone hierarchy.

```c
void _m3d_mul(float *r, float *a, float *b);
```
Performs a matrix multiplication 'r' = 'a' x 'b'.

```c
void _m3d_inv(float *m);
```
Calculates the matrix inverse in place.

```c
void _m3d_mat(float *r, m3dv_t *p, m3dv_t *q);
```
Constructs a 4x4 transformation matrix from a position and a quaternion.

```c
float _m3d_rsq(float x);
```
John Carmack's fast inverse square root calculation. If included with `M3D_DOUBLE`, then a close approximation of 1/sqrt(x).

