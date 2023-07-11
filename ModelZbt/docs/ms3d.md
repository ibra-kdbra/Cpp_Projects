Milkshape 3D ASCII File Format Specification
============================================

The Milkshape 3D ASCII format is an extremely simple textual format, very similar to [Wavefront OBJ](obj_spec.md), but unlike
OBJ, it **supports skeletal animations**. It is also widely supported, easy to create from any software, and almost every
modelling software has a plugin for it, which makes it a very good candidate for exchanging animated models. There's a catch
though: this format only supports one bone per vertex. If you need multiple bones with weights, then [Model 3D ASCII](a3d_format.md)
is your best bet (it looks even more like OBJ, if you have ever written an OBJ loader then you should have absolutely no issues
writing an A3D loader either. And A3D is also supported by the M3D SDK if included with the `M3D_ASCII` define), or maybe IQE if
you don't mind having multiple files for a single model (FYI, I do, so IQE is a no-go for me).

As I've said, this is a good *exchange* format, so I do not recommend using this format directly in an engine, because being
textual and easily read by humans also means it's not an efficient format. But it's extremely easy to create and parse, the only
drawback is, there's no usable specification, so I wrote one, reverse engineered from Scorched3D's source. It's going to be short,
because this is a brainfuck simple format.

General Considerations
----------------------

The Milkshape file is line based. Newline characters aren't specified, so I would recommend to be prepared to have both `\r\n`
and `\n` line endings in importers, and always write `\r\n` in exporters. Lines starting with `//` and empty lines must be
skipped, these and multiple empty lines should be handled as if they were a single newline character in the file.

For compatibility, I'd also recommend for exporters to always use rounded floating points representation `[-][0-9].[0-9]`, so
for example `0.9`, `-1.005` etc., and never use exponent notation (like `1e10`, do not use this latter).

The file otherwise consist of blocks. The ordering of the blocks not specified, but all exporters seem to follow the order
described in this spec. For maximum interoperability, I'd recommend importers not to rely on a specific order. Each block
starts with a magic line with the first character being an uppercase Latin letter (`A` - `Z`), contains a double-punct `:`
followed by an integer number. Block data lines *never* start with an uppercase letter, which means importers can skip blocks
they are not interested in.

There are different kinds of blocks, and each block kind can appear in a file only once. So for example there are no multiple
material blocks, one for each material, rather all material descriptions are squashed into a single block.

File Magic
----------

The file starts with a magic line, which is
```
// MilkShape 3D ASCII
```
This is simply followed by the blocks.

Frames
------

These blocks have no data, just an integer in their parameter:
```
Frames: (number of total frames)
Frame: (start frame)
```
Honestly I don't know why are these stored, because animations have their own number of frames, and they also encode timestamps.
Probably just needed by the original software only, and you can safely skip these.

Meshes
------

A mesh block starts with this magic:
```
Meshes: (number of meshes)
```
This is followed by that many data blocks. Each data block starts with a line, followed by three data sub-blocks. Each sub-block
starts with the number of lines in that sub-block, followed by that many lines. It is actually simpler than this sounds at first:
```
"(name of the mesh)" (flags) (material index)
(number of vertices)
(flags) (x) (z) (y) (u) (v) (bone index)        * number of vertices
(number of normals)
(x) (y) (z)                                     * number of normals
(number of faces)
(flags) (v1) (v2) (v3) (n1) (n2) (n3) (group)   * number of faces
```
And this is repeated for each mesh.

Name is always enclosed in quotes. I haven't seen any models with a quote in its mesh name, so about escaping, your guess is just
as good as mine (not blackslash quote `\"`, that's for sure, because I've seen blackslash in strings and those weren't escaped
as double `\\`). Flags should be skipped (usually 0), and finally material index is -1 if not specified. Otherwise it's just a
simple zero-based index to the materials block.

In vertices sub-block, flags should be skipped too (usually 0), `x`, `y`, `z` are simple coordinates stored as floating point.
NOTE: that the coordinate system is +Y up, +Z towards the viewer, like in any normal right-hand Cartesian coordinate system.
The `u` and `v` are texture coordinates, again, stored as floats, and for GL you'll have to flip `v` (so use `1.0 - v`). Bone
index is simply a zero-based integer indexing the bones, and it is -1 if not specified. (NOTE: with this format you cannot have
skins with weighted bones. Consider this as having a skin where there's only one bone per vertex with the implicit weight of 1.0.
Try the [A3D](a3d_format.md) format if you need more bones with different weights in a plain text file.)

The normals sub-block, is simple too, `x`, `y`, `z` are again coordinates stored as floating point. It worth mentioning that
most files have the same number of vertices and normals, but it's not always the case! Do not rely on this! Assume there are
different number of normals than number of vertices.

Faces sub-block contains the triangle definitions (no other polygons supported). Here flags again should be skipped (usually 0),
`v1`, `v2`, and `v3` are indexing a vertex from the vertices sub-block of this mesh, starting at 0. Normals are likewise,
`n1`, `n2`, `n3` are indexing the normals sub-block of the mesh, also starting at 0. The order is clock-wise, so for a
right-handed coordinate system you'd have to swap the second and the third index to get a CCW triangle. Finally `group` is the
smoothing group number, which I don't get why is needed as the format stores per triangle normal vectors. Probably you can safely
skip it.

Materials
---------

A material block starts with this magic:
```
Materials: (number of materials)
```
This is followed by 9 lines for each material:
```
"(name of the material)"
(ambient color red) (ambient color green) (ambient color blue) (ambient color alpha)
(diffuse color red) (diffuse color green) (diffuse color blue) (diffuse color alpha)
(specular color red) (specular color green) (specular color blue) (specular color alpha)
(emissive color red) (emissive color green) (emissive color blue) (emissive color alpha)
(shininess, specular exponent)
(overall alpha)
"(diffuse color texture file)"
"(alpha channel texture file)"
```
All numbers are floating points. There's not much sense in storing the `overall alpha`, because each color has it's own alpha
value. Probably you should just skip it. Likewise, the `alpha channel texture file` makes not much sense, because the
`diffuse color texture file` usually stores the alpha channel too. In all files I've seen, the `alpha channel texture file` value
was empty, `""`, but may be useful if the diffuse texture is a JPEG (however I'm not sure in what format the alpha image should
be, so it's probably better to stick to TGA, BMP and PNG diffuse textures with 32 bit pixels, so with an alpha channel).

In many model files I've seen texture files with *absolute paths*. Seriously, **NEVER** do that. That path might exists on your
computer, but I guarantee nobody else has a `C:\Users\lamanoob\AwesomeModels\converted_texture_images` directory...

Bones (and Animations)
----------------------

A bone block starts with this magic:
```
Bones: (number of bones)
```
This is followed by that many data blocks. Each data block starts with three lines, followed by two data sub-blocks. Each
sub-block starts with the number of lines in that block, followed by that many lines:
```
"(name of the bone)"
"(name of the parent bone)"
(flags) (x) (y) (z) (roll) (pitch) (yaw)
(number of position changes)
(time) (x) (y) (z)                              * number of position changes
(number of rotation changes)
(time) (roll) (pitch) (yaw)                     * number of rotation changes
```
Name of the bone and the parent's name are always enclosed in quotes. If the parent is missing, that line has an empty string,
`""`. NOTE: there's no guarantee that parent bone is stored first in the file! The positions as well as the rotations are stored
with floating point numbers. The rotations are in radians, meaning 0.0 means 0 degree, and 1.0 means 360 degrees.

Time is stored as float too, and its unit is *seconds*. In all files I've seen, the number of position and rotation changes
were the same, and they all had the same times. What's more, they were the same for all bones. (So for example, if the first
bone position change has 3 times, let's say 0.1, 0.5, and 1.0, then the rotaion change has these same values, and all the other
bones' positions and rotations have these same 0.1, 0.5 and 1.0 values. This probably isn't mandated by the file format, but
I haven't seen any model file having otherwise.)

The coordinates in the third line serve for the bind-pose. Animations are stored in position and rotation change lines. These
are relative changes, meaning you have to add these to the bind-pose, and for bones that have a parent, you also have to add
them to the parent's bind-pose values.

To convert the rotations into a quaternion, use
```c
sr = sin(roll/2);  cr = cos(roll/2);
sp = sin(pitch/2); cp = cos(pitch/2);
sy = sin(yaw/2);   cy = cos(yaw/2);

quat.x = sr * cp * cy - cr * sp * sy;
quat.y = cr * sp * cy + sr * cp * sy;
quat.z = cr * cp * sy - sr * sp * cy;
quat.w = cr * cp * cy + sr * sp * sy;
```

Or using matrixes: to get the bind-pose, rotate an identity matrix by the rotation values and translate that to the position.
If the bone has a parent, multiply this by the parent's bind-pose matrix (calculated the same way).

To get an animated matrix, rotate an identity matrix by the new rotation values and translate that to the new position.
Multiply this matrix with the bind-pose matrix to get the final transformation matrix for that animation frame (and if
that bone has a parent, multiply it again with the parent's animated matrix).

That's about it. Hope this is going to be useful.
