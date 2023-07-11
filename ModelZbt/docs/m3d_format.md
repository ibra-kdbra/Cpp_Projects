Model 3D File Format Specification
==================================

- File extension: `.m3d`
- Mime type: `model/x-3d-model`

This file describes the binary format (which requires far less storage space) and uses chunks to be expandable.
This is the primary format for storing 3D models, and the [model loader](https://gitlab.com/bztsrc/model3d/blob/master/m3d.h)
uses this format. Chunks always start with a 4 bytes magic and a 4 bytes length. Integers are stored in little-endian format,
floating points as per the IEEE-754 specification. There's an [ASCII version](https://gitlab.com/bztsrc/model3d/blob/master/docs/a3d_format.md)
too which is less space efficient but editable with a standard text editor and its primary goal is to help debugging.

(Hint: you can use `m3dconv -d` or `m3dconv -dd` to dump the contents of a binary Model 3D file.)

__IMPORTANT NOTE__ the license is MIT. This does not mean you should change the basic structure of an M3D file in any way. The
intention with the permissive license is to allow embedding M3D into any project (Open Source, proprietary, hobby and commertial),
and to allow you to specify and add your own chunks; and *not* to mess up M3D compatibility between software. Keep this in mind!

[[_TOC_]]

## Quick Glossary

 - texture: a 2d image, inlined or stored separately to the model
 - texture coordinate: 2 numbers specifying a point on a 2d texture with weight (u,v)
 - material: a more accurate description of how a triangle should look like, includes textures, ambient, diffuse color etc.
 - vertex: 4 numbers specifying a point in 3d space with weight (x,y,z,w)
 - normalized vertex: coordinates scaled to the range -1.0 and 1.0 (do not confuse with normal vector)
 - bounding cube: a 2 x 2 x 2 box (-1.0 to 1.0) around the model. The model is scaled to fit and fills up the box entirely
 - face: connected vertices describing the surface of the model, most likely using concatenated triangles
 - triangle: 3 vertices specifying a plane in 3d space
 - colored triangle: gradients between the three vertices using colors of the vertices
 - textured triangle: material's texture image stretched on a triangle by specifying texture coordinates for each triangle points
 - normal vector: 2 vertices, one on the model's face, the other outside of the model, direction pointing outwards
 - right-hand rule: triangle points are listed in counter-clockwise order, rhr specifies the triangle's normal vector
 - skeleton: means a bone hierarchy, the most outter bone is the bounding cube
 - bone: a group of vertices which are transformed together. A triangle can have vertices belonging to different bone groups
 - bone position: given by the zerus point of its coordinate system
 - bone orientation: given by a vertex, which must be interpreted as a quaternion
 - animation frame: list of modified bone positions and orientations to be applied on a specific sub-tree of the bone hierarchy
 - action: named group of animation frames
 - voxel: a small cube aligned on a regular 3D grid, voxel images are made up of voxels
 - distribution format: binary blob (.m3d) or text file (.a3d), also called on disk format
 - in-memory format: easily traversable arrays of structures
 - GPU ready format: VBO and EBO compatible lists of the model

## Model Dimensions and Orientation

The vertices are normalized, meaning a tall model will have 1.0 and -1.0 Y coordinates included, while a wide model would
have -1.0 and 1.0 X coordinates defined. All models must fill up the entire space in the -1.0 and 1.0 range of the bounding cube,
and the models relative sizes are determined by the scale factor field in the header.

All models MUST be oriented in an uniform way, which is in a right-handed Cartesian or Descartes coordinate system: provided that
+Y is up, +X is on the right, and +Z points towards the viewer, then model's front must be facing towards the viewer. This
means if the model is a building for example, then the main entrance must be in front; if the model supposed to move (like an
animal, vehicle, aircraft etc.) then it's forward direction must point towards the user. This criteria is very important to
simplify engines and make the models instantly replaceable. Right-handed coordinate system also means that polygons (triangles)
are stored in CCW order.

It is recommended that models should be positioned as such that X, Z plane is the ground (Y=0), probably having only positive Y
coordinates. The model should be centered on the X axis (|min X| = |max X|), and should have only negative Z coordinates (Z=0 is
the front of the model). These are only recommendations, as it's easy to recalculate the bounding cube in an engine (in contrast
to orientation, that cannot be detected).

## Chunks

The file always starts with a pair of file header and model header chunks. Other chunks are optional, but in a specific order: if
a chunk defines something that another chunk references, then the definition chunk should came first. The usual ordering is as
follows: 3DMO, PRVW, HEAD, CMAP, TMAP, VRTS, BONE, MTRL*, PROC*/MESH*/SHPE*/VOXT/VOXD*, LBLS*, ACTN*, ASET*, OMD3.

All chunks with invalid magic (that is, starts with a non-uppercase Latin letter) are reserved for application-specific purposes.
For example one could store Blender specific data in 'blnd' chunk. Full uppecase magics mean standardized, application-independent
chunks, which must be interpreted by all applications the same way.

## File Header Chunk

File starts with an 8 bytes long file header. The binary format has the magic in uppercase, in contrast to the ASCII format which
uses lowercase for the file magic.

| Offset | Length | Description                             |
|-------:|-------:|-----------------------------------------|
|      0 |      4 | magic, '3DMO'                           |
|      4 |      4 | size of file                            |

## Preview

This is an optional chunk. If exists, it must be the first chunk, before the compressed payload, so that thumbnailers can
quickly access and load it.

| Type     | Description               |
|----------|---------------------------|
| uint32_t | magic 'PRVW'              |
| uint32_t | length                    |
| x        | PNG preview image         |

The preview should be a 256 x 256 wide tops, paletted PNG image. It's size should not exceed 64k. It can be used in
thumbnailers and in file browser windows of CAD software.

## Model Header Chunk

The file header (or the preview if exists) is followed by either a Model Header chunk (magic "HEAD") or zlib compressed data
([RFC 1950](http://tools.ietf.org/html/rfc1950) deflate). After uncompression (or at file offset + 8) the buffer starts with
the model header.

| Offset | Length | Description                             |
|-------:|-------:|-----------------------------------------|
|      0 |      4 | magic, 'HEAD'                           |
|      4 |      4 | size of the header chunk                |
|      8 |      4 | float, scaling factor, size in meters   |
|     12 |      4 | bitfield, variable types                |
|     16 |      x | string table                            |

### Scaling Factor

This is the half of the model's bounding cube's size in __SI meters__. Because the model is always scaled to coordinates range
-1.0 and 1.0, this actually equals to 1.0 model coordinate units. This field is optional, can be zero, only makes sense to render
more models next to each other correctly to scale.

Note that scale factor corresponds to the bounding cube, and not to the model itself. For example, if there's a human model
with an animation in which he raises the hand above his head, then the bounding cube must be taller than the model. In this
case the scaling factor must be adjusted accordingly, adding the distance of the raised hand and head top in SI meters to it.

### Variable Types

This field encodes the different data types used in the model as follows:

| Bits   | Name   | Description                |
|--------|--------|----------------------------|
|  0 - 1 | vc_t   | vertex coordinate type     |
|  2 - 3 | vi_t   | vertex index type          |
|  4 - 5 | si_t   | string offset type         |
|  6 - 7 | ci_t   | color index type           |
|  8 - 9 | ti_t   | texture index type         |
| 10 -11 | bi_t   | bone index type            |
| 12 -13 | nb_t   | number of bones count      |
| 14 -15 | sk_t   | skin index type            |
| 16 -17 | fc_t   | frame bones count          |
| 18 -19 | hi_t   | shape index type           |
| 20 -21 | fi_t   | face index type            |
| 22 -23 | vd_t   | voxel dimension size       |
| 24 -24 | vp_t   | voxel pixel type           |

The type definition of the vertex coordinates (vc_t):

| BitVal | Type    | Description                             |
|--------|---------|-----------------------------------------|
| 00     | int8_t  | from -128 to 127                        |
| 01     | int16_t | from -32768 to 32767                    |
| 10     | float   | IEEE binary32 (float) from -1.0 to 1.0  |
| 11     | double  | IEEE binary64 (double) from -1.0 to 1.0 |

Note that signed integer coordinates are only for shrinking the file size considerably, they are loaded into
memory as floats just like the rest vertex coordinate types. There are lot of games (like Minecraft for one)
where a -128 to 127 grid for the models are more than sufficient, and those require only 1/4 of the size
of a float coordinate. Upon model import, -128 will be scaled to -1.0 and 127 to 1.0. I'd bet that most game
models as of 2019 would be more than satisfied with a 65536 x 65536 x 65536 grid, which cuts in half the
storage requirements of binary models.

For all the other index, size and count types the bits are:

| BitVal | Type     | Description     |
|--------|----------|-----------------|
| 00     | uint8_t  | from 0 to 255   |
| 01     | uint16_t | from 0 to 65535 |
| 10     | uint32_t | from 0 to 2^32  |
| 11     | -        | (not defined)   |

Here "not defined" does not mean that the value is not defined by the specification, but that the corresponding chunk
in the file does not exists so the index is not defined in the model.

The number of bones per vertex (nb_t) also encodes power of two values, and describes how many bone can influence
a skin point vertex in skeletal animation.

| BitVal | Description        |
|--------|--------------------|
| 00     | 1 bone per vertex  |
| 01     | 2 bones per vertex |
| 10     | 4 bones per vertex |
| 11     | 8 bones per vertex |

The voxel pixel (vp_t) type is either uint8_t or uint16_t which depends on the preceeding voxel type chunk, see the
section "Voxel Face". That describes one voxel index, while voxel dimension size (vd_t) describes the type of the dimensions
of the voxel image (either if it's maximum size is 255 x 255 x 255 or 65535 x 65535 x 65535 etc.). Voxel dimension type just
like vc_t is a signed value, because it's also used to indicate block positions.

### String Table

Strings are stored as zero terminated UTF-8 strings. The length of the string table equals to the size of the model header chunk
minus 16. The first 4 strings have special meaning.

| Pos. | Description                                                     |
|------|-----------------------------------------------------------------|
| 1st  | Name of the object, like "Utah teapot"                          |
| 2nd  | Usage condition or license, like "MIT", "LGPL" or "BSD-3clause" |
| 3rd  | Author: nickname, email, homepage or github URL etc.            |
| 4th  | Comments, meta-information. May contain '\\n' newline character |

The first three strings may contain ' ' space, but not control characters (ASCII < 32, line '\\n' newline or '\\r' carrige-return
or '\\t' tab for example).

The authors field should be encoded as a "To" field in SMTP protocol, that is, "name \<contact\>", however using "\<\>" is
optional, and "contact" can be a repository URL as well. When there are more authors, their names must be separated by a ", "
comma and space, like "Me Name \<me@github\>, John Doe \<https://gitlab.com/johndoe\>".

Lines in description are ended in a single '\\n' newline character; it is free-form, but empty lines are not allowed. If the first
line is a special schema magic, then other lines are structured in "(keyword)=(value)" pairs. See
[CAD models](https://gitlab.com/bztsrc/model3d/blob/master/docs/CAD.md). The magic "META" means keywords are not specified per se,
but the description is structured in key plus value pairs (pretty much like environment variables on UNIX). One keyword,
"GENERATOR=(unixname)-(version)" must be available in all schema.

The other strings are identifiers, should not contain ' ' space nor '/' and '\\' directory separators (those must be replaced by
an '\_' underscore), and for compatibility should only use latin letters and numbers (a-zA-Z0-9\_), although symbols and UTF-8 are
allowed (as other symbols and multibytes do not interfere with file specification for sure). Those are the names of the bones,
materials, actions etc. Generated names should begin with an identifier type byte, and followed by 3-7 hex numbers, like "b001"
(bone #1), "m0FF" (material #255), "a003" (action #3), "t080" (texture #128) or "s0005" (shape #5). There's no practical limit to
the strings' length (4G), but some engines may have problems with longer than 255 bytes identifiers.

Because the first string is not a valid identifier (but the name of the model), therefore string offset 0 encodes no string
identifier assigned.

## Color Map

If exists, the second chunk is a color map. Each color is stored on 4 bytes, RGBA (red is the least significant byte, alpha
is the most). This is a unique chunk, can occur only once in the model. It only exists if ci_t is uint8_t or uint16_t.

| Type     | Description               |
|----------|---------------------------|
| uint32_t | magic 'CMAP'              |
| uint32_t | length                    |
| x        | RGBA codes, each uint32_t |

Colors must be sorted by HSV value, meaning lower indices point to darker, higher indices point to brighter colors.

NOTE: How the colors are encoded depends on the bits in the header, but you don't have to worry about this. The M3D SDK always
returns RGBA pixels for you, you don't need to know which color encoding was actually used in the file, hence you'll never need
to parse this chunk and you'll never need to check for its existence.

## Texture Map

If exists, it defines unique UV coordinates for the textures. This is a unique chunk.

| Type     | Description                  |
|----------|------------------------------|
| uint32_t | magic 'TMAP'                 |
| uint32_t | length                       |
| x        | several fixed length records |

One coordinate pair is encoded as:

| Type     | Description  |
|----------|--------------|
| vc_t     | u            |
| vc_t     | v            |

The size of the record depends on the vertex coordinate type, but it's the same for each record.

Texture coordinates are stored in a way that +U is on the right, and -V is up. This is what most image formats do. It is the
opposite of +Y is up with the vertex coordinates. Therefore when one passes V to an OpenGL VBO, (1.0 - V) must be used (no
conversion required for DirectX).

## Vertex Chunk

This chunk stores the unique vertices (which could be polygon point, bone position, normal vector, etc., everything that has four
x, y, z, w components). This is also a unique chunk.

| Type     | Description                  |
|----------|------------------------------|
| uint32_t | magic 'VRTS'                 |
| uint32_t | length                       |
| x        | several fixed length records |

One vertex is encoded as:

| Type     | Description                                        |
|----------|----------------------------------------------------|
| vc_t     | x                                                  |
| vc_t     | y                                                  |
| vc_t     | z                                                  |
| vc_t     | w                                                  |
| ci_t     | ** color index (only if ci_t valid) **             |
| sk_t     | ** skin index (only if sk_t valid) **              |

The size of the record depends on the data type, but it's the same for each vertex. The last two fields only exists if their
type's corresponding flags in header are not 11 (meaning "not defined"). Quaternions are stored in this same list, however
always with skinid of -2 (-1 means no skin). Without bones and skin, this list only contains vertices, and no quaternions.
Normal vectors, just like polygon and bone positions are stored in this chunk too, because this list is deduplicated, meaning
the model can be much smaller. (Technically these are different kind of things, but since they all need 4 coordinates, it's
space efficient to store them together in a unique list.)

The x, y, z components are stored as +Y is up, +X is on the right and +Z is towards the viewer. The w component is usually
defaults to 1.0 if not defined otherwise.

## Skeleton

The skeleton chunk stores the bone structure hierarchy, and it's optional. This is unique chunk which describes the bind-pose of
the model's skeleton.

| Type     | Description                                  |
|----------|----------------------------------------------|
| uint32_t | magic 'BONE'                                 |
| uint32_t | length                                       |
| bi_t     | number of bones                              |
| sk_t     | number of skins                              |
| x        | bone hierarchy, several fixed length records |
| x        | skin, several variable length records        |

Each record in the bone hierarchy looks like:

| Type     | Description                              |
|----------|------------------------------------------|
| bi_t     | parent bone index                        |
| si_t     | offset to string table, name of the bone |
| vi_t     | bone position                            |
| vi_t     | bone orientation                         |

The name for the bone is optional, can be zero. If the bone is at the top level, then its parent is -1 (0xFF, 0xFFFF, 0xFFFFFFFF
depending on bi_t's type). The order is specified in that parent definitons must come first. The bone position designates the
zerus point in the bone's space, and orientation selects a vertex which actually represents an orientation quaternion in this
case. Bone positions without parent are in object space, while children bone positions are relative to their parents. They
describe a so called "bind-pose" skeleton.

After that comes the skin definition, which connects vertices to specific bones. The weight is a normalized float (meaning
255 = 1.0, 127 = 0.5 etc.), and all weights for a point must sum up to 255. Bone indexes for 0 weights are not stored. Depending
on the number of bones per vertex (nb_t) flag in the header, bone weight type is as follows:

| nb_t   | bone weight type (bw_t)                                           |
|--------|-------------------------------------------------------------------|
| 00     | bi_t (the weight is constant 1.0)                                 |
| 01     | uint8_t\[2] bi_t (bi_t)                                           |
| 10     | uint8_t\[4] bi_t (bi_t (bi_t (bi_t)))                             |
| 11     | uint8_t\[8] bi_t (bi_t (bi_t (bi_t (bi_t (bi_t (bi_t (bi_t))))))) |

First comes the weights, then bone ids for non-zero weights. With nb_t = 00 there's no weight stored (assumed constant 1.0) and
there's always a bone id.

For example if nb_t is 2, that means 4 weights are stored (4 bytes), and if 3 of those bytes are non-zero, then it's followed by
3 bi_ts (bone ids for the non-zero weights).

The skinid field of the vertex records and voxel types references these.

## Materials

This chunk specifies how the model's face looks like in a little bit more detail than a color. There can be more materials
defined in a single file, so this chunk may repeat several times and it's also optional.

Model 3D files are allowed to refer to undefined materials. In that case it is the loader application's responsibility to load the
material definitions from a separate Model 3D file which only has material definition chunks.

| Type     | Description                                      |
|----------|--------------------------------------------------|
| uint32_t | magic 'MTRL'                                     |
| uint32_t | length                                           |
| si_t     | offset to the string table, name of the material |
| x        | variable length records                          |

Each record specifies an additional attribute of the material. Within a material chunk, all records must be unique, having
different magic bytes.

| Type     | Description                 |
|----------|-----------------------------|
| uint8_t  | magic, property type id     |
| prop_t   | property specific parameter |

As a general rule, magic values 0 - 127 specify overall scalar properties, while their corresponding 128 - 255 magics represent
texture maps for the same property. There are a few exceptions, for example there's no illumination model map. Magic values 0 - 63
are for display properties (used by almost every engine), while magic values 64 - 127 describe physical properties of the material
(mostly useful to physic engines and raytracers).

| Magic     | Group                      |
|----------:|----------------------------|
|   0 - 63  | Scalar display properties  |
|  64 - 127 | Scalar physical properties |
| 128 - 191 | Display property maps      |
| 192 - 255 | Physical property maps     |

For convenience, the ASCII variant's keywords are shown in quotes next to the records. Texture maps has exactly the same
keywords, except they are prefixed by "map_". Where map keyword has an alias, it's mentioned. There are no texture options
(like mirroring), simply invert UV coodinates in vertices and scale your texture properly in advance. Material properties can
be configured for the API in [m3d.h](https://gitlab.com/bztsrc/model3d/blob/master/m3d.h) in `m3d_propertytypes[]` static array.
I strongly discourage to change the default property identifiers defined here, however you are free to add your own properties.

### Scalar Display Properties

| Magic    | Parameter | Keyword | Description                                                        |
|----------|-----------|---------|--------------------------------------------------------------------|
| 0        | ci_t      | "Kd"    | diffuse color index (if defined, overrides color index in vertex)  |
| 1        | ci_t      | "Ka"    | ambient color index                                                |
| 2        | ci_t      | "Ks"    | specular color index                                               |
| 3        | float     | "Ns"    | specular exponent                                                  |
| 4        | ci_t      | "Ke"    | emissive (emitting light of this color)                            |
| 5        | ci_t      | "Tf"    | transmission color index                                           |
| 6        | float     | "Km"    | bump strength                                                      |
| 7        | float     | "d"     | dissolve: 0 - fully transparent, 1.0 - fully opaque (do not use)   |
| 8        | uint8     | "il"    | illumination model                                                 |

(Dissolve is only for backward-compatibility. You should use the diffuse color's alpha-channel instead. Illumination model is
just informational, has the same values as a Wavefront OBJ. It is simply ignored by PBR-shaders.)

| il | Description                                            |
|----|--------------------------------------------------------|
|  0 | Color on and ambient off (no shading)                  |
|  1 | Color on and ambient on (Phong)                        |
|  2 | Hightlight on (Gouraud)                                |
|  3 | Reflection and ray trace                               |
|  4 | Transparency simple, reflection simple and ray trace   |
|  5 | Reflection fresnel and ray trace                       |
|  6 | Transparency refraction, reflection simple, ray trace  |
|  7 | Transparency refraction, reflection fresnel, ray trace |
|  8 | Reflection simple and no ray trace                     |
|  9 | Transparency simple, ray trace off                     |
| 10 | Casts shadows onto invisible surfaces                  |

### Scalar Physical Properties

| Magic    | Parameter | Keyword | Description                                                        |
|----------|-----------|---------|--------------------------------------------------------------------|
| 64       | float     | "Pr"    | roughness                                                          |
| 65       | float     | "Pm"    | metallic                                                           |
| 66       | float     | "Ps"    | sheen                                                              |
| 67       | float     | "Ni"    | index of refraction (optical density, Fresnel-effect, IOR)         |
| 68       | float     | "Nt"    | thickness of face (in millimeter, for printing)                    |

### Display Property Texture Maps

| Magic    | Parameter | Keyword  | Description                                                       |
|----------|-----------|----------|-------------------------------------------------------------------|
| 128      | si_t      | "map_Kd" | diffuce color texture map name or identifier (could be a filename)|
| 129      | si_t      | "map_Ka" | ambient occulsion color texture map name or identifier            |
| 130      | si_t      | "map_Ks" | specular texture map name or identifier                           |
| 131      | si_t      | "map_Ns" | specular exponent texture map name or identifier                  |
| 132      | si_t      | "map_Ke" | emissive map name or identifier                                   |
| 133      | si_t      | "map_Tf" | transmission color texture map name or identifier                 |
| 134      | si_t      | "map_Km" | "map_bump", bump map name or identifier                           |
| 135      | si_t      | "map_d"  | dissolve map name or identifier                                   |
| 136      | si_t      | "map_N"  | normal map name or identifier                                     |

### Physical Property Texture Maps

| Magic    | Parameter | Keyword  | Description                                                       |
|----------|-----------|----------|-------------------------------------------------------------------|
| 192      | si_t      | "map_Pr" | roughness map name or identifier (could be a filename)            |
| 193      | si_t      | "map_Pm" | "map_refl", metallic map name or identifier                       |
| 194      | si_t      | "map_Ps" | sheen map name or identifier                                      |
| 195      | si_t      | "map_Ni" | refraction map name or identifier                                 |
| 196      | si_t      | "map_Nt" | thickness map name or identifier                                  |

## Face

The face can be described in several ways. There are four kinds of chunks to do so, and a Model 3D file must have at least one
of those chunks to describe geometry. For all chunks the purpose is to connect the vertexes to create a surface for the model.
Usually there's only one face chunk, but if the model consist of more separated parts, could be more (however model importers
should handle those as if there were a single big concatenated chunk, unless they are providing a "Layers" option on their
user interfaces).

### Procedural Surface

| Type     | Description                                     |
|----------|-------------------------------------------------|
| uint32_t | magic 'PROC'                                    |
| uint32_t | length                                          |
| si_t     | offset to the string table, script identifier   |

The identifier must end in the corresponding script extension (.lua, .py etc.), and it is recommended to have the script inlined
in the model (see Inlined Assets below). For more details, see [procedural models](https://gitlab.com/bztsrc/model3d/blob/master/docs/procedural.md).

### Mesh

This chunk describes the face by connecting the vertices with polygons. Those are always defined in counter-clockwise
direction, specifing the normal vector according to the right-hand rule. This can be overriden by records containing
normal vector information, but it is strongly recommended to keep CCW even with normals. Polygon mesh is the default
visualization source, unless shapes are also defined (see below).

If the mesh can be described by a series of cubes, then a more compact form can be used using voxels (see Voxel Face below).
Upon load, voxels are converted into a mesh with triangles, so the application won't notice the difference.

| Type     | Description                                     |
|----------|-------------------------------------------------|
| uint32_t | magic 'MESH'                                    |
| uint32_t | length                                          |
| x        | variable length records                         |

#### Record Magic

Each record has vertex indices at minimum, but depending how they describe the polygon they may have more data. This is
encoded in the magic byte's least significant 4 bits. The number of vertices is encoded int the magic's most significant 4 bits.

| Magic bit | Description                                                   |
|-----------|---------------------------------------------------------------|
| 0         | texture coordinate index in the record                        |
| 1         | normal vector index in the record                             |
| 2         | maximum vertex index in the record                            |
| 3         | reserved for future use, must be zero                         |
| 4-7       | number of points int this poly (3 = triangle, 4 = quad, ...)  |

Please note that the file format is capable of storing points, lines, triangles, quads and other polygons up to 15 sides, however
the [M3D SDK](https://gitlab.com/bztsrc/model3d/blob/master/m3d.h) only supports the most common triangle meshes for now.

| Magic bit | Description                                                   |
|-----------|---------------------------------------------------------------|
| 0-3       | property (0 - material, 1 - parameter)                        |
| 4-7       | must be zero                                                  |

Records with zero vertices are special records, which set particular properties. Subsequent polygons are all expected to have the
same property that these records set. The property's value follows the magic byte, and its size depends on the property's type
(currently both material and parameter has a string index value).

#### Special Records

| Type    | Description                                      |
|---------|--------------------------------------------------|
| uint8_t | magic 0, use material                            |
| si_t    | offset to the string table, name of the material |

The magic 0 encodes the material. 0 offset means no material, revert to vertex colors. Model 3D files may refer to undefined
materials (material name without a corresponding MTRL chunk), in which case it is the loader's responsibility to load a material
library from another Model 3D file which only contains material definitions.

| Type    | Description                                       |
|---------|---------------------------------------------------|
| uint8_t | magic 1, use parameter                            |
| si_t    | offset to the string table, name of the parameter |

Normally polygon points are stored with one vertex. However with bit 2, maximum vertex can be stored too. If this is set, then
the normal vertex coordinate means minimum value, and the actual vertex coordinate is between the minimum and the maximum.
Where excatly depends on a parameter value, and this record selects that parameter.

#### Polygon Data Records

Records must be sorted by material, minimalizing material context switches. Moreover, they must be ordered by descending opacity,
so polygons that belong to no material comes first, then the polygons for the most opaque material, finally the polygons with the
most transparent material. This ordering is important so that the engines can use alpha blending, knowing that a simple Z-depth
check would suffice when drawing the mesh.

For convenience, here are the record formats written out for triangle meshes:

| Type    | Description                            |
|---------|----------------------------------------|
| uint8_t | magic 48, simple triangle              |
| vi_t    | vertex index                           |
| vi_t    | vertex index                           |
| vi_t    | vertex index                           |

| Type    | Description                            |
|---------|----------------------------------------|
| uint8_t | magic 49, triangle with UV coordinates |
| vi_t    | vertex index                           |
| ti_t    | texture coordinate index               |
| vi_t    | vertex index                           |
| ti_t    | texture coordinate index               |
| vi_t    | vertex index                           |
| ti_t    | texture coordinate index               |

| Type    | Description                            |
|---------|----------------------------------------|
| uint8_t | magic 50, triangle with normal vectors |
| vi_t    | vertex index                           |
| vi_t    | vertex index of normal vector          |
| vi_t    | vertex index                           |
| vi_t    | vertex index of normal vector          |
| vi_t    | vertex index                           |
| vi_t    | vertex index of normal vector          |

| Type    | Description                            |
|---------|----------------------------------------|
| uint8_t | magic 51, triangle with UV and normal  |
| vi_t    | vertex index                           |
| ti_t    | texture coordinate index               |
| vi_t    | vertex index of normal vector          |
| vi_t    | vertex index                           |
| ti_t    | texture coordinate index               |
| vi_t    | vertex index of normal vector          |
| vi_t    | vertex index                           |
| ti_t    | texture coordinate index               |
| vi_t    | vertex index of normal vector          |

| Type    | Description                            |
|---------|----------------------------------------|
| uint8_t | magic 52, scalable triangle            |
| vi_t    | minimum vertex index                   |
| vi_t    | maximum vertex index                   |
| vi_t    | minimum vertex index                   |
| vi_t    | maximum vertex index                   |
| vi_t    | minimum vertex index                   |
| vi_t    | maximum vertex index                   |

### Voxel Face

If vertices are arranged on a 3D grid, and they are connected by cubes only, then a mesh can be stored in a more compact
form using voxels. Voxel images are described by two kind of chunks. Both are optional, and the second can be repeated multiple
times.

With voxel face, the corresponding vertex and mesh records (in the `VRTX` and `MESH` chunks) are not stored in the file, rather
generated on load. The [M3D SDK](https://gitlab.com/bztsrc/model3d/blob/master/m3d.h) does this if `M3D_NOVOXELS` is *not*
defined, but its algorithm is not effective, being correct is more important.

#### Voxel Types

This is a unique chunk. If exists, it must preceed all the other Voxel Data chunks.

| Type     | Description                                         |
|----------|-----------------------------------------------------|
| uint32_t | magic 'VOXT'                                        |
| uint32_t | length                                              |
| x        | variable length records, voxel type definitions     |

The number of records in this chunk defines voxel data size (vp_t). It's either uint8_t or uint16_t. There's always two implicit
voxel types: -1 unset (or air) and -2 clear (clear to air). For each voxel type, there's a record:

| Type    | Description                                              |
|---------|----------------------------------------------------------|
| ci_t    | diffuse color of the voxel                               |
| si_t    | offset to the string table, name of the voxel / material |
| uint8_t | rotation info                                            |
| uint8_t | voxel shape                                              |
| uint8_t | number of items (N)                                      |
| sk_t    | ** skin index (only if sk_t valid) **                    |
| x       | ** item records (only if N is greater than zero) **      |

First two fields specify the voxel's look. Each fields can be zero, but not both.

Voxel type names are not defined, it can be any name specific to the rendering engine. The only restriction is that space ' '
and directory separators ('/' and '\\') are not allowed in them, and those will be replaced by an underscore '\_', just as
in every other M3D identifiers. These are usually technical names like 'minecraft:cobblestone'. With voxel type names, all the
other voxel type properies (rotation, shape, subitems etc.) can be overriden.

If voxel name matches a material name, then that material specifies how the voxel should look like. Without a name or when the
name does not match any material, simply the given color is used.

In the rotation info byte, when info byte type bit is 0, then bits 0-5 encodes rotations as follows:

| Bits   | Description                      |
|--------|----------------------------------|
| 0-1    | rotation around Y axis           |
| 2-3    | rotation around Z axis           |
| 4-5    | rotation around X axis           |
| 6      | voxel shape most significant bit |
| 7      | info byte type 0                 |

Rotations are stored like 0 = normal, 1 = rotated 90 degree CW, 2 = rotated 180 degree, 3 = rotated 90 degree CCW. With X and Z
rotations being 0, Y rotations mean: 0 = faces North, 1 = faces East, 2 = faces South, 3 = faces West.

Rotations for colored full voxels are typically not used, but makes a lot more sense when the voxels are using materials with
textures, or when non-cubic voxel shape is selected.

Voxel types that look the same no matter the rotation are free to reinterpret the rotation info byte, as long as they don't
forget to set info byte type bit to 1 (OR'd 0x80).

| Bits   | Description                      |
|--------|----------------------------------|
| 0-5    | free to be used by renderer      |
| 6      | voxel shape most significant bit |
| 7      | info byte type 1                 |

The voxel shape divides voxels into 2 x 2 x 2, thus giving 8 partitions. The most significant bit is encoded in the rotation info
byte to give the total of 512 possible voxel shapes. These are numbered as:

```
   +-----+      +-----+      +-----+       +--+
  /     /|     /__   /|     /_____/|      /__/|
 /     / |     | /  / |     |    | |      |  ||
+-----+  |     |+--+  |     |____| |      |__|/--+      +-----+      +-----+      +-----+      +--+
|     |  |     /|  |  |     /    + |     /      /|     /     /|     /__   /|     /_____/|     /__/|
|     |  |    / |  |  |    /     / |    /      / |    /     / |     | /  / |     |    | |     | | |
|     |  +   +--+  |  +   +-----+  +   +------+  +   +-----+  +     |+--+  +     |____|/      |_|/
|     | /    |     | /    |     | /    |      | /    |     | /       |  | /
|     |/     |     |/     |     |/     |      |/     |     |/        |  |/
+-----+      +-----+      +-----+      +------+      +-----+         +--+
   0            1            2             3            4            5            6            7

                 __
                / /+
  ______       +-+ |_       ______       ____
 /     /|     /| |/ /+     /     /|     /   /|
+-----+ |    +-+ +-+ |    +-----+ |    +---+ |
|     |/     |     |/     |     |/     |   | |
+-----+      +-+ +-+      +-+ +-+      +-+ | +
               |_|/         |_|/         |_|/

   8           9            10           11

 0 = full cube, no paritions missing
 1 = one partition missing
 2 = two partitions missing
 3 = three partitions missing
 4 = four partitions missing (half-cube)
 5 = five partitions missing
 6 = six partitions missing, aligned at bottom back
 7 = seven partitions missing, aligned at bottom back left
 8 = six parittions missing, aligned at middle (coloumn)
 9 = one partition wide cross, aligned at middle (cross)
 10 = one partition wide T-shape, aligned at bottom middle
 11 = one partition wide L-shape, aligned at bottom middle left
 12 = cylinder (same as 8, but round)
 13 = cylinder crossed pipe (same as 9, but round)
 14 = cylinder T-shaped pipe (same as 10, but round)
 15 = cylinder L-shaped pipe (same as 11, but round)
 16 = lego top
 17 = lego bottom
 18 - 31 = reserved
 32 - 511 = user defined, free to use by a renderer for any shape they want
```
Note: the voxshape 6 and 8 are identical, except 6 is at the back bottom, while 8 is in the middle. Voxshape 12 to 15 are
very similar to 8 to 11, except they are cylindrical, has round surface. Voxshape 16 is a one third cube high, half cube diameter
cylinder at the bottom middle, while voxshape 17 is the negative of that, it's like a full cube which has a third-cube high
cylinder missing from the bottom middle. With the combination of rotation info byte, it is possible to describe many kinds of
voxel shapes.

The number of items field is typically zero. If it's not zero, then the record contains that many sub-records:

| Type     | Description                                              |
|----------|----------------------------------------------------------|
| uint16_t | count                                                    |
| si_t     | offset to the string table, name of the voxel / material |

This item list is used only with named voxel types, and only for voxels that may include other voxels, like a chest in a
game for example. There every chest in the voxel data has it's unique voxel type id and with that, a unique list of contents.
Those content voxels are normally not displayed, so it is safe for a simple viewer to simply skip this list.

Assigning voxels to skin will allow you to define animated voxel models. There each animation frame will move the bone, which in
turn moves the voxels that are associated with that bone through the skin. It works exactly as how animated skin moves vertices,
only this time voxels (8 vertices at a time) are affected.

This chunk must preceed voxel data chunks and must be followed by at least one voxel data chunk. The number of records in this
chunk specifies the size of the vp_t in the subsequent voxel data chunks.

#### Voxel Data

This is a multiple chunk. Voxels are stored in blocks, and more blocks may exists.

| Type     | Description                                         |
|----------|-----------------------------------------------------|
| uint32_t | magic 'VOXD'                                        |
| uint32_t | length                                              |
| si_t     | offset to the string table, name of the voxel block |
| vd_t     | pX position of the block                            |
| vd_t     | pY position of the block                            |
| vd_t     | pZ position of the block                            |
| vd_t     | sX size of the block                                |
| vd_t     | sY size of the block                                |
| vd_t     | sZ size of the block                                |
| uint8_t  | block uncertainty                                   |
| uint8_t  | alternate block group id                            |
| x        | variable length records, voxel data                 |

If there are more voxel data blocks in the file, their area may overlap, allowing loading of different voxel types into the
same box.

The vd_t type is a signed value. In the overall scheme (concatenated blocks) structures are always stored as centered on the
Z and X axii. The pY value of 0 represents the ground level (the XZ plane). Positive pY values are above the ground, negative
values are under ground.

The blocks can be optional when loaded into the world if uncertainty is greater than zero. It is a reverse probability value,
meaning uncertainty of 0 equals to 100% probability, and 255 equals to 0% probability. If alternate block group is zero, then
the block can be optional, meaning if it's not loaded, then the above blocks must be moved down by substracting this block's sY
from the others' pY, or adding sY if pY is negative. If alternate block group is not zero, then there should be at least one
more block with that same alternate block group id. Blocks in the same group must have uncertainty values sum up to 255. When
loaded, exactly one block of the same alternate block group is loaded.

After the chunk header, comes the voxel data, describing sY \* sZ \* sX voxels. This is an RLE compressed data.

| Type         | Description                    |
|--------------|--------------------------------|
| uint8_t      | N, uncompressed if bit 7 unset |
| (n+1) * vp_t | N+1 times voxel pixel          |

| Type         | Description                    |
|--------------|--------------------------------|
| uint8_t      | N, compressed if bit 7 set     |
| vp_t         | one voxel pixel                |

The repeat number (N) is (record magic & 0x7F). If record magic bit 7 is not set, then N+1 vp_t follows. If bit 7 is set, then
only one vp_t follows, which must be repeated N+1 times. Each voxel (vp_t) is represented either as an uint8_t or an uint16_t,
depending how many voxel types are defined by the preceeding voxel types chunk. If there's less than 254, then uint8_t should
be used and this is specified in the model header's flags.

Voxels are stored in Y, Z, X order, in a right-handed coordinate system, meaning Y means the horizontal layer from bottom to
top, Z is the depth and X is the width. The lowest layer (which is probably the ground level) is stored first, then the layer
above that. Within a layer, the row farest away stored first, and within a row the voxel on the left. +Y is up, +Z is towards the
viewer, and +X is on the right. The voxel not set values are encoded as -1, and clear to empty as -2. Empty (unset) voxels
should be skipped, and original voxel in the world kept. With clear, then the voxel in the world must be cleared to empty.
Non-empty voxels in the chunks always override world voxels upon load.

### Mathematical Shapes

This chunk describes the face by parameterized mathematical formulas of shapes. If it exists, then shapes are the primary
visualization source, and not the polygon mesh.

| Type     | Description                                     |
|----------|-------------------------------------------------|
| uint32_t | magic 'SHPE'                                    |
| uint32_t | length                                          |
| si_t     | offset to the string table, name of the shape   |
| bi_t     | group bone id                                   |
| x        | variable length records                         |

Each record starts with a magic, which defines the type and number of its arguments and thus the length of the record. Just like
with meshes, the magic 0 encodes use material, and string offset 0 means revert to vertex colors.

| Type    | Description                                      |
|---------|--------------------------------------------------|
| uint8_t | magic 0, use material                            |
| si_t    | offset to the string table, name of the material |

Magic value 1 encodes parameterized back-reference:

| Type    | Description                                      |
|---------|--------------------------------------------------|
| uint8_t | magic 1, use shape                               |
| hi_t    | shape index                                      |
| vi_t    | new shape position                               |
| vi_t    | new shape orientation and scaling                |

With record magic 2, a part of the polygon mesh can be included:

| Type     | Description                                      |
|----------|--------------------------------------------------|
| uint8_t  | magic 2, use mesh                                |
| uint32_t | start face index                                 |
| uint32_t | number of face to include                        |
| vi_t     | new mesh position                                |
| vi_t     | new mesh orientation and scaling                 |

Other magic encode shapes with arguments:

| Type    | Description                       |
|---------|-----------------------------------|
| uint8_t | 0xxxxxxx magic 3-127, shape type  |
| x       | arguments                         |

| Type    | Description                       |
|---------|-----------------------------------|
| uint8_t | 1xxxxxxx magic LSB, 128-32767     |
| uint8_t | xxxxxxxx magic MSB shape type     |
| x       | arguments                         |

With two bytes magics, you have to skip the most significant bit of the first byte: type = (magic\[1] << 7) | (magic\[0] & 0x7F).
These records describe shapes like spheres, cylinders, Bezier curves, B-Splines, NURBS etc. For a full list, consult the
[shapes documentation](https://gitlab.com/bztsrc/model3d/blob/master/docs/shape.md).

## Annotation Labels

Labels are optional chunks, and there can be more if there are more annotation groups or translations.

| Type     | Description                                                     |
|----------|-----------------------------------------------------------------|
| uint32_t | magic 'LBLS'                                                    |
| uint32_t | length                                                          |
| si_t     | offset to the string table, name of the annotation group        |
| si_t     | offset to the string table, optional language code              |
| ci_t     | ** color index (only if ci_t valid) **                          |
| x        | several fixed length records                                    |

Both annotation group name and language code are optional. If not specified, encoded as zero offset. If language code given, it
must be 5 characters long, 2 lowercase main language, and '\_' underscore and 2 uppercase region code, like 'en_US' or 'en_GB'.

Records are fixed length, and if there are more translations, then all must have an annotation chunk with the same records,
except for the name offset.

| Type    | Description                            |
|---------|----------------------------------------|
| vi_t    | vertex index                           |
| si_t    | offset to the string table, label      |

These should be visualized as label texts listed on the left and on the right of the model, underlined in the given color and
with the underline's ending connected to the vertex. Those connections must not cross each other.

Annotations are mostly used with complex shapes and [CAD models](https://gitlab.com/bztsrc/model3d/blob/master/docs/CAD.md) and
not with animated polygon models.

## Actions and Animations

Animation frames are optional chunks, and can be present only if there's also a skeleton chunk present. There can be more
animation chunks, each describing frames for a particular motion (called action). For example a 3D model of a human face can have
many chunks with different mouth positions, one for each sound, one for blinking etc. Likewise, a game can use more action chunks,
one for walking, one for attacking, one for dying etc.

| Type     | Description                                                     |
|----------|-----------------------------------------------------------------|
| uint32_t | magic 'ACTN'                                                    |
| uint32_t | length                                                          |
| si_t     | offset to the string table, name of the action (animation)      |
| uint16_t | number of frames                                                |
| uint32_t | overall animation duration in millisecond (1/1000 second units) |
| x        | several fixed length records                                    |

### Frames

Each frame starts with the timestamp and the number of modified bones on that frame. The size of the counter is defined in the
header.

| Type     | Description                   |
|----------|-------------------------------|
| uint32_t | msec timestamp                |
| fc_t     | number of modified bones (N)  |

This is followed by (N) fixed length records.

| Type     | Description                   |
|----------|-------------------------------|
| bi_t     | bone index                    |
| vi_t     | bone position                 |
| vi_t     | bone orientation              |

First you start from the bind-pose skeleton (see BONE chunk). Each frame describes an animation-pose skeleton compared to the
previous skeleton. So the first frame describe modified bone geometry as compared to the bind-pose skeleton. The second frame
describes what's modified compared to the first frame's skeleton, the third frame what's modified compared to the second
skeleton etc.

Using the engine's required FPS and the duration information, bone positions for missing frames have to be generated by
linear interpolating the coordinates, and sprial interpolating the orientation.

The frames are not necessarily sampled at regular intervals of the motion. For example if the animation lasts 10 seconds
(duration 10000), and there are 3 frames in the chunk, at 0, 3000 and 10000 msecs, then to calculate the skeleton at 5000
you'll need the animation-pose skeleton at 3000 and at 10000, and interpolate between the two at 2/7.

Another example, the first frame is at 6000, and you need the skeleton at 5000. Then you take the bind-pose skeleton (using it
as an animation-pose skeleton at 0) and the animation-pose skeleton at 6000, and interpolate at 5/6.

It is possible to store only animations in a Model 3D file, however VRTS/VOXT and BONE chunks must exists in that file too.

## Inlined Assets

Normally texture name refers to an external file, but with these chunks it's possible to embed assets in models, which is
desirable for model distribution. This chunk is optional, and may appear several times.

| Type     | Description                                                                   |
|----------|-------------------------------------------------------------------------------|
| uint32_t | magic 'ASET'                                                                  |
| uint32_t | length                                                                        |
| si_t     | offset to the string table, asset name or identifier                          |
| x        | inlined texture script or image                                               |

When the asset is generated using procedural expression, then the identifier must end in the corresponding script extension
(.lua, .cxc, .cxb, .cxs etc.). For PNG, the extension is omited. A texture should be looked for inlined version and then
fallback on the file system as "(identifier).png", "(identifier)". Other image formats are deliberately not supported: PNG has
a very efficient compression rate, and it supports 32 bit RGBA, 16 bit and 8 bit grayscale. It is almost always enough to store
PNGs with a palette (textures tend to have limited number of colors), which makes it smaller than with any other format. Also
decoder is embedded in the .m3d library.

## End Chunk

This is a unique chunk, and unconventional as it has no length field.

| Type      | Description               |
|-----------|---------------------------|
| uint32_t  | magic 'OMD3'              |
