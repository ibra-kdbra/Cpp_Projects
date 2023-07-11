Model 3D Bare Minimum Voxel Format
==================================

This file describes the bare minimum to store voxel images in [Model 3D](https://gitlab.com/bztsrc/model3d/blob/master/docs/m3d_format.md)
files. This format is very complex, so I thought it could be helpful to have a simpler description limited to voxels only.

All numbers are in little endian format.

[[_TOC_]]

## File Header Chunk

File starts with an 8 bytes long file header.

| Offset | Length | Description                             |
|-------:|-------:|-----------------------------------------|
|      0 |      4 | magic, '3DMO'                           |
|      4 |      4 | size of file                            |

This file header is followed by zlib compressed data ([RFC 1950](http://tools.ietf.org/html/rfc1950) deflate, could be compressed
by `stbi_zlib_compress(buffer, len, &outlen, 9)` and uncompressed by
`stbi_zlib_decode_malloc_guesssize_headerflag((const char*)data+8, *((uint32_t*)(data+4))-8, 4096, &outlen, 1)` calls).
After uncompression the buffer starts with the model header, and the buffer's last 4 bytes must be the end chunk.

## Model Header Chunk

| Offset | Length | Description                             |
|-------:|-------:|-----------------------------------------|
|      0 |      4 | magic, 'HEAD'                           |
|      4 |      4 | size of the header chunk, must be 20    |
|      8 |      4 | float, set to 0x3F800000                |
|     12 |      4 | flags, set to 0x014FCF80                |
|     16 |      4 | must be zero                            |

Header is followed by the voxel types chunk.

## Voxel Types

| Type     | Description                                         |
|----------|-----------------------------------------------------|
| uint32_t | magic 'VOXT'                                        |
| uint32_t | length                                              |
| x        | fixed length records, voxel type definitions        |

For each voxel type, 8 bytes stored:

| Type     | Description                                                           |
|----------|-----------------------------------------------------------------------|
| uint32_t | diffuse color of the voxel, RGBA (red is the least significant byte)  |
| uint32_t | must be zero                                                          |

Voxel type definition chunk is followed by one or more voxel data chunks.

## Voxel Data

This is a multiple chunk. Voxels are stored in blocks (layers), and more blocks may exists and their coordinates may overlap.

| Type     | Description                                         |
|----------|-----------------------------------------------------|
| uint32_t | magic 'VOXD'                                        |
| uint32_t | length                                              |
| uint8_t  | must be zero                                        |
| int16_t  | pX position of the block                            |
| int16_t  | pY position of the block                            |
| int16_t  | pZ position of the block                            |
| uint16_t | sX size of the block                                |
| uint16_t | sY size of the block                                |
| uint16_t | sZ size of the block                                |
| uint16_t | must be zero                                        |
| x        | variable length records, voxel data                 |

After the chunk header, comes the voxel data, describing sY \* sZ \* sX voxels. This is an RLE compressed data.

| Type             | Description                        |
|------------------|------------------------------------|
| uint8_t          | magic, uncompressed if bit 7 unset |
| (N+1) * uint16_t | N+1 times voxel pixel              |

| Type             | Description                        |
|------------------|------------------------------------|
| uint8_t          | magic, compressed if bit 7 set     |
| uint16_t         | one voxel pixel                    |

The repeat number (N) is (record magic & 0x7F). If record magic bit 7 is not set, then N+1 uint16_t follows. If bit 7 is set,
then only one uint16_t follows, which must be repeated N+1 times.

Voxels are stored in Y, Z, X order, in a right-handed coordinate system, meaning Y means the horizontal layer from bottom to
top, Z is the depth and X is the width. The lowest level (which is probably the ground level) is stored first, then the level
above that. Within a level, the row farest away stored first, and within a row the voxel on the left. +Y is up, +Z is towards the
viewer, and +X is on the right. The voxel not set values are encoded as -1 (0xFFFF), and clear to empty as -2 (0xFFFE). Empty
(unset) voxels should be skipped, and original voxel in the world kept. With clear, then the voxel in the world must be cleared
to empty. Non-empty voxels in the chunks always override world voxels upon load.

## End Chunk

This is a unique chunk, and unconventional as it has no length field.

| Type      | Description               |
|-----------|---------------------------|
| uint32_t  | magic 'OMD3'              |

Using the M3D SDK with Voxel Images
-----------------------------------

This is a very basic and minimal description of the format. One may want to use the [M3D SDK](https://gitlab.com/bztsrc/model3d/blob/master/m3d.h),
which is a single, stb-style ANSI C/C++ header file. This has an advantage that as the SDK handles all features so that you can
load any M3D file with it, not just the simplified one described above (with all those "must be zero" and mistic constant flag
fields). The SDK is totally dependency-free. In order to avoid compression code duplication, one could simply include stb_image
and stb_image_write *before* the m3d.h include, and the M3D SDK will use those instead of the built-in ones. The SDK operates
on memory buffers only.

## Loading

Deserializing a model file from a memory buffer goes like
```c
#define M3D_IMPLEMENTATION
#define M3D_NOVOXELS        // this is needed so that voxel models don't get automatically converted to triangle mesh
#include <m3d.h>

m3d_t *model = m3d_load(&filecontentsbuffer, NULL, NULL, NULL);
```

After that you can access the uncompressed data in the following arrays:

| Field      | Description                            |
|------------|----------------------------------------|
| numvoxtype | number of voxel types                  |
| voxtype    | voxel types, only color is interesting |
| numvoxel   | number of voxel layers                 |
| voxel      | uncompressed voxel layers data         |

Iterating on voxel types looks like
```c
for(i = 0; i < model->numvoxtype; i++)
    printf("%d. %08x\n", i, model->voxtype[i].color);
```
and the uncompressed voxel data (with indeces to voxtype) as
```c
for(i = 0; i < model->numvoxel; i++) {
    printf("layer #%d. name '%s' x=%d y=%d z=%d width=%d height=%d depth=%d\n  data:",
        i + 1, model->voxel[i].name,
        model->voxel[i].x, model->voxel[i].y, model->voxel[i].z,
        model->voxel[i].w, model->voxel[i].h, model->voxel[i].d);

    for(j = 0; j < model->voxel[i].w * model->voxel[i].h * model->voxel[i].d; j++)
        printf(" %d", model->voxel[i].data[j]);

    printf("\n\n");
}
```

## Saving

First, create an empty in-memory model. It is important to use calloc, or one can use malloc + memset zero combo.
```c
#define M3D_IMPLEMENTATION
#define M3D_EXPORTER        // there'll be no m3d_save function without this define
#include <m3d.h>

m3d_t *model = calloc(1, sizeof(m3d_t));
```
then fill up voxel type array with different colors (the format is capable of lot more, like materials and skeleton-based voxel
animations, not covered here) like `m3d->voxtype[i].color = some_uint32_color_value;`. Also fill up the layers data to the
`model->voxel` array. This should be trivial, set fields x,y,z,w,h,d and data with voxel type indices for each layer (use -1 or
M3D_VOXUNDEF for voxels that are not set). If you want, you can point `m3d->voxtype[i].name` to a zero terminated UTF-8 string,
which will then be saved as the name of the layer (however string table is not covered in this simplified description). Finally,
serialize the in-memory model:
```c
unsigned char *filecontentsbuffer = m3d_save(model, 0, 0, &outlen);
```
This will return a newly allocated buffer with a bitchunk to be written to a file. The SDK probably serializes into a more
compact format than the one described above and might store extra info too (like voxel type names, layer names or copyright
string for example).

To free voxel types, layer data and layers, use
```c
m3d_free(model);
```

