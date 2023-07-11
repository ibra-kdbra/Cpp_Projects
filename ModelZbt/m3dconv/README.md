Model 3D Converter
==================

A small command line tool to convert [Model 3D](https://gitlab.com/bztsrc/model3d/blob/master/docs/m3d_format.md) files.

------------------------------------------------------------------------------------------------------------------------

**NOTE:** you don't need the patch in `../assimp`, this tool only uses the assimp library to read in formats which aren't
supported natively. The **m3dconv** tool works with an *unpatched* stock assimp library (shipped by your distro) perfectly!

------------------------------------------------------------------------------------------------------------------------

Main purpose: many format libraries as well as [assimp](http://assimp.org) requires external linkage, and using them is a tricky
thing to do, which can get messy pretty quickly if you need skeletal information too. It requires lots of memory, and it is slow
(you have to match nodes by comparing strings). With this tool you can incorporate all of that complexity and overhead into your
build environment, and in your application you can relax and deal with just a simple, truly uniform 3D model structure that has
a more effective index only representation.

```
Model 3D Converter by bzt Copyright (C) 2019-2022 MIT license

./m3dconv [-0|-1|-2|-3] [-u|-A] [-i|-e] [-t <n>] [-V <n>] [-F] [-x|-X] [-y|-Y]
    [-z|-Y] [-R] [-r] [-g|-G] [-w] [-C] [-N] [-U] [-M] [-f <delay>] [-s <size>]
    [-n <name>] [-l <lic>] [-a <author>] [-c <comment>] [-v|-vv] <in> <out.m3d>
./m3dconv -m [-u|-A] [-i] [-n <name>] [-l <license>] [-a <author>]
    [-c <comment>] [[-p|-P] <pal>] <in1.m3d> [<in2.m3d> ...] <out.m3d>
./m3dconv [-d|-dd|-ddd|-D] <in.m3d>

 -0:    use  8 bit coordinate precision (int8_t 1/256, default)
 -1:    use 16 bit coordinate precision (int16_t 1/65536)
 -2:    use 32 bit coordinate precision (float, default for NURBS)
 -3:    use 64 bit coordinate precision (double)
 -u:    save uncompressed binary model
 -A:    save in ASCII format
 -i:    inline assets (like textures)
 -e:    extract inlined assets
 -t:    triangulate shapes and voxels
 -V:    voxelize mesh
 -x:    rotate model +90 degrees clock-wise around its X axis in place
 -X:    rotate model +90 degrees clock-wise around the X axis
 -y:    rotate model +90 degrees clock-wise around its Y axis in place
 -Y:    rotate model +90 degrees clock-wise around the Y axis
 -z:    rotate model +90 degrees clock-wise around its Z axis in place
 -Z:    rotate model +90 degrees clock-wise around the Z axis
 -R:    mirror coordinates on Z axis
 -r:    convert to right-handed coordinate system
 -g:    move model to ground (only positive Y, and centered X, Z)
 -G:    move model global, to absolute center
 -w:    don't use world transformation
 -C:    don't normalize coordinates (only for debug purposes)
 -N:    don't save normal vectors
 -U:    don't save texture UV coordinates
 -F:    flip UV
 -f:    set framedelay multiplier
 -M:    export mesh only, no skeleton or animation
 -s:    set model's size in SI meters (float)
 -n:    set model's name
 -l:    set model's license string ("MIT", "CC-0", "GPL" etc.)
 -a:    set author's name, contact (email address or git repo etc.)
 -c:    set comment on model
 -m:    create material library
 -O:    don't use our importers, use Assimp's (does not support NURBS)
 -v:    verbose
 -d:    dump contents of a M3D file
 -D:    dump the m3d_t in-memory structure
 -p:    use voxel palette
 -P:    force voxel palette
 in:    input file(s), any format that assimp can handle
 out:   output M3D filename
```

[[_TOC_]]

Usage Examples
--------------

The following input formats are supported:

 - Model 3D [binary](https://gitlab.com/bztsrc/model3d/blob/master/docs/m3d_format.md) and [ASCII](https://gitlab.com/bztsrc/model3d/blob/master/docs/a3d_format.md) (of course)
 - [Wavefront OBJ](https://gitlab.com/bztsrc/model3d/blob/master/docs/obj_spec.md), simple plain text format for static models (additionally to meshes: negative indices, polylines, Bezier and B-spline curves, Bezier surfaces and NURBS too)
 - [Milkshape 3D](https://gitlab.com/bztsrc/model3d/blob/master/docs/ms3d.md), simple plain text format for skeletal animations (only one bone per vertex and only one animation per model, if you need more, consider Model 3D ASCII)
 - Autodesk [FBX](https://github.com/bqqbarbhg/ufbx), both binary and ASCII variant (work in progress, for more complex models and animations)
 - [BINVOX](http://www.patrickmin.com/binvox/binvox.html), [MagicaVoxel](https://github.com/ephtracy/voxel-model/blob/master/MagicaVoxel-file-format-vox.txt), [Qubicle](https://getqubicle.com/qubicle/documentation/docs/file/qb), [Minecraft Schematics](https://minecraft.gamepedia.com/Schematic_file_format); for importing voxel images
 - Blender files (work in progress, but there's also a [Blender plugin](../blender) which is fully functional)
 - for every other formats (or with the `-O` flag even for the built-in ones), the support is provided via [assimp](http://assimp.org).

Output formats:

 - Model 3D binary and ASCII

### Converting From Other Formats

```
./m3dconv example.3ds example.m3d
```
Supported features: vertices (with colors), indices, materials, textures, UV coordinates and normals, bones, skin, animations,
inlined assets.

By default, the .m3d file will store coordinates with the smallest 8 bit precision. If that is not enough for you (model loses
details, or becames "squarey"), then
```
./m3dconv -1 example.obj example.m3d
```
saves model using a 65536 x 65536 x 65536 grid, and `-2` uses floating point numbers. The WusonBlitz0.m3d model in the
[models](https://gitlab.com/bztsrc/model3d/tree/master/models) directory was saved using a 256 x 256 x 256 grid without loosing
significant quality. The WusonBlitz1.m3d uses 65536 x 65536 x 65536 grid, and it is indistinguishable from the WusonBlitz2.m3d,
which uses floating points. Without stream compression, each quality level requires approx. twice the storage space than the
previous level.

Regardless which quality level you choose, you'll always get floating point coordinates in the in-memory structure.

### Model Orientation, Position and Normalization

Model's orientation in M3D format is important. You must save the model so that it would face towards you, when +Y is up, +X is
on the right and +Z is front. If the original model has different orientation (some CAD software developers were missing from
school when they learned about Cartesian / Descartes coordinate system), then you can use `X`, `Y`, `Z` flags to rotate the
model around the given axis by +90 degrees clock-wise. You can also repeat those flags, for example to rotate around the Y axis
by +180 degrees and +90 degrees around the X:
```
./m3dconv -YYX example.3ds example.m3d
```
Note that you can rotate the model around its own axii as well. For example, if a model has only positive Y coordinates, then
rotating around the Z axis 180 degree would change those coordinates into negative ones (obviously). But if you use lowercase
letters, like `-z`, then the model will be rotated in place, meaning all of the Y coordinates would remain positive.

Another often issue is that the original model was saved for D3D with -Z towards the viewer, but the other two axii is correct.
You can mirror such models on the Z axis with `-R`. This is implemented as a rotation flag, so its place in the command arguments
matters, and it does not influence the ordering of polygon edges (for that, see `-r` below).

If you want to move the model to the ground (X, Z plane), use
```
./m3dconv -g misplaced.3ds onground.m3d
```
This will center the model along the X and Z axii, and move it to positive Y coordinates so that the smallest Y coordinate
will be 0.0. You can also center on the Y axis too with `-G` (center globally).

If the model looks inside-out, then most likely it was created in a left-handed coordinate system. Exporters should never do
that, they must save models in a right-handed system. However if you run into a model which is bad, you can fix it with:
```
./m3dconv -r lefthanded.blend righthanded.m3d
```
This will change the order of triangle points from CW to CCW. (But does not touch the coordinates like `-R`.)

Models saved in M3D files must have coordinates normalized in -1.0 .. 1.0, meaning they must fit in a unit cube, and they must
fill up the unit cube. This is calculated automatically by the converter. If you want to avoid this coordinate normalization, use
the `-C` command line flag, but this is for debugging only. Never use unnormalized models. If you are not satisfied how the
converter scales the model, scale it yourself manually before you convert it. This option also influences the quality setting,
as only normalized coordinates can be saved storage efficiently.

### Converting Parameterized Shapes and Voxel Images

By default if the input format supports these, then they will be converted into M3D keeping their types (surfaces like NURBS as
shapes and voxel images as voxels). However if there's a need, you can convert them into a mesh by triangulation. That requires
one argument, the size of the resulting triangles. That is the sampling rate in case of surfaces:
```
./m3dconv -t 0.001 shapes.step onlymesh.m3d
```
For voxel images, the argument is the size of one voxel box:
```
./m3dconv -t 0.1 minecraft.schem onlymesh.m3d
```

### Voxelizing Parameterized Shapes and Triangle Meshes

The other direction is also possible, converting shapes and triangle meshes into voxel volumes. The argument defines the
volume's grid size. For example `-V 128` will voxelize the model into a 128 x 128 x 128 volume.
```
./m3dconv -V 128 mymodel.obj onlyvoxel.m3d
```

### Converting Animations

Some formats does not store FPS value, or files may have incorrect FPS (quite often 0). For those models it is impossible to
determine the duration of an animation, therefore they would be converted as one frame per tick, which is 1000 frames per second.
To corrigate, you can specify a frame delay multiplier, which affects the frame's timestamp. For example to tell that each frame
should take 3 times longer,
```
./m3dconv -f 40 notickpersec.b3d goodfps.m3d
```
This will multiply the frame's millisec timestamp by 40, thus encoding 25 frames per second. To check the actions' durations
and each frame's timestamps, you can use the `-dd` flag to dump frames, see below.

### Creating a Model for Distribution

For that, you should specify the model's name, license, name the author, and you should inline textures into the model file:
```
./m3dconv -n "A cube" -l "GPL" -a "AtomAnt aacoolmodels@github" -i example.blend distributed.m3d
```
Double check if the model's orientation is correct. If you have compiled m3dconv with EXPINLINE, then all Assimp textures (the
ones that were inlined in the original model too) will be automatically exported and converted into PNGs for embedding in the
output. Without you have to provide PNG textures only.

### Extracting Textures from Distributed Model

To do the opposite, use
```
./m3dconv -e distributed.m3d ingamemodel.m3d
```
This will extract all textures as PNG files into the same directory, leaving only the references to them in the second M3D file.

This also works on material libraries with inlined textures:
```
./m3dconv -e mtllib.m3d mtldefonly.m3d
```

### Creating ASCII output

A3D is a human readable format, similar to Wavefront OBJ, but it does not use separate mtl files, instead includes material
definitons, and it's a bit more compact than OBJ. Just as OBJ, it can store meshes, curves, Bezier surfaces and NURBS, but
unlike OBJ A3D also supports skeletons and animations.
```
./m3dconv -A example.pbx example.a3d
```
The converter can also read these [.a3d ASCII variants](https://gitlab.com/bztsrc/model3d/blob/master/docs/a3d_format.md) and
convert them into binary .m3d files.

### Creating Material Library

To remove material chunks from models and collect them into a separate file, do:
```
./m3dconv -m model1.m3d model2.m3d model3.m3d mtllib.m3d
```

### Dumping

You can dump all bytes in hex and explanation next to them. For example, to show all chunks in a file and decode chunk contents:
```
./m3dconv -dd example.m3d
```
It can quickly check if a model has a skeleton for example, and very useful if you're writing your own parser.
This also validates the model file, looking for inconsistencies. If anything is wrong with the file, it will tell.

Dumping the in-memory structure is also possible, which is very useful when you're integrating m3d_t into your engine. It can
also tell you how would a certain model be represented in-memory if it were loaded from an .m3d file.
```
./m3dconv -D example.blend
```
Note the difference between `-d` and `-D`: the former dumps the raw M3D file before it is parsed, while the latter dumps the
in-memory format after it has been parsed.

This functionality is also available as a web based [Model 3D file validator](https://bztsrc.gitlab.io/model3d/validator).

### Compilation

If you want to compile `m3dconv` from source, that's easy, you have three options:
```
make
PROFILING=1 make
EXPINLINE=1 make
```
The first is the simplest, and it only depends on `libassimp`. The second requires gettimeofday() system call, and adds
microsecond precise time measurements for importing / exporting models. The third one is very useful, it exports all textures
(inlined too) in any image format that Assimp understands (BMP, TGA, JPEG etc.), into inlined PNGs on-the-fly. This makes the
distribution format creation simple, as you don't have to manually create PNG textures for inlining, however it depends on
two additional libraries, `libpng` and `libimagequant`.


# TODO

- convert STEP geometry to M3D shape commands (step.h, step_geom)
- shape triangulation (tri.h, tri_shapes)
- voxel triangulation (tri.h, tri_voxels)
- shape voxelization (voxel.h, voxelize_shapes)
- mesh voxelization (voxel.h, voxelize_face)
- blender file format parser (blend.h, blend_load)
- materials and animations in FBX format parser (fbx.h, fbx_load)
