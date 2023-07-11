Model 3D File Format Specification
==================================

- File extension: `.a3d`
- Mime type: `text/x-3d-model`

This file describes the ASCII variant of the Model 3D file format (see
[binary](https://gitlab.com/bztsrc/model3d/blob/master/docs/m3d_format.md) format). The purpose of the ASCII variant is
to be readable by humans and easy parsing, editing; however it requires a lot more storage space than the binary version,
so models should be primairly stored and distributed in the binary format.

Chunks are always started by a magic line and ended in an empty line. In the file whitespace separator could be one or more spaces
or tabs, and lines are ended in a '\n' newline character. If '\r' does exist before '\n', it's simply ignored. The SDK always
emits '\r\n' so that Windows users too can read the ASCII variant easily. Floating point numbers are represented in a "1.0" form,
optionally with an exponent suffix, like "1.0e-2". Coordinates are always expressed in floating points, indices as integers.

Color map and skin chunks are not stored separately, they are included in the "Vertex" block. When converting from binary,
inlined texture chunks are saved in separate files, by the name "(identifier).png".

Materials are not stored in separate files as with Wavefront OBJ, but preferably included as chunks, however the format supports
separated libraries as well. For a linked material library, only the material's name is stored, not the definitions, and there
should be another model with only the definitions. There it is the loader application's responsibility to link a 3D model with
the model containing the materials.

[[_TOC_]]

## Header

Starts with the magic string "3dmodel" (lowercase, in contrast to the binary variant which has uppercase magic).

```
3dmodel (scale factor)
(name of the model)
(license)
(author)
(comment or description, this may
be more lines, but without empty lines)

```

Note that there's an empty line terminating the header. Typical '#' comments are not allowed to ease parsing, however the header
contains a comment block which may be used for that purpose (the reason for this is that the binary format supposed to contain
the comments on the model).

Comment may contain structured data. In that case the first comment line includes a magic word to identify the keywords in the
other lines. The magic 'ISO-10303' means keywords are originated from a STEP schema, and magic 'META' can be used to say there's
no specification, but description is structured. Example:

```
3dmodel 1.0
Example model
GPL
me
META
GENERATOR=myengine-1.0
Created=2019-11-18T00:00:00

```

## Preview Image

```
Preview
(preview image).png

```
Optional. If exists, then the given PNG file will be included as a preview chunk. On output, it's just the model's name dot png.

## Texture Map

```
Textmap
(u) (v)
(u) (v)
(same as Wavefront OBJ's, without the "vt " prefix)

```

## Vertex Chunk

```
Vertex
(x) (y) (z) (w)[ (color code)] [(bone id)[:(weight)] [(bone id):(weight)...]]
(x) (y) (z) (w)[ (color code)] [(bone id)[:(weight)] [(bone id):(weight)...]]
(similar to Wavefront OBJ's, without the "v " prefix, but has more options)

```

Color code uses a hex HTML color notation '#AARRGGBB' and it's optional.

If skeleton is used, then there can be 0 to 8 additional bones and weights for each points (bone id selects a line in "Bones"
chunk, see below). If there's only one bone id, then the weight is optional, otherwise the weights must sum up to 1.0 in total.

M3D does not differentiate between vertex and vertex normal coordinates, both are listed in this chunk (and if there's a skeleton,
then orientation quaternions too).

## Skeleton

```
Bones
(v) (o) (bone name)
[parent level/](v) (o) (bone name)
```

The (v) is an index to a vertex, and selects the zerus point for the bone, and (o) selects another vertex, but it is interpreted
as an orientation quaternion. Children bones are prefixed by '/' to the required level (unfortunately tab or space can't be used
as text editors, most notably IDEs tend to threat those freely in plain text files). Parent bone definiton must preceed the
childrens. Example:
```
0 1 body
/2 3 head
//4 5 mouth
/6 7 left_arm
/8 9 right_arm
```

## Materials

```
Material (name)
Kd (color code)
Ka (color code)
Ns (number)
map_Kd (texture name)
map_bump (texture name)
  ...
(similar as Wavefront OBJ's mtl, but not entirely identical)

```
The magic bytes in records are substituted with short keywords, similar to OBJ's mtllib. For keywords, see the Model 3D
[binary format](https://gitlab.com/bztsrc/model3d/blob/master/docs/m3d_format.md) documentation, as it lists them in the
corresponding records under the section Materials. Note it is possible to store only materials in a Model 3D file, and omiting
material chunks in other mesh Model 3D files, thus creating a shared material library. Also it is possible to generate textures
for the materials on-the-fly, by referencing scripts as texture names, see
[procedural models](https://gitlab.com/bztsrc/model3d/blob/master/docs/procedural.md) section Procedural Texture Generation.

## Face

```
Procedural[ name, just a comment for designers]
(script name)

```
For more details, see [procedural models](https://gitlab.com/bztsrc/model3d/blob/master/docs/procedural.md) section
Procedural Surface Generation.

```
Mesh[ name, just a comment for designers]
use[ (material name)]
par[ (parameter name)]
(v) (v) (v)
(v)/(t) (v)/(t) (v)/(t)
(v)//(n) (v)//(n) (v)//(n)
(v)///(m) (v)///(m) (v)///(m)
(v)/(t)/(n)/(m) (v)/(t)/(n)/(m) (v)/(t)/(n)/(m)
(same as Wavefront OBJ's, without the "f " prefix)

```
Multiple meshes should be loaded into a single mesh list. A polygon can contain 1 to 15 vertices, however the loader and exporter
right now is limited to triangles only. A simple "use" keyword without a material name reverts to vertex colors. You can change
the material any time you like, the exporter will group faces of the same material together which produces a smaller model file.
The "par" keyword selects a parameter, and one without a name reverts to no parameter (and no ///(m) maximum vertex records
allowed).

```
Shape[ (name)]
[group (bone id)]
use[ (material name)]
inc (shape id) (position vertexid) (orientation and scaling quaternion vertexid)
mesh (start face id) (number of polygons) (position vertexid) (orientation and scaling quaternion vertexid)
(command) [arguments]
```
Describes the model by a list of parameterized mathematical shape commands. Uses shperes, cylinders, Bezier curves, B-Splines,
NURBS, etc. For more details, see [shape documentation](https://gitlab.com/bztsrc/model3d/blob/master/docs/shape.md).

Shapes can be grouped together. When converting from [STEP](https://gitlab.com/bztsrc/model3d/blob/master/docs/CAD.md)
files, shells will be saved as bones and each face will be a separate shape. Faces that belong to the same shell will get the
same group bone id.

```
VoxTypes
#(voxel type color)[/(rotation byte hex)[/(voxshape hex)] (voxel type name) [(bone id)[:(weight)] [(bone id):(weight)...]] [ '{' items '}' ]
#(voxel type color)[/(rotation byte hex)[/(voxshape hex)] (voxel type name) [(bone id)[:(weight)] [(bone id):(weight)...]] [ '{' items '}' ]
#FF007F00/01/000 default:chest { 1 default:apple 3 default:spoon }
```

Specifies the voxel type map for the subsequent voxel data chunks. If items specified, then it's a space separated list of
count + voxel type name pairs.

```
Voxel[ (name)]
uncertain (percentage)[ (alternate block group id)]
pos (x) (y) (z)
dim (w) (h) (d)
layer
(voxel type idx) (voxel type idx) ...
(voxel type idx) (voxel type idx) ...
layer
(voxel type idx) (voxel type idx) ...
(voxel type idx) (voxel type idx) ...
```

Describes a group of voxel image face. Could be more of these chunks. This should be mutually exclusive with meshes and shapes
chunks (but that's not a strong constraint, if both mesh or shape and voxel exists, then voxel face should be automatically
converted to mesh). Each line must contain x ids, in z lines. Each y block is preceeded by the 'layer' command. Unset voxels
(-1) are represented as '.' and clear to unset (-2) as '-'.

## Annotation Labels

```
Labels[ (name)]
[color (color)]
[lang (language code)]
(vertex id) (label text)
(vertex id) (label text)

```
Both `color` and `lang` are optional. Language code is in the form "xx_XX". Annotations are mostly used with complex shapes
and [CAD models](https://gitlab.com/bztsrc/model3d/blob/master/docs/CAD.md) and not with animated polygon models.

## Actions

```
Action (durationmsec) (name)
frame (msec)
(bone id) (v) (o)
(bone id) (v) (o)
frame (msec)
(bone id) (v) (o)
(bone id) (v) (o)

```
Bone id is an index to the "Bones" chunk. Values override the bone's position or orientation for that specific frame.
Note that animations can be stored separately to models. That way the same animation can be applied to more bone structures,
as long as the number of bones and the bone hierarchy are identical.

## Inlined Assets

```
Assets
(filename1).png
(filename2).png

```
Optional. If given and the files found, then the textures will be embeded into the model.

## Engine Specific Extra Chunks

```
Extra (name)
(xx) (xx) (xx) ...

```
The name of an extra chunk is always 4 characters, Latin letters, numbers, or ASCII symbols ('_', '!', '@', '#' etc.), but
not space, tab or newline character. Whitespaces and control characters are not allowed. Chunk data is encoded in one or
more lines, containing only hex encoded bytes, each two digits (0-9A-F).

## End Chunk

```
End
```

The only chunk that may not have a terminating empty line.
