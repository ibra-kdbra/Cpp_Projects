Mathematical Shapes in Model 3D Files
=====================================

These are stored in "SHPE" ([M3D binary](https://gitlab.com/bztsrc/model3d/blob/master/docs/m3d_format.md)) and "Shape"
([A3D ASCII](https://gitlab.com/bztsrc/model3d/blob/master/docs/a3d_format.md)) chunks. The binary format uses the `Id`,
and encodes the arguments according to the specified format. The ASCII variant uses the `Keyword` column, and encodes
material as string, vc_t as a floating point number, and everything else as an integer.

Shapes are mainly used with [CAD models](https://gitlab.com/bztsrc/model3d/blob/master/docs/CAD.md) and provide an alternative
to polygon meshes. They do not use skin groups, each shape explicitly belongs to one bone only.

[[_TOC_]]

Command Arguments
-----------------

The commands can have any combinations of the following arguments (with one exception, `m3dcp_mi_t` not allowed in variadic
argument lists):

| Argument   | Description           |
|------------|-----------------------|
| m3dcp_mi_t | material index        |
| m3dcp_hi_t | shape index           |
| m3dcp_fi_t | mesh face index       |
| m3dcp_ti_t | texture map index     |
| m3dcp_vi_t | vertex index          |
| m3dcp_vc_t | floating point scalar |
| m3dcp_i1_t | int8 scalar           |
| m3dcp_i2_t | int16 scalar          |
| m3dcp_i4_t | int32 scalar          |
| m3dcp_va_t | variadic arguments    |

The available commands are defined in a single static array in [m3d.h](https://gitlab.com/bztsrc/model3d/blob/master/m3d.h),
called `m3d_commandtypes[]`. If you add to this array, no need to change the M3D SDK, the importer and exporter will support
the new command instantly. Be careful, if more commands' keywords start with the same string, the longer one must come first.

Variadic argument `m3dcp_va_t` is special, it can occour only once in an argument list, and it can't be the last. It encodes
an integer number, the number of how many times the remaining arguments are repeated. For example
```
m3dcp_fi_t, m3dcp_va_t, m3dcp_vi_t = means the command has a face index argument, then n times a vertex index.
10, 3, 20, 21, 23
m3dcp_va_t, m3dcp_fi_t, m3dcp_vi_t = means the command has n times a face index and vertex index pair.
3, 10, 20, 11, 21, 12, 22
```

Technical Commands
------------------

These do not define a shape per se, they are rather references to other chunks.

| Id  | Keyword    | Arguments                | Description                                           |
|----:|------------|--------------------------|-------------------------------------------------------|
|   0 | "use"      | mi_t                     | use material                                          |
|   1 | "inc"      | hi_t vi_t qi_t vi_t      | include a shape at position with rotation and scaling |
|   2 | "mesh"     | fi_t fi_t vi_t qi_t vi_t | include part of the polygon mesh                      |

If the two fi_ts in `mesh` are 0 and -1, then the entire polygon mesh is included, otherwise only the vertices selected. First
is the start face id, the second is the number of polygons to be included. The first vi_t selects a position vertex, the
second a scaling vector (encoded as a vertex). The qi_t also select a vertex, which must be interpreted as a quaternion.

Approximation and Modifier Parameters
-------------------------------------

Curve and Surface commands (see below) may have parameter commands. Those are either approximation parameters (one can be used per
curve / surface), or modifier parameters (more allowed per curve / surface). These parameter commands must preceed the curve /
surface command, and they only apply to the first curve / surface command that follows them.

| Id  | Keyword    | Arguments           | Description                                           |
|----:|------------|---------------------|-------------------------------------------------------|
|   3 | "div"      | vc_t                | specifies a constant subdivision for both u and v     |
|   4 | "sub"      | vc_t vc_t           | specifies constant parametric subdivision for u, v    |
|   5 | "len"      | vc_t                | specifies a constant spatial subdivision              |
|   6 | "dist"     | vc_t vc_t           | specifies a maximum distance and maximum angle        |

With Wavefront OBJ equivalents, "div (res)" is the same as `ctech cparm (res)` and `stech cparmb (uvres)`, while
"sub (ures) (vres)" equals to `stech cparma (ures) (vres)`, "len (maxlen)" is the same as `ctech cspace (maxlen)` and
`stech cspace (maxlen)`, finally "dist (maxdist) (maxangle)" equals to `ctech curv (maxdist) (maxangle)` and `stech
curv (maxdist) (maxangle)`. M3D does not use different commands for "ctech" and "stech", it is simply a matter of
which command's subcommands they are.

| Id  | Keyword    | Arguments           | Description                                           |
|----:|------------|---------------------|-------------------------------------------------------|
|   7 | "degu"     | i1_t                | degree for u (default to 1)                           |
|   8 | "deg"      | i1_t i1_t           | degree for both u, v (defaults to 1, 1)               |
|   9 | "rangeu"   | ti_t                | range for u (defaults to 0.0..1.0)                    |
|  10 | "range"    | ti_t ti_t           | range for both u, v (defaults to 0.0..1.0, 0.0..1.0)  |
|  11 | "paru"     | va_t vc_t           | specify knot vectors for u                            |
|  12 | "parv"     | va_t vc_t           | specify knot vectors for v                            |
|  13 | "trim"     | va_t ti_t i2_t      | specify outer trimming curve                          |
|  14 | "hole"     | va_t ti_t i2_t      | specify inner trimming curve                          |
|  15 | "scrv"     | va_t ti_t i2_t      | specify a single spacial curve                        |
|  16 | "sp"       | va_t vi_t           | specify special points which must be on the surface   |

For the last three, i2_t indexes a previous curve command in this shape, and ti_t selects two coordinates from tmap
which must be interpreted as u = u0, v = u1. Wavefront OBJ equivalents are pretty much the same, but there `deg` is
allowed to have one parameter only, and "range" is included in `curv` and `surf` commands as first arguments. The "paru"
is called `parm u`, and "parv" is `parm v` in OBJ.

Curve Commands
--------------

Curve commands do not define any shape, they are only helpers to define trimming in surfaces. Only surfaces have visual
representation. Curve / Surface commands are locally numbered within a shape, and referenced by modifier parameters.

| Id  | Keyword    | Arguments           | Description                          |
|----:|------------|---------------------|--------------------------------------|
|  17 | "bez1"     | va_t vi_t           | bezier1d(\[control points])          |
|  18 | "bsp1"     | va_t vi_t           | bspline1d(\[control points])         |
|  19 | "bez2"     | va_t vi_t           | bezier2d(\[control points])          |
|  20 | "bsp2"     | va_t vi_t           | bspline2d(\[control points])         |

Wavefront OBJ equivalents are a bit more complicated,
```
cstype rat {bezier|bspline}
deg n
curv u0, u1, [control points]
[modifiers]
end
```
and
```
cstype rat {bezier|bspline}
deg n m
curv2 [control points]
[modifiers]
end
```

Surface Commands
----------------

There are two kinds of surfaces in M3D: rational Bezier surface (bezier3d) and NURBS is a non-uniform rational B-spline surface
(bspline3d). M3D does not store non-rational shapes, if you need one, use vertices with the same w component.

| Id  | Keyword    | Arguments             | Description                             |
|----:|------------|-----------------------|-----------------------------------------|
|  21 | "bezun"    | va_t vi_t ti_t vi_t   | bez_uv_normal(\[control, UV, normal])   |
|  22 | "bezu"     | va_t vi_t ti_t        | bez_uv(\[control, UV])                  |
|  23 | "bezn"     | va_t vi_t vi_t        | bez_normal(\[control, normal])          |
|  24 | "bez"      | va_t vi_t             | bez(\[control points])                  |
|  25 | "nurbsun"  | va_t vi_t ti_t vi_t   | nurbs_uv_normal(\[control, UV, normal]) |
|  26 | "nurbsu"   | va_t vi_t ti_t        | nurbs_uv(\[control, UV])                |
|  27 | "nurbsn"   | va_t vi_t vi_t        | nurbs_normal(\[control, normal])        |
|  28 | "nurbs"    | va_t vi_t             | nurbs(\[control points])                |
|  29 | "conn"     | i2_t ti_t i2_t i2_t ti_t i2_t | connect(surf1, range1, curve1, surf2, range2, curve2) |

Wavefront OBJ equivalents are similar to curves, but uses `surf u0, u1, v0, v1, [control points]` in `cstype ... end` blocks.
The command that connects surfaces is called `con` in OBJ, otherwise identical to M3D's.

Geometrical Shape Commands
--------------------------

| Id  | Keyword    | Arguments                     | Description                                           |
|----:|------------|-------------------------------|-------------------------------------------------------|
|  30 | "line"     | va_t vi_t                     | polyline(\[points])                                   |
|  31 | "polygon"  | va_t vi_t                     | polygon(\[points])                                    |
|  32 | "circle"   | vi_t qi_t vc_t                | circle(pos, ori, radius)                              |
|  33 | "cylinder" | vi_t qi_t vc_t vi_t qi_t vc_t | cylinder(pos1, ori1, radius1, pos2, ori2, radius2)    |
|  34 | "shpere"   | vi_t vc_t                     | shpere(pos, radius)                                   |
|  35 | "torus"    | vi_t qi_t vc_t vc_t           | torus(pos, ori, radius1, radius2)                     |
|  36 | "cone"     | vi_t vi_t vi_t                | cone(pos1, pos2, pos3)                                |
|  37 | "cube"     | vi_t vi_t vi_t                | cube(pos1, pos2, pos3) (the bottom 3 points in CCW)   |
| ... | ...        | ...                           | TODO: define more shapes                              |

These define basic, but complex geometric shapes. There are no equivalents in OBJ, except for polyline, `l`. Geometrical shape
commands also define a local curve / surface index.

Examples
--------

For simplicity, examples are given in [A3D](https://gitlab.com/bztsrc/model3d/blob/master/docs/a3d_format.md), which is an
identical representation of the in-memory format. For clearity, these are the same examples as in the
[OBJ specification](obj_spec.md). Variadic arguments in A3D may be confusing at first, you have to explicitly tell the number
of arguments, that's because of the identical mapping with in-memory and binary formats.

#### Bezier Curve Example

Same as OBJ Spec B1 - 28. Note that in M3D those vertex coordinates MUST be normalized, they are not in this example to make it
easier to match with the OBJ version. If you want to save M3D with exactly these coordinates, then add `-C` flag to m3dconv.
M3D also does not store non-rational curves, use the same w component to achieve that effect.
```
Vertex
-2.3 1.95 0.0 1.0
-2.2 0.79 0.0 1.0
-2.34 -1.51 0.0 1.0
-1.53 -1.49 0.0 1.0
-0.72 -1.47 0.0 1.0
-0.78 0.23 0.0 1.0
0.07 0.25 0.0 1.0
0.92 0.27 0.0 1.0
0.80 -1.61 0.0 1.0
1.62 -1.59 0.0 1.0
2.44 -1.57 0.0 1.0
2.69 0.67 0.0 1.0
2.9 1.98 0.0 1.0

Textmap
0.0 4.0

Shape myCurve1
div 1.000
degu 3
paru 5[ 0.0 1.0 2.0 3.0 4.0 ]
range 0 0
bez1 13[ 0 1 2 3 4 5 6 7 8 9 10 11 12 ]

```
Using `[` and `]` is just a syntactic sugar to separate variadic arguments. The number of arguments is not necessarily the number
of elements in the line. Here they are the same only because one element is defined in the variadic argument item for `bez1`.

#### Texture Mapped Rational B-spline Surface (NURBS) Example

Same as OBJ Spec B1 - 31, also coordinates are not normalized to simplify the example, but they should be.
```
Vertex
-1.3 -1.0 0.0 1.0
0.1 -1.0 0.4 7.6
1.4 -1.0 0.0 2.3
-1.4 0.0 0.2 1.0
0.1 0.0 0.9 0.5
1.3 0.0 0.4 1.5
-1.4 1.0 0.0 2.3
0.1 1.0 0.3 6.1
1.1 1.0 0.0 3.3

Textmap
0.0 0.0
0.5 0.0
1.0 0.0
0.0 0.5
0.5 0.5
1.0 0.5
0.0 1.0
0.5 1.0
1.0 1.0

Shape myNurbs
use SkinMaterial
deg 2 2
paru 6[ 0.0 0.0 0.0 1.0 1.0 1.0 ]
parv 6[ 0.0 0.0 0.0 1.0 1.0 1.0 ]
range 6 6
nurbsu 9[ 0 0 1 1 2 2 3 3 4 4 5 5 6 6 7 7 8 8 ]

```
Here we have two elements per variadic item in `nurbsu`, so variadic argument encodes half the number of elements in the line.

#### Trimmed NURBS Example

Same as OBJ Spec B1 - 32, coordinates should be normalized but they are not as with the other examples.
```
Vertex
-0.625 1.850 3.0 1.0
0.915 1.193 0.0 1.0
2.485 0.470 2.0 1.0
2.485 -1.03 0.0 1.0
1.605 -1.89 10.7 1.0
-0.745 -0.645 0.5 1.0
-1.35 -1.03 0.0 1.0
0.13 -1.03 0.432 7.6
1.48 -1.03 0 2.3 1.0
-1.46 0.06 0.201 1.0
0.120 0.06 0.915 0.5
1.38 0.06 0.454 1.5
-1.48 1.03 0.0 2.3
0.12 1.03 0.394 6.1
1.17 1.03 0.0 3.3

Textmap
-1.0 2.5
-2.0 2.0
0.0 2.0

Shape myTrimmedNurbs
degu 3
paru 3[ 0.0 1.0 2.0 ]
bez2 7[ 0 1 2 3 4 5 0 ]
deg 2 2
paru 6[ -1.0 -1.0 -1.0 2.5 2.5 2.5 ]
parv 6[ -2.0 -2.0 -2.0 -2.0 -2.0 -2.0 ]
range 0 1
trim 1[ 2 0 ]
nurbs 9[ 6 7 8 9 10 11 12 13 14 ]

```
Here trim has one pair, selects the range 0.0..2.0 (from tmap idx 2) and the first curve from the shape (i2 0). The shape defines
two curve / surface commands, therefore local index goes 0 (bez2) to 1 (nurbs), and there's no separate numbering for curves and
surfaces like with OBJ.
