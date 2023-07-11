CAD Data in Model 3D Files
==========================

The [Model 3D](https://gitlab.com/bztsrc/model3d/blob/master/docs/m3d_format.md) format is capable of storing CAD-related
information and basic meta information. This document summarize them and their usage.

[[_TOC_]]

Shapes
------

For CAD, it is preferable to use [shapes](https://gitlab.com/bztsrc/model3d/blob/master/docs/shape.md) instead of polygon meshes.
Reading parameterized shapes from Wavefront OBJ files is straightforward, the shapes documentation also has examples.
Shapes can store geometrical shapes (like cube, cylinder etc. hence the name) and Bezier surfaces as well as NURBS. Shape
definitions are recursive, you can include a geometry into the shape-space again but with different position and orientation.
It is also possible to include part of the mesh into shape-space.

Grouping Mesh for Inclusion
---------------------------

Unlike shapes which are groupped in a hierarchy by design, meshes aren't. So you can select parts of the mesh by a start face
index and the number of polygons to include.

Annotation Labels
-----------------

The M3D format is capable of storing labels for vertices. It is advisable to add a new vertex in shape-space for each label,
but not necessary. The exporter will detect if the additional vertex is the same as the one in the shape, and it will only
store it once.

Labels can be grouped together in layers, a color assigned to them, and they can be translated into several languages. M3D file
stores all strings as UTF-8, so international texts shouldn't be a problem.

For translation, a 5 character language code can be given in the form "xx_XX" like "en_US" and "en_GB". If translations are used,
then there should be identical label groups in the file with the same layer identifier and vertex list, but with different string
offsets and language code. It is possible to define entirely different annotations for each language (even with different number
of vertices or different grouping), but highly discouraged.

Labels should be visualized as texts listed on the left and on the right of the model, underlined in the given color and
with the underline's ending connected to the vertex. Those connections must not cross each other.

Materials
---------

M3D material definitions are flexible. If material name is not enough to uniquely identify a physical material for some reason,
then PBR properties can help you: there's a property for roughness and thickness for example in addition to standard 3D model
material properties like diffuse color. If those are not enough, [let me know](https://gitlab.com/bztsrc/model3d/issues) and I'll
add a new property type. See [Model 3D](https://gitlab.com/bztsrc/model3d/blob/master/docs/m3d_format.md) specification section
Materials.

Meta Information
----------------

Any kind of UTF-8 encoded meta information can be embeded into M3D files. There's a description field for that, which must store
the meta information in the following structure: first line is magic, identifies the scheme (defines available keywords). Other
lines contain "(keyword)=(value)" pairs, one assignment per line. Regardless to the scheme, one keyword,
"GENERATOR=(CADsoftware unixname)-(version)" always recognized. The keywords must not contain space; whitespaces at the
beginning of the line and around "=" not allowed. The format of the value otherwise is free-form, depends on the keyword. If the
first line is not recognized as a scheme magic, then the description is an entirely free-form comment (but without empty lines)
and does not contain CAD-related information. Currently one scheme is defined, "ISO-10303", which is followed by any
model-related assignment that is allowed in STEP file "DATA;" blocks and which does not describe topology or geometry. Comments
in the description must be removed, it should be easily parsable by programs, not by humans. Entity references must be resolved
by entities embeded, for example
```
/* some comment */ #3 = APPLICATION_PROTOCOL_DEFINITION( 'international standard', 'automotive_design', 2001, #57 );
#57 = /* the context */ APPLICATION_CONTEXT( 'core data' );
```
becames
```
APPLICATION_PROTOCOL_DEFINITION=('international standard','automotive_design',2001,APPLICATION_CONTEXT('core data'));
```

Note that aside from the description field, there's a designated model's name, license and author field in the files.
See [Model 3D](https://gitlab.com/bztsrc/model3d/blob/master/docs/m3d_format.md) specification section String Table.

Units
-----

The M3D format doesn't care about the application's unit system. It stores normalized coordinates and what 1.0 in model space
means in SI meters. It is the CAD software's responsibility to convert into whatever unit system the user prefers and display
distances in that on screen. There's no place for configurable unit systems in a model file, for interoperability it must use
one standard unit, period. That's what SI is for.

Preview
-------

To help quickly choosing the right model, CAD software can embed and load a preview image of the model in PNG format.

Converting STEP Files into M3D
==============================

The [m3dconv](https://gitlab.com/bztsrc/model3d/tree/master/m3dconv) utility is capable of reading geometry and topology data
from [ISO-10303-21-4](https://www.iso.org/standard/76142.html) and [ISO-10303-24-2](https://ap242.org) files, or informally
(but much more commonly) called STEP files. These usually have the extension _.stp_ or _.step_ and can be exported from all
major CAD software. Import is based on Sreeramulu's and Rao's Geometric Data Extraction Algorithm with lots of reverse
engineering of files I found on the internet, eg. in the [ABC Dataset](https://deep-geometry.github.io/abc-dataset).

Locating the Actual Data
------------------------

The parsing starts by locating closed shells with their transformations. The converter recognizes the following entities:

| STEP Entity                             | Description                                    |
|-----------------------------------------|------------------------------------------------|
| `MANIFOLD_SURFACE_SHAPE_REPRESENTATION` | one possible root with transformation          |
| `ADVANCED_BREP_SHAPE_REPRESENTATION`    | another possible root with transformation      |
| `FACETED_BREP_SHAPE_REPRESENTATION`     | another possible root without transformation   |
| `SHELL_BASED_SURFACE_MODEL`             | contains list of shells without transformation |
| `MANIFOLD_SOLID_BREP`                   | exactly one shell without transformation       |
| `FACETED_BREP`                          | exactly one shell without transformation       |
| `AXIS2_PLACEMENT_3D`                    | strange way to define position and orientation |
| `CLOSED_SHELL`                          | contains list of geometry entites              |

To locate, several paths are checked. Here `-1->` means one-to-one relation, and `-*->` means one-to-many.
```
MANIFOLD_SURFACE_SHAPE_REPRESENTATION -1-> SHELL_BASED_SURFACE_MODEL -*-> CLOSED_SHELL
                                      \1-> AXIS2_PLACEMENT_3D

ADVANCED_BREP_SHAPE_REPRESENTATION -1-> MANIFOLD_SOLID_BREP -1-> CLOSED_SHELL
                                   \1-> AXIS2_PLACEMENT_3D

FACETED_BREP_SHAPE_REPRESENTATION -1-> FACET_BREP -1-> CLOSED_SHELL (with identity transform)

CLOSED_SHELL (with identity transform)
```
Yes, you see that right, shell based surface refers to more closed shells with the same transformation, which makes no
sense unless the shells are not in their local space as they should be.

The last, the direct closed shell path is only used as a last resort, when the other paths failed.

Parsing Surfaces
----------------

Each shell is converted to a world-space bone. The shells are containing further topology for faces:

| STEP Entity     | Description                     |
|-----------------|---------------------------------|
| `FACE_SURFACE`  | contains a list of more records |
| `ADVANCED_FACE` | contains a list of more records |

```
CLOSED_SHELL -*-> FACE_SURFACE -*-> FACE_OUTER_BOUND / FACE_BOUND / (BOUNDED_SURFACE...)
                               |  /> PLANE -1-> AXIS2_PLACEMENT_3D
                               \-1-> CYLINDRICAL_SURFACE -1-> AXIS2_PLACEMENT_3D
                                  \> SPHERICAL_SURFACE -1-> AXIS2_PLACEMENT_3D

CLOSED_SHELL -*-> ADVANCED_FACE -*-> FACE_OUTER_BOUND / FACE_BOUND / (BOUNDED_SURFACE...)
                                |  /> PLANE -1-> AXIS2_PLACEMENT_3D
                                \-1-> CYLINDRICAL_SURFACE -1-> AXIS2_PLACEMENT_3D
                                   \> SPHERICAL_SURFACE -1-> AXIS2_PLACEMENT_3D
```

Parsing Geometry
----------------

Once the topology is known, each face (or surface) is converted into a separate
[M3D shape](https://gitlab.com/bztsrc/model3d/blob/master/docs/shape.md) and groupped by its shell bone. Shapes are converted
using the following geometry entities:

| STEP Entity           | Description                     |
|-----------------------|---------------------------------|
| `PLANE`               | plane placement                 |
| `CYLINDRICAL_SURFACE` | cylindrical placement           |
| `SHERICAL_SURFACE`    | sherical placement              |
| `EDGE_LOOP`           | edge loop                       |
| `POLY_LOOP`           | polygon loop                    |
| `ORIENTED_EDGE`       | one edge                        |
| `EDGE_CURVE`          | a curve to that edge            |
| `VERTEX_POINT`        | a point in 3D space             |
| `LINE`                | a line                          |
| `CIRCLE`              | a circle                        |

How these are converted into shape commands is complicated. For example lines on a plane are converted into a single polygon
command; circles on a plane are converted to circle commands, but two circles on a cylindrical surface are converted to a single
cylinder command, etc.
