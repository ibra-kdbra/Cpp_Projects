Model 3D SDK Usage
==================

This is the API manual. For the function reference, see the [API](https://gitlab.com/bztsrc/model3d/blob/master/docs/API.md) documentation.

[[_TOC_]]

Preface
-------

The M3D SDK is following the K.I.S.S. principle (keep it simple, stupid). Therefore it's a single ANSI C/C++ header file which
provides 16 typedef structs (this includes material properties, shapes and animations as well) and only 5 functions:
- 1 to decode a compressed model
- 1 to free an in-memory model
- 1 to encode an in-memory model
- 2 to aid animated models

It does not hide the in-memory model behind complex node trees and funny methods. It provides full access to the in-memory
structure, because it is YOUR model, after all. Use as you see best fit in your engine. Everything is indexed, so fast access
guaranteed. The structure was designed in a way so that for static models you can directly feed it into VBO / EBO lists.

By default, only the C API is included. To get the C++ wrapper that uses std::string and std::vector, define `M3D_CPPWRAPPER`.
Note that there's absolutely no logic in the C++ class, string and vector is only used as a container for C variables. This means
that the code only uses their constructors in the "return" statement. If you want, you can do the same in your own wrapper
class that could use any container class.

All typedefs have the form `m3dX_t`. Normally `X` is one or two letters, for example `a` for action, `b` for bone, `m` for
material etc. However there are long names defined too if you'd prefer more readable code.

Usage Overview
--------------

The API uses the simpliest approach possible with minimal functions. There's only one function (m3d_load) to parse the binary
Model 3D into an easily traversable in-memory structure.

```
+===================+===========================+-----------------------+
|                   |                           |                       |
|               M3D::Model()                    |                       |
| binary blob   m3d_load()                      |             shader    |
| .m3d or   --------+----->   m3d_t       (your renderer)     buffers   |
| .a3d              |       in-memory ----------+-----------> / OpenGL  |
| text file <-------+------   format   (traverse or getters)  / Vulkan  |
|               m3d_save()      |           m3d_frame()       / DirectX |
|               M3D::Save()     |      M3D::getActionFrame()            |
|                   |           |               |                       |
|                   |           | m3d_free()    |                       |
|                   |           | ~M3D::Model() |                       |
|                   |           V               |                       |
|                   |          NULL             |                       |
|                   |                           |                       |
+== Distribution ===+========= CPU RAM =========+------- GPU RAM -------+
```

Thread-Safety
-------------

All functions operate excusively on a context passed to them, no global variables used at all. Therefore the API is entirely
thread-safe, regardless to thread implementation. Just don't pass the same context to it in two different threads at the same
time, and everything will be fine.

The ASCII format relies on libc to format floating point numbers. To do that consistently, both the importer and the exporter
temporarily sets the number format locale to C. This means you should not set locales during text-based model loads and saves,
otherwise the result will be undefined. This limitation is very reasonable (I can't think of any valid reason why would anyone
change the locale in a separate thread during loading or saving models in debug format), and allows much smaller code base and
a portable and efficient implementation at the same time. The binary format is not affected by the locale.

Execution Time
--------------

The importer is all about speed, even if that means more memory. The exporter is quite the contrary, speed there doesn't matter,
the resulting memory footprint does.

The format eagers to store pre-processed values to make model loading as fast as possible. There're no miracles, those
processing has to be done somewhere. M3D SDK chooses to do those in the exporter, meaning saving into Model 3D can be slow
(but in return, loading is always lightning fast). A game ready, low polygon count model (few thousand polygons tops) should be
exported in no time.

Loading Models
--------------

The SDK does not care about file systems. It expects the model file's content to be loaded into memory apriori, therefore you
can use any source you want. Models can be directly linked into your application, they can be served from a static buffer (like
a tar, cpio or zip archive you read from the file system or linked with your application), or simply loaded from a file.

Now once you have the model, you should decompress it into an in-memory structure. That's as simple as:
```c
/* C */
m3d_t *myModel = m3d_load(&ucharbuffer, NULL, NULL, NULL);

/* C++ */
std::vector<unsigned char> buffer(&ucharbuffer, &ucharbuffer + sizeof(ucharbuffer));
M3D::Model myModel = new M3D::Model(buffer, NULL, NULL, NULL);
```

The input buffer can contain a binary raw model, or, if compiled with `M3D_ASCII`, a zero terminated string with optional UTF-8
characters. There are some minimal checks, but in general you are expected to pass only valid input to the loader, otherwise you'll
get undefined behaviour. You can use the [m3dconv](https://gitlab.com/bztsrc/model3d/tree/master/m3dconv) utility to validate
and verify the raw models in compilation time (and you can include the validator into your code for run-time validation, separately
to the loader).

Don't free the input buffer `ucharbuffer` before an md3_free() call, because the returned m3d_t structure may contain pointers
into that buffer. The m3d_t structure tries to be as memory efficient as possible, therefore it does not duplicate buffers if it's
not absolutely necessary (this does not apply to stream compressed binary or ASCII format models, you can free those as soon
as m3d_load() returned).

Some models may refer to external assets. For that the SDK requires a file read callback. It is recommended to simply use the
same function you have used to load the model itself. But it can be different, for example you can load the models from the game
directory, and the external assets (textures) from a modpack directory. The callback's prototype is pretty simple and minimal:
```c
extern "C" unsigned char *readfile(char *filename, unsigned int *size);
```
It should look up `filename`, and return the size in `size` and it's contents in a buffer. If file not found, it must return
NULL and size of 0. Simple. The SDK calls this with filenames only, you can add the path (or paths) to be searched for in the
function. For C++, you must declare the callback with `extern "C"`, but that's all.
```c
/* C */
m3d_t *myModel = m3d_load(&ucharbuffer, readfile, free, NULL);

/* C++ */
std::vector<unsigned char> buffer(&ucharbuffer, &ucharbuffer + sizeof(ucharbuffer));
M3D::Model myModel = new M3D::Model(buffer, readfile, free, NULL);
```
The `free` callback is only needed if your readfile callback has allocated the returned buffer. For static buffers, free can be
omited.

Here's a read file callback example which simply reads files from the current working directory:
```c
/* C */
unsigned char *readfile(char *filename, unsigned int *size)
{
    FILE *f;
    unsigned char *data = NULL;

    f = fopen(filename, "rb");
    if(f) {
        fseek(f, 0L, SEEK_END);
        *size = (unsigned int)ftell(f);
        fseek(f, 0L, SEEK_SET);
        data = (unsigned char*)malloc(*size + 1);
        if(data) {
            fread(data, *size, 1, f);
            data[*size] = 0;
        } else
            *size = 0;
        fclose(f);
    }
    return data;
}
```
It's not mandatory, but it's better to add a terminator zero at the end of the buffer. This is useful if the model is in ASCII
format.

Using Material Libraries
------------------------

With the `m3dconv -m` tool you can remove material definitions from models and collect them into a single separate file. To
use that, you have to first load that material library model, then pass it to the other object models.
```c
/* C */
m3d_t *mtllib = m3d_load(&materialsonlymodel, NULL, NULL, NULL);

m3d_t *myObj1 = m3d_load(&objectmodel1, NULL, NULL, mtllib);
m3d_t *myObj2 = m3d_load(&objectmodel2, NULL, NULL, mtllib);

/* do something with the models */

m3d_free(myObj1);
m3d_free(myObj2);

m3d_free(mtllib);

/* C++ */
M3D::Model mtllib = new M3D::Model(materialsonlyucharvector, NULL, NULL, NULL);

M3D::Model muObj1 = new M3D::Model(objmodel1vector, NULL, NULL, mtllib);
M3D::Model muObj2 = new M3D::Model(objmodel2vector, NULL, NULL, mtllib);

delete myObj1;
delete myObj2;

delete mtllib;
```
It is important to free the shared material library last.

Traversing the In-Memory Model Struct
-------------------------------------

The philosophy of the SDK is not to hide the structure behind cryptic functions and methods. Instead it provides a simple and
intuitively named structure list to traverse. It allows the developer to directly access everything, after all, it is YOUR model.
For easier integration, the C++ wrapper provides getters and setters, but you are better off with just using the struct arrays,
much-much faster and requires less memory (the wrapper std::vector allocates some extra memory). For using the C struct, you can
query it from a C++ object with:
```c
/* C++ */
m3d_t *myModel_c = myModel->getCStruct();
```

The easiest way to dump those structures programatically to console is to include a header file `m3dconv/dump.h`, and call
`dump_cstruct(myModel)`.

Note that in the following examples `printf` is just an example to showcase accessing all the struct members.

### Meta Information

First and most of all, you want to get the model's name.
```c
/* C */
char *model_name = myModel->name;

/* C++ */
std::string model_name = myModel->getName();
```
Similarly, you can access the model's license, the name and contact of its author, and a comment and description on what
that model is about. For C, you have the field (in order) `license`, `author`, `desc`; for C++ you have the getters
`getLicense()`, `getAuthor()`, `getDescription()` all returning std::string.

The model also stores a scaling factor specified in SI meters. This is very handly if you want to render more models next to each
other to scale. This equals to the half of the model's bounding cube's size, meaning it equals to the size of 1.0 in model-space.
```c
/* C */
float model_size_in_meters = myModel->scale;

/* C++ */
float model_size_in_meters = myModel->getScale();
```

### Getting Vertices

This is very simple. Color codes are uint32_t RGBA values, where red is the least significant byte. Without a material
(see below), a renderer should revert to these diffuse colors.

`m3dv_t` / `m3d_vertex_t`:

| Type     | Name   | Desciprition          |
|----------|--------|-----------------------|
| float    | x      | the X coordinate      |
| float    | y      | the Y coordinate      |
| float    | z      | the Z coordinate      |
| float    | w      | weight                |
| uint32_t | color  | diffuse color         |
| uint32_t | skinid | skin index            |

```c
/* C */
for(i = 0; i < myModel->numvertex; i++) {
    printf(" vertexid %d coordinate (%g,%g,%g,%g) color #%08x\n"
        i,
        myModel->vertex[i].x,
        myModel->vertex[i].y,
        myModel->vertex[i].z,
        myModel->vertex[i].w,
        myModel->vertex[i].color);
}

/* C++ */
std::vector<m3dv_t> myModel->getVertices();
```
You can use the vertex weight in your shader if you want, however most engines just sets it as 1.0.

Coordinates are always normalized to -1.0 .. 1.0 in the vertex list when loaded. To get the model's effective
coordinates, you have to multiply `x`, `y`, `z` with `myModel->scale`. Don't modify the vertex list itself, do
the scaling when you copy the vertices into a VBO. For animation, you'll have to have a separate vertex list
anyway, because you'll modify them by interpolated bone pose, and you have to keep the original vertices for
calculating the next frame (see below under section Animation).

The coordinates are encoded as +Y is up, +X is on the right and +Z is towards the viewer. The model is oriented in this
coordinate system to facing you.

### Getting the Textures

Uncompressed textures can be accessed in an uniform way, regardless if they were inlined, loaded as external assets or
[generated](https://gitlab.com/bztsrc/model3d/blob/master/docs/procedural.md) on-the-fly. The SDK also has done the favour to decode
PNGs for you without external libraries. The C++ wrapper has a getter which receives a texture id and returns the texture name
as std::string.

`m3dtx_t` / `m3d_texturedata_t`:

| Type      | Name | Desciprition        |
|-----------|------|---------------------|
| char*     | name | name of the texture |
| uint8_t*  | d    | pixel data array    |
| uint16_t  | w    | width               |
| uint16_t  | h    | height              |
| uint8_t   | f    | format              |

```c
/* C */
for(i = 0; i < myModel->numtexture; i++) {
    printf("textureid %d. name %s, dimensions %d x %d, numcomponent %d, pixelbuf %p\n",
        i,
        myModel->texture[i].name,
        myModel->texture[i].w,
        myModel->texture[i].h,
        myModel->texture[i].f,
        myModel->texture[i].d);
}

/* C++ */
std::vector<m3dtx_t> textures = myModel->getTextures();
std::string first_textures_name = myModel->getTextureName(0);
```
Pixel data is typically RGBA (where the least significant byte is the red component), when `f` is 4. With 2, each pixel will
be a grayscale plus alpha channel. Finally, with `f` being 1, each pixel is a grayscale pixel. The size of `d` array is always
`w` * `h` * `f`.

Now parts of the textures are mapped by coordinates. Model faces are referencing those by indices, called texture map index. To
query those mappings, use:

`m3dti_t` / `m3d_textureindex_t`:

| Type      | Name | Desciprition        |
|-----------|------|---------------------|
| float     | u    | the X coordinate    |
| float     | v    | the Y coordinate    |

```c
/* C */
for(i = 0; i < myModel->numtmap; i++) {
    printf("texture coordinate map id %d, coordinates (%g, %g)\n",
        i,
        myModel->tmap[i].u,
        myModel->tmap[i].v);
}

/* C++ */
std::vector<m3dti_t> tmaps = myModel->getTextureMap();
```
The SDK handles UV as (0,0) is the upper left corner of the texture. When you're rendering in OpenGL, you'll have to
invert V (the Y coordinate) and pass it as `1.0 - myModel->tmap[i].v` to the VBO.

### Getting the Materials (PBR-compliant)

This is very simple, and goes just like the textures:

`m3dm_t` / `m3d_material_t`:

| Type      | Name    | Desciprition            |
|-----------|---------|-------------------------|
| char*     | name    | name of the material    |
| uint32_t  | numprop | number of properties    |
| m3dp_t*   | prop    | property list           |

```c
/* C */
for(i = 0; i < myModel->nummaterial; i++) {
    printf("materialid %d, name %s, number of properties %d, property list %p\n",
        i,
        myModel->material[i].name,
        myModel->material[i].numprop,
        myModel->material[i].prop);
}

/* C++ */
std::vector<m3dm_t> materials = myModel->getMaterials();
```
Most SDKs provide a complex system to get the material properties. Model 3D does not complicate things.

`m3dp_t` / `m3d_property_t`:

| Type      | Name    | Desciprition            |
|-----------|---------|-------------------------|
| uint8_t   | type    | type of the property    |
| union     | value   | property value          |

However interpreting property value depends on the property's type. For example, `m3dp_Kd` stores the diffuse color, so `value` is
a color, and can be accessed with `value.color`. Colors are always stored in RGBA format, where red is the least significant byte.
Similairly, `m3dp_il` stores the illumination model's number, so `value.num` should be used. The specular exponent `m3dp_Ns` is a
float, accessed by `value.fnum`. The properties that store textures, like the ambient color map `m3dp_map_Ka`, has
`value.textureid` which indexes the `myModel->texture` array:
```c
m3dtx_t *materialtexture = myModel->texture[ myModel->material[mi].prop[i].value.textureid ];
```
The C++ wrapper has five getters here, `getMaterialName()`, and for the material properties `getMaterialPropertyColor()`,
`getMaterialPropertyInt()`, `getMaterialPropertyFloat()` and `getMaterialPropertyMap()`, all expecting a material id and the last
four a property type too. They iterate through the property list looking for the value of that particular type. The first two
return an integer, the third a float, the fourth returns the `m3dtx_t` texture pointer, already looked up in the textures array.

```c
/* C */
m3dm_t *mat = myModel->material[materialid];

for(i = 0; i < mat->numprop; i++) {
    printf("property %d, type %d ", i, mat->prop[i].type);
    switch(mat->prop[i].type) {
        case m3dp_Kd:     printf(" diffuse color #%08x",   mat->prop[i].value.color); break;
        case m3dp_Ka:     printf(" ambient color #%08x",   mat->prop[i].value.color); break;
        case m3dp_il:     printf(" illumination model %d", mat->prop[i].value.num); break;
        case m3dp_Ns:     printf(" specular exponent %g",  mat->prop[i].value.fnum); break;
        case m3dp_map_Km: printf(" bump map textureid %d", mat->prop[i].value.textureid); break;
    }
}

/* C++ */
std::string first_materials_name =              myModel->getMaterialName(0);
uint32_t    first_materials_diffuse_color =     myModel->getMaterialPropertyColor(0, m3dp_Kd);
int         first_materials_illum_model =       myModel->getMaterialPropertyInt(0, m3dp_il);
float       first_materials_specular_exponent = myModel->getMaterialPropertyFloat(0, m3dp_Ns);
m3dtx_t    *first_materials_ambient_map =       myModel->getMaterialPropertyMap(0, m3dp_map_Ka);
```

If you're unsure what format a property has, or if you want to iterate through all the properties, then a static array
`m3d_propertytypes` cames to the rescue. You should search for the property `type` in that array matching the `id` field, and
then the `format` field tells you how to interpret that particular property type. The material property system of Model 3D is
very logical, types 0 - 127 encode scalar values, and 128 - 255 their corresponding mappings. For example `m3dp_Pm` is the
metallic property, then `m3dp_Pm` + 128 is `m3dp_map_Pm` which is the metallic texture map. Therefore `m3d_propertytypes` table
should only contain ids 0 - 127, because mapping types can be easily calculated, although it has a few mapping types too for
aliases (like `m3dp_map_Km` is the same as `m3dp_bump`).

```c
/* C */
m3dm_t *mat = myModel->material[materialid];

for(i = 0; i < mat->numprop; i++) {
    printf("property %d, type %d ", i, mat->prop[i].type);
    /* dynamically look up property format */
    if(mat->prop[i].type & 0x80) {
        printf(                               " textureid %d\n", mat->prop[i].value.textureid);
    } else {
        for(j = 0; j < sizeof(m3d_propertytypes)/sizeof(m3d_propertytypes[0]); j++) {
            if(m3d_propertytypes[j].id == mat->prop[i].type) {
                switch(m3d_propertytypes[j].format) {
                    case m3dpf_color:  printf(" color #%08x\n",  mat->prop[i].value.color); break;
                    case m3dpf_float:  printf(" float %g\n",     mat->prop[i].value.fnum); break;
                    case m3dpf_uint8:
                    case m3dpf_uint16:
                    case m3dpf_uint32: printf(" int %d\n",       mat->prop[i].value.num); break;
                }
            }
        }
    }
}

/* C++ */
// NOTE: it is not possible to iterate through the properties with C++, you have to know
// the property's format in advance, but that shouldn't be a problem in the first place
```
To get the material properties for a Disney-like Physically Based Renderer (PBR) shader, you'll need these:

| Type | Name         | Desciprition                                              |
|-----:|--------------|-----------------------------------------------------------|
|    0 | m3dp_Kd      | overall base diffuse color (albedo)                       |
|  128 | m3dp_map_Kd  | base color map                                            |
|    1 | m3dp_Ka      | overall ambient color                                     |
|  129 | m3dp_map_Ka  | ambient occlusion color map                               |
|    2 | m3dp_Ks      | overall specular color                                    |
|  130 | m3dp_map_Ks  | specular color map                                        |
|    4 | m3dp_Ke      | overall emissive color                                    |
|  132 | m3dp_map_Ke  | emissive color map                                        |
|  134 | m3dp_map_Km  | height map                                                |
|  136 | m3dp_map_N   | normal map                                                |
|   64 | m3dp_Pr      | overall roughness                                         |
|  192 | m3dp_map_Pr  | roughness map                                             |
|   65 | m3dp_Pm      | overall reflectivity (metalness)                          |
|  193 | m3dp_map_Pm  | reflectivity map                                          |
|   66 | m3dp_Ps      | overall sheen (microsurface)                              |
|  194 | m3dp_map_Ps  | sheen map                                                 |
|   67 | m3dp_Ni      | overall optical density (refraction, IOR, Fresnel-effect) |
|  195 | m3dp_map_Ni  | optical density map                                       |

In general, if a map exists, you should use that, and fallback to the overall property otherwise. The opacity is stored in the
alpha-channel of the base color map. In special cases for backward-compatibility, opacity can also be stored in the dissolve
(m3dp_d or m3dp_map_d) values as a separate alpha-channel, but this is discouraged and should be avoided.

A comprehensive list of all available material property types can be found in the [file format](https://gitlab.com/bztsrc/model3d/blob/master/docs/m3d_format.md)
specification, under section Materials. To get the C/C++ enumeration name from the "Keyword" coloumn, just add the "m3dp_" prefix.

### Get the Face (Triangle Mesh)

Regardless if mesh is stored by data, compressed using voxels or was [generated](https://gitlab.com/bztsrc/model3d/blob/master/docs/procedural.md)
on-the-fly, you can get the triangles in an uniform way. Note that the file format can handle different polygons, the SDK only
supports triangles for now. In general, the index -1 (or 0xFFFFFFF) means not defined. For example if the material id is -1, then
you should use the color in the vertex list. If texcoord\[0] is -1, then the triangle has no texture UV coordinate associated with
it. In the table I've noted the arrays for which the specific indices are. Notes: for normals, you just simply discard the `w`,
`color` and `skinid` fields of the vertex. By default the face is the polygon mesh (if `myModel->numshape` is zero, see below).

`m3df_t` / `m3d_face_t`:

| Type          | Name       | Desciprition                       |
|---------------|------------|------------------------------------|
| uint32_t      | materialid | material index `myModel->material` |
| uint32_t\[3]  | vertex     | position indices `myModel->vertex` |
| uint32_t\[3]  | normal     | normals indices `myModel->vertex`  |
| uint32_t\[3]  | texcoord   | UV indices `myModel->tmap`         |

```c
/* C */
for(i = 0; i < myModel->numface; i++) {
    printf("triangle %d ", i);
    if(myModel->face[i].materialid != -1U)
        printf("materialid %d ", myModel->face[i].materialid);
    for(j = 0; j < 3; j++) {
        printf("pos=%d ", myModel->face[i].vertex[j]);
        if(myModel->face[i].normal[j] != -1U)
            printf("normal=%d ", myModel->face[i].normal[j]);
        if(myModel->face[i].texcoord[j] != -1U)
            printf("uv=%d ", myModel->face[i].texcoord[j]);
    }
    printf("\n");
}

/* C++ */
std::vector<m3df_t> modelface = myModel->getFace();
```

The order of the triangle edges is always counter-clockwise, according to the right-hand rule.

You can access voxel data the same way as other arrays; but this is usually not needed as voxel images are automatically
converted to triangle mesh on load, unless `M3D_NOVOXELS` is defined. This default is ideal for simple renderers and one
model viewers. To take advantage of all the options M3D offers for voxel images, in a game engine for example, you should
convert the voxels into cubes yourself.

For that, you can query the voxel types (or voxel palette) like this:

`m3dvt_t` / `m3d_voxeltype_t`:

| Type          | Name       | Desciprition                         |
|---------------|------------|--------------------------------------|
| char*         | name       | an optional name for the voxel type  |
| uint32_t      | color      | diffuse color if there's no material |
| uint32_t      | materialid | material index `myModel->material`   |
| uint32_t      | skinid     | skin index `myModel->skin`           |

```c
/* C */
for(i = 0; i < myModel->numvoxtype; i++) {
    printf("%d color #%08x name '%s'", i, myModel->voxtype[i].color, myModel->voxtype[i].name);
    if(myModel->voxtype[i].materialid != -1U)
        printf("materialid %d ", myModel->voxtype[i].materialid);
    if(myModel->voxtype[i].skinid != -1U)
        printf("skinid %d ", myModel->voxtype[i].skinid);
    printf("\n");
}

/* C++ */
std::vector<m3dvt_t> modelvoxeltypes = myModel->getVoxelTypes();
std::string first_voxeltype_name =     myModel->getVoxelTypeName(0);
```

Voxel data are groupped into blocks (or layers, however the name "layer" can be confusing here as that may also refer to the
vertical levels. We'll stick to the name "block" for now). Each block has it's own 3D position and size, and an array of voxel
type indices to fill up that area with. Index of -1 (0xFFFF or `M3D_VOXUNDEF`) means voxel is empty (air, not set etc.), and -2
(0xFFFE or `M3D_VOXCLEAR`) means clear to empty. When blocks overlap, and the new block has a -1, then the original voxel should
be kept. With -2 the world's voxel must be replaced by -1 (hence this called clear to empty).

`m3dvx_t` / `m3d_voxel_t`:

| Type          | Name       | Desciprition                          |
|---------------|------------|---------------------------------------|
| char*         | name       | an optional name of the block / layer |
| int32_t       | x, y, z    | position of the block                 |
| uint32_t      | w, h, d    | block size (width, height, depth)     |
| uint16_t[whd] | data       | voxel type indices `myModel->voxtype` |

```c
/* C */
for(i = 0; i < myModel->numvoxel; i++) {
    printf("%d name '%s' x %d y %d z %d w %d h %d d %d\n", i, myModel->voxel[i].name,
        myModel->voxel[i].x, myModel->voxel[i].y, myModel->voxel[i].z,
        myModel->voxel[i].w, myModel->voxel[i].h, myModel->voxel[i].d);
    j = 0;
    for(y = 0; y < sy; y++)                                             /* from bottom to top */
        for(z = 0; z < sz; z++)                                         /* from far to close */
            for(x = 0; x < sx; x++)                                     /* from left to right */
                printf(" %d", myModel->voxel[i].data[j++]);
    printf("\n");
}

/* C++ */
std::vector<m3dvx_t> modelvoxelblocks =       myModel->getVoxelBlocks();
std::string first_voxelblock_name =           myModel->getVoxelBlockName(0);
std::vector<uint16_t> first_voxelblock_data = myModel->getVoxelBlockData(0);
```

#### Alternative to Mesh

The M3D file format can store other than triangle meshes. First, it is capable of storing other polygons than triangles, but
the M3D SDK only handles triangles for now.

Then, if the mesh can be desribed by many small cubes aligned on a grid, voxels can be used (the SDK automatically converts
these into a mesh on load).

Second, there's a completely different way to describe the model than a list of polygons. This mode uses parameterized
mathematical formulas, called shapes. If they are given (number `myModel->numshape` greater than zero), then the primary
face visualization source is the shape list, and not the polygon mesh. These are very similar in structure to materials:

`m3dh_t` / `m3d_shape_t`:

| Type      | Name    | Desciprition            |
|-----------|---------|-------------------------|
| char*     | name    | name of the shape       |
| uint32_t  | group   | bone id of its group    |
| uint32_t  | numcmd  | number of commands      |
| m3dc_t*   | cmd     | command list            |

```c
/* C */
for(i = 0; i < myModel->numshape; i++) {
    printf("shapeid %d (group %d), name %s, number of commands %d, command list %p\n",
        i,
        myModel->shape[i].name,
        myModel->shape[i].group,
        myModel->shape[i].numcmd,
        myModel->shape[i].cmd);
}

/* C++ */
std::vector<m3dh_t> shape = myModel->getShape();
std::string first_shape_name = myModel->getShapeName(0);
unsigned int first_shape_group_boneid = myModel->getShapeGroup(0);
std::vector<m3dc_t> first_shape_commands = myModel->getShapeCommands(0);
```
Similarly to other structs, the C++ wrapper provides three more getters, one for the name, one for the group, and one for the
commands.

`m3dc_t` / `m3d_shapecommand_t`:

| Type      | Name    | Desciprition            |
|-----------|---------|-------------------------|
| uint16_t  | type    | type of the command     |
| uint32_t* | arg     | arguments               |

Here the arguments are determined by the type, similarly to the material properties. The difference is, each command allowed
to have an arbitrary number of arguments, while material properties have only one argument. For example:

Type 0 (use material) has one argument, which is an index to a material.

Type 1 (include shape) has four arguments, shape index, position vertex index, rotation vertex (quaternion) index and scaling
vertex index.

Type 2 (include mesh) has five arguments, start face index, number of polygons, position, rotation and scaling vertex index.

Type 17 (1D Bezier curve control points) has variadic arguments, their number changes for every command.

To use mesh face with shapes, you'll have to include the mesh into shape-space. Use Type 2 with number of -1 to include
the entire mesh. For other types, see the [shape documentation](https://gitlab.com/bztsrc/model3d/blob/master/docs/shape.md). They
describe Bezier surfaces, NURBS and geometrical shapes like spheres and cylinders etc. typically used with
[CAD models](https://gitlab.com/bztsrc/model3d/blob/master/docs/CAD.md). Shapes can be grouped together. This is used when
M3D files are converted from STEP files: each face is converted to a shape, and faces that belong to the same shell will get
the same group bone id.

### Getting the Skeleton

This means actually two things. First, you have to get the bone hierarchy, which tells you which bone is related to which; and next,
you need to get the skin which tells you which bone affects which vertices and how much.

#### Bone Structure

Parent field indexes this same bone list. Top level bones has the parent -1 (or 0xFFFFFFFF). Bone's position is specified by it's
zerus point relative to its parent (the (0,0,0) in the bone's space), and an orientation quaternion. Note there's a trick here,
as `ori` indexes the `myModel->vertex` array too, but you have to interpret that particular vertex as a (w,x,y,z) quaternion in
this case. For convenience, there's a transformation matrix pre-calculated for you. `mat4` is a matrix which transforms from
model-space into bone-local space. This array is describing a static bone structure, a so called __"bind-pose"__ or
__"rest-pose"__ skeleton.

Similarly to textures and materials, the C++ wrapper has a getter which receives the bone id and returns its name.

`m3db_t` / `m3d_bone_t`:

| Type       | Name      | Desciprition                               |
|------------|-----------|--------------------------------------------|
| char*      | name      | name of the bone                           |
| uint32_t   | parent    | parent bone index `myModel->bone`          |
| uint32_t   | pos       | position index `myModel->vertex`           |
| uint32_t   | ori       | orientation `myModel->vertex`              |
| uint32_t   | numweight | number of vertices controlled by this bone |
| m3dw_t*    | weight    | weights for those vertices                 |
| float\[16] | mat4      | model -> bone space transformation matrix  |

```c
/* C */
for(i = 0; i < myModel->numbone; i++) {
    printf("boneid %d. name %s, parent %d, position %d, orientation %d, transformation matrix:\n",
        i,
        myModel->bone[i].name,
        myModel->bone[i].parent,
        myModel->bone[i].pos,
        myModel->bone[i].ori);
    for(j = 0; j < 4; j++) {
        for(k = 0; k < 4; k++)
            printf("%d:%g ", k * 4 + j, myModel->bone[i].mat4[k * 4 + j]);
        printf("\n");
    }
}

/* C++ */
std::vector<m3db_t> bones = myModel->getBones();
std::string first_bones_name = myModel->getBoneName(0);
```
To print out the whole hierarchy, you'll need a recursive function like this:
```c
void printbones(unsigned int parent, unsigned int level) {
    unsigned int i, j;
    assert(level < M3D_BONEMAXLEVEL);
    for(i = parent + 1; i < myModel->numbone; i++) {
        if(myModel->bone[i].parent == parent) {
            for(j = 0; j < level; j++) printf("  ");
            printf("boneid %d. name %s\n", i, myModel->bone[i].name);
            printbones(i, level + 1);
        }
    }
}
printbones(-1U, 0);
```
Example output:
```
boneid 0. name BODY
  boneid 1. name LEFTARM
    boneid 2. name LEFTHAND
      boneid 3. name LEFTFINGER1
      boneid 4. name LEFTFINGER2
      boneid 5. name LEFTFINGER3
      boneid 6. name LEFTFINGER4
      boneid 7. name LEFTFINGER5
  boneid 8. name RIGHTARM
    boneid 9. name RIGHTHAND
      boneid 10. name RIGHTFINGER1
      boneid 11. name RIGHTFINGER2
      boneid 12. name RIGHTFINGER3
      boneid 13. name RIGHTFINGER4
      boneid 14. name RIGHTFINGER5
boneid 15. name HAT
  boneid 16. name FEATHER
```

For convenience, bones has a cross-reference list for the vertices they control. This is not stored in the file (which contains
vertex skin ids only), rather generated dynamically for the in-memory stucture. The `weight` array has two fields, the `vertexid`
and the `weight`.

`m3dw_t` / `m3d_weight_t`:

| Type      | Name      | Desciprition                               |
|-----------|-----------|--------------------------------------------|
| uint32_t  | vertexid  | vertex index `myModel->vertex`             |
| float     | weight    | influence weight for this vertex           |

This array is sometimes called vertex group in certain applications. For an example, see section Skin below.

##### Dealing with quaternions

If the model has bones and skin, then the vertex list also contains orientation quaternions. For those, the `skinid` is always -2.
If by any chance the w component of the quaternion is missing (it is 1.0) in the vertex array, then you can recalculate it using
this formula:
```c
/* C */
m3dv_t *vertex = myModel->vertex[quatid];
float w = 1.0 - vertex->x * vertex->x - vertex->y * vertex->y - vertex->z * vertex->z;
vertex->w = w < 0.0 ? 0.0 : -sqrtf(w);
```
To convert an orientation quaternion into a transformation matrix to rotate points, you can do:
```c
/* C */
m3dv_t *vertex = myModel->vertex[quatid];
float xx = vertex->x * vertex->x, xy = vertex->x * vertex->y, xz = vertex->x * vertex->z, xw = vertex->x * vertex->w;
float yy = vertex->y * vertex->y, yz = vertex->y * vertex->z, yw = vertex->y * vertex->w;
float zz = vertex->z * vertex->z, zw = vertex->z * vertex->w;

float rotationMatrix[4 * 4] = {
    1.0-2*(yy+zz), 2*(xy+zw),     2*(xz-yw),     0,
    2*(xy-zw),     1.0-2*(xx+zz), 2*(yz+xw),     0,
    2*(xz+yw),     2*(yz-xw),     1.0-2*(xx+yy), 0,
    0,             0,             0,             1
};
```
For C++ you probably use a math library to do this for you, like GLM.

#### Skin

For the skin, let's go back to the vertex array for a bit. Each vertex has `skinid` which indexes the `myModel->skin` array
with pairs of bone id and weight. The index -1 means that the vertex is not related to any bones (and index of -2 means the
record stores a quaternion). The weights must sum up to 1.0, and it tells how much a particular bone influences that vertex.
If there's only one bone id, then weight must be 1.0 and means that that bone controls the vertex exclusively. A pair is only
valid if weight is greater than zero. There can be up to `M3D_NUMBONE` pairs, which is by default 4, but can be defined as 8.

`m3ds_t` / `m3d_skin_t`:

| Type                   | Name   | Desciprition          |
|------------------------|--------|-----------------------|
| uint32_t\[M3D_NUMBONE] | boneid | bone indices          |
| float\[M3D_NUMBONE]    | weight | weights for the bones |

Getting which skin group a vertex belongs to:
```c
/* C */
for(i = 0; i < myModel->numvertex; i++) {
    if(myModel->vertex[i].skinid != -1U && myModel->vertex[i].skinid != -2U) {
        printf(" vertexid %d skinid %d\n", i, myModel->vertex[i].skinid);
    }
}

/* C++ */
std::vector<m3ds_t> myModel->getSkin();
```
In context:
```c
/* C */
for(i = 0; i < myModel->numvertex; i++) {
    /* vertex - bones relation */
    if(myModel->vertex[i].skinid != -1U && myModel->vertex[i].skinid != -2U) {
        printf(" vertexid %d ", i);
        skinid = myModel->vertex[i].skinid;
        for(j = 0; j < M3D_NUMBONE && myModel->skin[skinid].weight[j] > 0.0; j++) {
            printf(" boneid %d (weight %g)",
                myModel->skin[skinid].boneid[j],
                myModel->skin[skinid].weight[j]);
        }
        printf("\n");
    }
}

for(i = 0; i < myModel->numbone; i++) {
    /* bone - vertices relation */
    printf(" boneid %d ", i);
    for(j = 0; j < myModel->bone[i].numweight; j++) {
        printf(" vertexid %d (weight %g)",
            myModel->bone[i].weight[j].vertexid,
            myModel->bone[i].weight[j].weight);
    }
    printf("\n");
}

/* C++ */
// NOTE: no special getter, you just simply read an array
std::vector<m3ds_t> myModel->getSkin();
std::vector<m3dv_t> myModel->getVertices();
std::vector<m3db_t> myModel->getBones();
```

### Getting the Actions (Animations)

(Before we get deeper into how the action frames are retrieved, you should know that there's an API to extract the model in
animation-pose, see Animated Models below.)

Again, nothing interesting here, the same usual array with a C++ getter for the names. But because every action contains a list of
frames which in turn contains list of transforms, and because integrating an animation into an engine is important, there are
extra getters for the entire skeleton for conveniece. Each action is a short animation with frames, like "walking", "attacking",
etc. The array is as follows:

`m3da_t` / `m3d_action_t`:

| Type      | Name         | Desciprition                     |
|-----------|--------------|----------------------------------|
| char*     | name         | name of the action               |
| uint32_t  | durationmsec | animation time in millisec       |
| uint32_t  | numframe     | number of frames                 |
| m3dfr_t*  | frame        | frames array                     |

```c
/* C */
for(i = 0; i < myModel->numaction; i++) {
    printf("actionid %d. name %s, duration %g sec, %d frames, frames array %p\n",
        i,
        myModel->action[i].name,
        ((float)myModel->action[i].durationmsec) / 1000,
        myModel->action[i].numframe,
        myModel->action[i].frame);
}

/* C++ */
std::vector<m3da_t> actions = myModel->getActions();
std::string first_actions_name = myModel->getActionName(0);
uint32_t first_actions_duration = myModel->getActionDuration(0);
std::vector<m3dfr_t> first_actions_frames = myModel->getActionFrames(0);
```
So far so good. Things are getting a bit complicated from here, because each frame contains another array, the list of required
transformations on the bones for that frame. As usual, there's a C++ getter here, which now requires animation index and also a
frame index.

`m3dfr_t` / `m3d_frame_t`:

| Type      | Name          | Desciprition                     |
|-----------|---------------|----------------------------------|
| uint32_t  | msec          | frame's position on the timeline |
| uint32_t  | numtransform  | number of transformations        |
| m3dtr_t*  | transform     | transformations array            |

`m3dtr_t` / `m3d_tranform_t`:

| Type      | Name   | Desciprition                             |
|-----------|--------|------------------------------------------|
| uint32_t  | boneid | changed bone index `myModel->bone`       |
| uint32_t  | pos    | new position index `myModel->vertex`     |
| uint32_t  | ori    | new orientation `myModel->vertex`        |

```c
/* C */
m3da_t *action = myModel->action[actionid];

for(i = 0; i < action->numframe; i++) {
    printf("Frame %d at %g sec\n", i, ((float)action->frame[i].msec) / 1000);
    for(j = 0; j < action->frame[i].numtransform; j++) {
        printf("  boneid %d. new position %d, new orientation %d\n",
            action->frame[i].transform[j].boneid,
            action->frame[i].transform[j].pos,
            action->frame[i].transform[j].ori);
    }
}

/* C++ */
uint32_t first_actions_first_frames_timestamp = myModel->getActionFrameTimestamp(0, 0);
std::vector<m3dtr_t> first_actions_first_frames_transforms = myModel->getActionFrameTransforms(0, 0);
```
Why is it called transformations when it's a simple bone-pose definiton list? That's because to calculate a particular frame, a
lot of transformations going to be needed. See Animated Models below.

### Getting Engine-Specific Extra Data

You can save extra, application-specific information into an M3D file in a standardized way (see Saving Models below). To get
those back, you have

`m3dchunk_t`:

| Type      | Name   | Desciprition                              |
|-----------|--------|-------------------------------------------|
| char\[4]  | magic  | magic, should start with lowercase letter |
| uint32_t  | length | length of the data including this header  |
| ...       | ...    | everything after the header is data       |

```c
/* C */
for(i = 0; i < myModel->numextra; i++) {
    printf("%d. magic %c%c%c%c, length %d, data %p\n",
        i, myModel->extra[i]->magic[0],myModel->extra[i]->magic[1],myModel->extra[i]->magic[2],myModel->extra[i]->magic[3],
        myModel->extra[i]->length, (unsigned char*)myModel->extra[i] + sizeof(m3dchunk_t));
}

/* C++ */
std::vector<std::unique_ptr<m3dchunk_t>> extra_chunk_ptrs = myModel->getExtras();
```

Animated Models
---------------

### Get the Bone Hierarchy for the Frame's Pose

Now for a particular frame, first create a copy of the bone list which will be the working copy skeleton. Fill this up
with the __bind-pose skeleton__ positions and orientation found in the `myModel->bone` list. If the first frame is at 0 msec,
then iterate through the transformations. Replace the bone's position and orientation with the new values, and you'll get the
__animation-pose skeleton__ for the first frame. If the first frame is not at 0 msec, then simply use this bind-pose skeleton
as the first animation-pose skeleton. After this, each frame describe what's changed compared to the previous skeleton. So for
the rest frames, iterate through the frame's transformation list, replacing new positions and orientations in this working copy,
and you'll get the __animation-pose skeleton__ for that frame. Note that except for the root bone, positions and orientations
are always parent-relative (so if upper arm moved, no need to change lower arm's position or orientation).

Luckily for you, the SDK provides a single call to do all of this. It receives three things: `actionid`, which selects the
animation, `frameid` which selects the frame, and a working copy of a skeleton. When you call it for the first time, you pass 0
as `frameid` and NULL as `skeleton`. It will return a bone list for the __animation-pose skeleton__. For subsequent calls, you pass
`frameid + 1`, and the skeleton returned by the previous call. This skeleton is an array of `m3dtr_t` structs, and has exactly as
many elements as `myModel->numbone`.

`m3dtr_t` / `m3d_transform_t`:

| Type      | Name   | Desciprition                             |
|-----------|--------|------------------------------------------|
| uint32_t  | boneid | element's index in this working copy     |
| uint32_t  | pos    | position index `myModel->vertex`         |
| uint32_t  | ori    | orientation `myModel->vertex`            |

If you call this function with `actionid` being -1 (or 0xffffffff), then it returns the bind-pose skeleton. For any other actionid
values the `frameid` contains the required frame's index, and `skeleton` must be the working copy skeleton returned by the previous
call with the previous frameid. Using non-sequential frameids will result in unexpected behaviour. Calling with any arbitrary
frameid and NULL as skeleton will return a newly allocated working copy of the skeleton at that frameid for that action.

```c
/* C */
m3dtr_t *skeleton;
skeleton = m3d_frame(myModel, 0, 0, NULL);
skeleton = m3d_frame(myModel, 0, 1, skeleton);
skeleton = m3d_frame(myModel, 0, 2, skeleton);
...
free(skeleton);

/* get a working copy of the animation-pose skeleton for the 2nd action's 5th frame */
skeleton = m3d_frame(myModel, 1, 4, NULL);

/* C++ */
std::vector<m3dtr_t> skeleton;
skeleton = myModel->getActionFrame(actionid, 0, nullptr);
skeleton = myModel->getActionFrame(actionid, 1, skeleton);
skeleton = myModel->getActionFrame(actionid, 2, skeleton);
...
delete skeleton;
```
Just for the records, to get the __bind-pose skeleton__ unmodified by any action frames, pass -1 as `actionid` (but you can also
copy out `pos` and `ori` fields from `myModel->bone`).

NOTE the difference between `myModel->getActionFrameTransforms()` and `myModel->getActionFrame()`. They both return
`std::vector<m3dtr_t>`, however the former returns a list of bones that were changed on the frame, while the latter returns the
entire skeleton updated for that frame.

### Interpolate the Bone Hierarchy into a Pose

Because not all frames are stored, you have to calculate 3 skeletons for displaying the animation at a given time (unless you have
a time which has a frame with exactly that timestamp). First you get two __animation-pose skeletons__ which are stored, one that's
past the requested time, and one that's in the future. Finally, you generate a third skeleton by interpolating between these two to
get the __interpolated animation-pose skeleton__ at a specific time.
```c
/* for each bone, assuming P is in the past pose, and F is in the future pose */
void interpolate_pos(m3dv_t *P, m3dv_t *F, float interp, m3dv_t *ret) {
    ret->x = P->x + interp * (F->x - P->x);
    ret->y = P->y + interp * (F->y - P->y);
    ret->z = P->z + interp * (F->z - P->z);
}
```
For the position, that's a simple linear interpolation.
```c
/* for each bone, assuming P is in the past pose, and F is in the future pose */
void interpolate_ori(m3dv_t *P, m3dv_t *F, float interp, m3dv_t *ret) {
    float a, b, c, d = P->w * F->w + P->x * F->x + P->y * F->y + P->z * F->z, e = fabsf(d);
    if(e > 0.99999) {
        a = 1.0 - interp; b = interp;
    } else {
        c = acosf(e); b = 1 / sinf(c); a = sinf((1.0 - interp) * c) * b; b *= sinf(interp * c);
        if(d < 0) b = -b;
    }
    ret->w = P->w * a + F->w * b;
    ret->x = P->x * a + F->x * b;
    ret->y = P->y * a + F->y * b;
    ret->z = P->z * a + F->z * b;
}
```
Interpolating an orientation is a little bit more complex, as linear would not cut it, you have to think on a sphere's surface.
This algorithm is called SLERP (spherical linear interpolation), which is slow, but works (note this is a highly optimized
version that does not use square root).

Again, the SDK is here to help you with a single API call. This receives `actionid` to select the animation, and a millisec
timestamp. (Note that this function's main goal is simple usage, and not to be optimized for performance.)

```c
/* C */
pose = m3d_pose(myModel, 0, 1000);
free(pose);

/* C++ */
pose = myModel->getActionPose(0, 1000);
delete pose;
```
Both will return an array which looks exactly like `myModel->bone`, has `m3db_t` records, also has number of elements
`myModel->numbone`, but instead of the rest-pose, the returned array contains an __interpolated animation-pose__. Also the
transformation matrix is updated for this skeleton pose, but this time it takes from bone-space to animated model-space which is
appropriate for skinning.

`m3db_t` / `m3d_bone_t`:

| Type       | Name      | Desciprition                               |
|------------|-----------|--------------------------------------------|
| char*      | name      | name of the bone                           |
| uint32_t   | parent    | parent bone index `myModel->bone`          |
| uint32_t   | pos       | position index `myModel->vertex`           |
| uint32_t   | ori       | orientation `myModel->vertex`              |
| uint32_t   | numweight | number of vertices controlled by this bone |
| m3dw_t*    | weight    | weights for those vertices                 |
| float\[16] | mat4      | bone -> model space transformation matrix  |

When you are done with the pose skeleton, simply free the array. Do not free the name or the weights in the records, as they
are still used by the bind-pose skeleton.

__Do not use this function in production. It is not efficient__, its purpose is to provide a simple way to debug the model
animation, and maybe used in simple model viewers. A game engine which operates on several synced animated models at once
should use `m3d_frame()` and calculate the interpolation yourself in a more efficient way.

#### Skinning

Okay one final step remained, we have to recalculate the vertices for the interpolated skeleton. First, before you do any
animations, you calculate transformation matrices for each bone using the __bind-pose skeleton__, by converting the position
and orientation into a 4x4 transformation matrix, multiplying along the bone's hierarchy. Remember, the root bone is in
model-space, but the rest are relative to their parent bone. For each bone, store the *inverse* of those matrices. Keep this list
of inverse matrices cached for the entire animation, you'll need them for every single frame.

Then for each frame, calculate the same transformation matrices for the __interpolated animation-pose skeleton__ (not the inverses
this time). Once you got that, then for each skin multiply with all of it's bones' weight, then add them together. You'll get a
frame transformation matrix for that skin group in that particular frame's pose. (You can cache this matrix for the frame as it
will be the same for all vertices in the same skin group.)

Now for each vertex, multiply with the corresponding skin group's __inverse bind-pose matrix__. This will take them from their
normal bind-pose to pose neutral bone-local space. Then multiply them with the __animated pose's skin group matrix__, which will
take them back into model-space again, but this time into a position modified by the pose. (Expand the vertex position with 1.0
to make it matrix41 / vec4, and multiply this to get the new position of the vertex. Same way expand the normal vertex with 0.0
and multiply that too to get the normal for that vertex in that pose.)

If you have used `m3d_pose()` or `myModel->getActionPose()`, then you have an easy job, because almost everything is calculated
for you. You just have to get a weighted sum for each vertex.

```c
/* C */
float tmp[4];
m3dv_t out_vertex;
out_vertex.x = out_vertex.y = out_vertex.z = 0.0;

animatedpose = m3d_pose(myModel, 0, 9999);

/* if the vertex is part of a skin group */
if(myModel->vertex[vertexId].skinid != -1U && myModel->vertex[vertexId].skinid != -2U) {
    skinid = myModel->vertex[vertexId].skinid;
    /* iterate on bone id + weight pairs */
    for(j = 0; j < M3D_NUMBONE && myModel->skin[skinid].weight[j] > 0.0; j++) {
        /* copy to a temporary vector, we don't want to ruin myModel->vertex[] */
        tmp[0] = myModel->vertex[vertexId].x; tmp[1] = myModel->vertex[vertexId].y;
        tmp[2] = myModel->vertex[vertexId].z; tmp[3] = 1.0;
        /* convert from bind-pose model space into bone-local */
        multiply_vec4_by_mat4(&tmp, &myModel->bone[ myModel->skin[skinid].boneid[j] ].mat4);
        /* convert from bone-local to animation-pose model space */
        multiply_vec4_by_mat4(&tmp, &animatedpose[ myModel->skin[skinid].boneid[j] ].mat4);
        /* adjust with weight and accumulate */
        out_vertex.x += tmp[0] * myModel->skin[skinid].weight[j];
        out_vertex.y += tmp[1] * myModel->skin[skinid].weight[j];
        out_vertex.z += tmp[2] * myModel->skin[skinid].weight[j];
    }
    printf("vertexid %d position for pose %g, %g, %g\n", vertexId, out_vertex.x, out_vertex.y, out_vertex.z);
}
free(animatedpose);

/* C++ */
/* again, there's nothing C++ specific here. What's more, you can use several libraries for multiplying a vec4
with a mat4, for example GLM, so there's no point in bloating the M3D SDK. */
skin_list = myModel->getSkin();
vertex_list = myModel->getVertices();
skeleton = myModel->getActionPose(0, 9999);
```

The biggest advantage of skin groups is that they are small (4 or 8 pairs), they use pre-calculated matrices only, so you
can easily pass them to a vertex shader, and let the GPU do the math for you.

Saving Models
-------------

Similarly to the importer, the exporter has only one function, which does not care about file systems and operates in memory only.
It compresses the in-memory structure into on disk format and returns that in a newly allocated buffer.
```c
/* C */
unsigned int size;
unsigned char *ucharbuffer = m3d_save(myModel, M3D_EXP_FLOAT, M3D_EXP_INLINE | M3D_EXP_EXTRA, &size);

/* C++ */
std::vector<unsigned char> myModel->Save(M3D_EXP_FLOAT, M3D_EXP_INLINE | M3D_EXP_EXTRA);
```
Now you can save the returned buffer into a file if you wish (or you can stream it over network etc.).

Keep in mind that if you don't use double precision with the in-memory format with `M3D_DOUBLE`, then there's not much point
in exporting into `M3D_EXP_DOUBLE` (however it will work and it will produce a valid output). If you set the `M3D_EXP_ASCII`
flag, then the `quality` parameter doesn't matter because ASCII stores the coordinates in strings. Also the `M3D_EXP_INLINE`
and `M3D_EXP_NOZLIB` flags make only sense with the binary format and do nothing with ASCII output.

If you want to inline textures or scripts, then you'll have to fill up the `m3d_t.inlined` array with file contents, and pass
`M3D_EXP_INLINE` in flags.

To store engine specific data into the output, then you'll have to fill up the `m3d_t.extra` array with chunk pointers and
pass `M3D_EXP_EXTRA` in flags.

Usually this function is "It Just Works" (TM). However you may encounter strange models, in which case there are export flags
to corrigate those model's coordinate systems before saving into a file.

Texture coordinates are stored that +U is on the right, and -V is up. If you have UV coordinates that are in model's coordinate
system (+V is up), then you can pass the `M3D_EXP_FLIPTXTCRD` flag to corrigate.

The coordinates in the model must be normalized to -1.0 .. 1.0. If you pass model->scale == 0.0 on input, then it is assumed that
model coordinates are in world-space, and the scale will be automatically calculated. If it's non-zero, then coordinates will be
normalized regardless, and scale will be saved as-is. This is a simple way to resize model easily. You can avoid this normalization
if, and only if you pass already normalized coordinates with non-zero scale value in the model, and `M3D_EXP_NORECALC` in the flags.

The coordinate system saved to the Model 3D file MUST be right-handed. Normally this is not an issue, but if you happen to have a
left-handed model in memory, then you can pass the `M3D_EXP_IDOSUCK` flag to indicate that, and the exporter will rearrange
the triangle points from CW to CCW order. You can also use the `m3dconv -r` tool to convert model files into right-handed
coordinate system.

You must always pass coordinates so that the model's orientation is +Y upwards, +X right and +Z front (as it's normal in a
Cartesian or Descartes coordinate systems). The exporter cannot change the orientation, however the `m3dconv -xyz` tool can, by
rotating the model around one of more axii one or more times.

### Model Quality

Model 3D files can store models with different quality. This is specified in the `quality` argument. Each level doubles the
required number of bytes to store the model.

| Number | Quality            | Description                                  |
|-------:|--------------------|----------------------------------------------|
|      0 | M3D_EXP_INT8       | export with -128 to 127 coordinate precision |
|      1 | M3D_EXP_INT16      | export with -32768 to 32767 precision        |
|      2 | M3D_EXP_FLOAT      | export with 32 bit floating point precision  |
|      3 | M3D_EXP_DOUBLE     | export with 64 bit floatint point precision  |

Most game models, which are already converted to low polygon count can be saved with quality 0 without loosing any detail. If
your model turns out to be "squarey", save with quality 1 as that would suffice in almost every cases. If you want to store
a model for engineering purposes, then you can use quality 2 which would store the coordinates in floating point numbers. For
some extreme cases, you can also use the highest quality 3, but for that you have to include the M3D SDK with the `M3D_DOUBLE`
define (so that the in-memory model would use double precision in the first place).

### Saving Raw Binary for Game Engines

If you don't care about model size, faster loading time is more important to you, then you can pass `M3D_EXP_NOZLIB` in flags.
Then the resulting buffer will contain an uncompressed M3D model and thus you'll save the decompression time on load.

### Saving into Distribution Format

For that, you should fill in `myModel->name`, `myModel->license`, `myModel->author` fields, specify the model's size, add all
textures to the `myModel->inlined` array, and save with `M3D_EXP_INLINED` flag. Always use zlib compression for distributed models,
and double check if the model's orientation is correct (+Y up, +X right, +Z front and the model is facing you).
```c
/* C */
myModel->name = "A Simple Model";                       /* use UTF-8 encoding only */
myModel->license = "GPL";
mymodel->author = "me <me@somemail.com>";               /* use an email or repo url */
myModel->scale = 0.75; /* in SI meters */

myModel->numinlined = 2;
myModel->inlined = (m3di_t*) malloc( 2 * sizeof(m3di_t) );

myModel->inlined[0].name = "hair";                      /* don't use ".png" extension */
myModel->inlined[0].length = hair_png_filesize;
myModel->inlined[0].data = &hair_png_filecontent;

myModel->inlined[1].name = "coat";
myModel->inlined[1].length = coat_png_filesize;
myModel->inlined[1].data = &coat_png_filecontent;
```
Only PNG textures are allowed, they should use palettes, and should be minimized (for example with
[pngquant2](https://github.com/kornelski/pngquant) or the online tinypng.com service). If the m3dconv utility was compiled
with EXPINLINE option, then it will take care of the texture conversion for you.

### Adding Scripts

Scripts has to be added into the `myModel->inlined` array. If those are also listed in the `myModel->texture` array, then they
will be interpreted as procedural texture scripts, and only saved if `M3D_EXP_INLINE` flag is also set. Other files which are
not PNGs and not in the texture list, are considered to be procedural surface scripts. Those will be saved regardless to the
`M3D_EXP_INLINE` flag, and also a corresponding "PROC" chunk will be generated for them. Unlike PNG textures, the inlined
scripts must have a proper file extension in their names.
```c
/* C */
myModel->numinlined = 1;
myModel->inlined = (m3di_t*) malloc( 1 * sizeof(m3di_t) );

myModel->inlined[0].name = "shpere.lua";                /* always use proper extension for scripts */
myModel->inlined[0].length = strlen(lua_script) + 1;
myModel->inlined[0].data = lua_script;
```

### Adding Your Own Engine-Specific Extra Chunks

This is as easy as adding buffers to the `myModel->extra` array. Those buffers must start with a proper chunk header, that is
4 bytes magic identifier (should be lowercase) and 4 bytes little-endian chunk size. The rest of the buffer is up to you.

Once you have that array, you can pass `M3D_EXP_EXTRA` flag, and those chunks will be saved into the resulting model file.
```c
/* C */
((m3dchunk_t*)somebuffer)->magic[0] = 'b';              /* use lowercase to avoid interference */
((m3dchunk_t*)somebuffer)->magic[1] = 'l';
((m3dchunk_t*)somebuffer)->magic[2] = 'n';
((m3dchunk_t*)somebuffer)->magic[3] = 'd';
((m3dchunk_t*)somebuffer)->length = somebuffer_size;    /* length must include the 8 bytes header */

myModel->numextra = 1;
myModel->extra = (m3dchunk_t**) malloc( 1 * sizeof(m3dchunk_t*) );

MyModel->extra[0] = (m3dchunk_t*)somebuffer;
```
The `myModel->extra` array is copied into the output as-is, there's absolutely no check on the magic. Therefore you can pass
handcrafted standard M3D chunks too with this option, so be aware not to start your own chunks with an upper-case letter.

### Saving into ASCII

If you have included the M3D SDK with the `M3D_ASCII` define, then you can pass `M3D_EXP_ASCII` in the flags. As a result,
the exporter will return a zero terminated, UTF-8 string instead of a binary bit-chunk. For ASCII files, the quality is
not important, because it stores coordinates as strings. Also `M3D_EXP_NOZLIB` and `M3D_EXP_INLINE` has no effect, because
a zero terminated string can't have binary blobs.

