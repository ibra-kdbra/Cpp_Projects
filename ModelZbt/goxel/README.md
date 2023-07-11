Model 3D Goxel Integration
==========================

This is a standalone (non-SDK) implementation for [Goxel](https://github.com/guillaumechereau/goxel) to import and export
voxel images in [Model 3D format](https://gitlab.com/bztsrc/model3d/blob/master/docs/voxel_format.md).

Installation
------------

Download the Goxel repository, and just add this single file to `src/formats`, then compile by running `scons`. No modifications
required.

Limitations
-----------

Since this is a non-SDK, standalone implementation, it does not support all M3D features. Does not support triangle
meshes and can't inline textures for example, and it can only save full cubic voxels. This is reasonable since Goxel itself
have no support for these. Otherwise supports layers, materials, etc., everything you need in a fully featured voxel editor.

When compiled with the `M3D_SAVE_GOXSPEC` define, then it also saves some private Goxel specific chunks, like cameras, additional
meta layer info, etc., all the things that are saved in a .gox file, just does that more efficiently. By changing the
`M3D_SAVE_PREVIEW` define at the beginning of the file, it will also save 2D preview chunks into the voxel images.
