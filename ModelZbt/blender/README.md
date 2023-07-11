Model 3D Blender Integration
============================

This Python script adds Model 3D support to [Blender](https://blender.org) 2.80 and above (last tested version 3.3 LTS).

Installation
------------

1. Download [io_scene_m3d.py](https://gitlab.com/bztsrc/model3d/blob/master/blender/io_scene_m3d.py)
2. Start Blender
3. Go to menu, "Edit" > "Preferences..." > "Add-ons" tab > "Install..." button
4. Browse and double click on the downloaded file
5. On the "Add-ons" tab, search for "m3d"
6. Click on "Enable Add-on."
7. Now in the menu, "File" > "Export" > "Model 3D (.m3d)" (and eventually "File" > "Import" > "Model 3D (.m3d)") should appear

Export
------

Features: vertex list (with colors), mesh data, normals, UVs, materials (with colors, textures, standard and PBR properties),
bone hierarchy (armature), skin (bone weights per vertex), inlined assets, actions (animations).

### Tips and Tricks

For the best material results, use PricipledBSDF surface. (There's a workaround to fetch the diffuse texture from others and
those are converted to this, but the blender API isn't perfect in converting material surfaces at least to say.)

Create exatly one Armature. Bones that are right beneath the Armature are stored with model-space coordinates. Bones under other
bones however are stored with relative positions to their parent, allowing a much compact export output. So you should create only
one root bone node under the Armature (eg.: "spine"), and create the entire skeleton structure under that single root bone node.

Don't use spaces ' ', slash '/' or backslash '\\' in bone names (everyting else is okay, like dot '.', hash '#' etc.). Create
exactly one vertex group per bone, and name it exactly as the bone. This group should reference all verticies that the bone
influences (the same vertex can appear in 8 different groups, but most engines only support 4 groups per vertex tops).

Models can use any arbitrary coordinate units. To counteract this discrepancy, the model is scaled to fit in, and fill in entirely
the -1.0..1.0 bounding cube, and you have to tell the size of the bounding cube in SI meters using the **"Scale"** option. If you
set it to 0.00, then the exporter calculates it to a value so that the original coordinates will be restored on decoding. If you
uncheck the **"Use gridcompression"** option, then the model file will be much bigger, but all coordinates will be saved as-is
without any conversion (this will also set the precision to at least 32 bit, floating points).

By default, each action is saved as a separate animation. If your model does not have actions, then one big animation is exported
instead. To avoid this, you can either create actions, or add markers to the timeline (at positions where you want to split the
animation into actions; for example add a marker named "Idle" at 0, "Walk" at 10, "Jump" at 50 etc.), and check the
**"Use Markers"** option when exporting.

The plugin is written in a way that it first collects data from bpy into lists. Then the second half serializes those lists into
M3D chunks. To minimize memory consumption and to speed up the export, the lists contain indices to other lists mostly. They are
documented in the plugin's source. If the Blender API changes (again), then only the list filler part needs to be changed, and
not the M3D chunk encoder part.

Import
------

`WARNING: this part of the plugin is Work In Progress`

Features: it can read a M3D file, inflates it, parses the chunks in it, but just prints the chunks, does not add data to bpy
structures for real yet.

Missing: almost everything.

