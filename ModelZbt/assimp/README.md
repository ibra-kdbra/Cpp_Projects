Model 3D Integration with Assimp
================================

**IMPORTANT NOTE**

This patch was included in Assimp, but had been criticised and removed by idiots (namely RichardTea and turol). I feel truly sorry
for kimkulling. Here's a brief summary:

1. I've submitted a PR which was working perfectly, compiled on all platforms with all compilers without a single warning, all valgrind and CI tests green: [PR Quality Review](https://app.codacy.com/app/kimkulling/assimp/pullRequest?prid=4513861), [appveyor](https://ci.appveyor.com/project/kimkulling/assimp/builds/29028527), [travis](https://travis-ci.org/assimp/assimp/builds/615149096?utm_source=github_status&utm_medium=notification)
2. Those said idiots claimed `thread_local` isn't a valid C++ keyword (it is), so I had to remove Assimp IOSystem integration, along with it the external asset support
3. The ASCII format temporarily sets LC_NUMERIC to C so that libc can read and write floats consistently. They said [this is an UB](https://github.com/assimp/assimp/issues/3910) (no, it isn't, actually this is the best practice and there's NO re-entrancy in Assimp anyway, I wonder if they know at all what re-entrant actually means), so I had to remove ASCII format support
4. They have made unnecessary and shady modifications with changed identation, so that git diff is useless and you cannot review easily (for example [here](https://github.com/assimp/assimp/pull/2805))
5. Then they fed it with invalid input, and they got extremely pissed because my code handled that very well, but *their* Assimp C++ class crashed :-D
6. Nonetheless, I've workarounded the calls to *their* buggy C++ code to avoid crashes, and after that passed all fuzzer tests green
7. They are [spreading lies](https://github.com/assimp/assimp/pull/3848) that my code has UBs and buffer overflow errors (no it does not, see the output of the various sanitizers, fuzzers and the automated valgrind testing in my repo too)
8. They went even that far to [put outright errors in my code](https://github.com/assimp/assimp/pull/3999) (you can't find the faulting part here in my repository, that was entirely RichardTea's doing)

As a consequence, **Assimp is officially not supported** any more. I will keep this directory, so you can use this patch if you want,
but the truth is, you're better off without Assimp. It is extremely slow (lots of unneccessary conversion needed with the insane
structures), eats up outrageous amount of RAM (see valgrind output below, for example Assimp allocates 1k for *every* *single*
string, no matter their size!), does about *thousand* times more (yeah, no mistake, 1000x more!) allocations than the M3D SDK, and
lot of the M3D features can't be supported anyway because of fundamental design flaws in Assimp (like mathematical shapes, voxel
images, procedural surfaces, automatic texture decoding etc. see "Differences in Assimp's and M3D's Philosophy" below for details).

**IMPORTANT NOTE ENDS**

The [M3D format](https://gitlab.com/bztsrc/model3d/blob/master/docs/m3d_format.md) is implemented in [Assimp](http://assimp.org).
No need to patch Assimp with these files, they are already included. This patch isn't needed for the **m3dconv** tool, that works
without it!

This documentation details the specifics how an [in-memory model](https://gitlab.com/bztsrc/model3d/blob/master/docs/usage.md) is
mapped into Assimp structures. The [SDK](https://gitlab.com/bztsrc/model3d/blob/master/m3d.h)'s read file callback was implemented
using Assimp's IOSystem, but because Assimp core developers are unfamiliar with the C++ thread_local keyword it had to be removed.

Static Model
------------

In the simpliest case, when you load a static model, there's only one node in the aiScene, which is the root node, and it has
all the meshes. Because Assimp requires one material per mesh, if the model doesn't use materials, there'll be only one mesh.

```
aiScene->mRootNode
 |          |->mName = model's name is set as the root node's name
 |          \->mMeshes[], indices all meshes
 |->aiMaterial mMaterials[] = m3d materials are mapped one-to-one, -1 as DefaultMaterial
 |              |->mName = material names are the same
 |              \->mProperties = converted using a static table in assimp/code/M3D/M3DMaterials.h
 |->aiTexture mTextures[] = m3d textures mapped one-to-one
 |              |->achFormatHint = either "rgba0800" (grayscale), "rgba0808" (ga), "rgba8880" (rgb) or "rgba8888" (rgba)
 |              \->mFilename = converted as m3d texture identifier + ".png"
 \->aiMesh mMeshes[] = all coordinates in model space
            |->mPrimitiveType = always aiPrimitiveType_TRIANGLE
            |->mMaterialIndex = m3d materialid -1 is mapped as 0 (DefaultMaterial), all the rest materialid + 1
            |->mFaces = m3d face converted into an aiFace list with triangles only
            |->mVertices = m3d vertex list converted into aiMesh local list
            |->mNormals = m3d normals likewise, aiMesh local list
            |->mTextureCoords[0] = m3d tmap UV coordinates are mapped locally into the first TextureCoords channel
            \->mColors[0] = m3d vertex colors are mapped into the first color map channel
```
(Note: aiScene.mMeshes is a list of aiMesh, while mMeshes in nodes are lists of array indices to the said aiScene.mMeshes)

Model With Skeleton(s)
----------------------

If there's a skeleton assossiated with the model, then it's loaded as further nodes under the root node. Those additional nodes
do not have meshes (only the root node like with static models).

```
aiScene->mRootNode
 |          |->mMeshes[], indices all meshes
 |          \->children
 |                \->aiNode skeleton root
 |                      |->mName = bone's name is the same
 |                      \->children
 |                          |->aiNode bone
 |                          | |->mName = name is the same
 |                          |  \->children
 |                          |       \->aiNode subbone
 |                          |           \->aiNode sub-subbone
 |                          |->aiNode bone
 |                          |   ...
 |                          \->aiNode bone
 \->aiMesh meshes[]
            \->aiBone, referencing mesh-less aiNodes from above (match by name)
```
Usually there's only one skeleton, however if more m3d bones has the parent of -1, they will be all loaded as separate
node sub-trees, each under the root node.

```
aiScene->mRootNode
 |          |->mMeshes[], indices all meshes
 |          \->children
 |            |->aiNode skeleton 1 root
 |            |     \->aiNode bone
 |            |   ...
 |            \->aiNode skeleton N root
 |                  \->aiNode bone
 \->aiMesh meshes[]
            \->aiBone, referencing mesh-less aiNodes from above (match by name)
```

Animated Models
---------------

Most formats store the entire skeleton for each frame, therefore Assimp aiAnimation tend to have all bones. M3D only stores
changed bones per frame. However you won't notice this, because each Assimp frame will contain the entire bone hierarchy's
aiVectorKeys and aiRotationKeys (aiScalingKeys is always empty as M3D encodes scaling in rotation quaternions). This requires
much much more memory, but it is needed to be compatible with other Assimp file format importers.
```
aiScene->mRootNode
 |          \->children
 |                \->aiNode skeleton, at least one bind-pose skeleton tree is mandatory
 |->mNumAnimations = as many animations as actions in m3d
 \->aiAnimation mAnimations[]
                    |->mName = m3d action name used as animation name
                    |->mDuration = duration of the animation in ticks
                    |->mTicksPerSecond = always 100
                    |->mNumChannels = as many channels as bones
                    \->mChannels[] = m3d action frames converted
                        |->mNodeName = points to the bone node in skeleton
                        |->mNumPositionKeys = mNumRotationKeys = m3d number of frames
                        |->mPositionKeys = m3d tranform position
                        |->mRotationKeys = m3d tranform rotation
                        \->mScalingKeys = always empty (scaling is encoded in the rotation quaternions)
```

CAD Models (with NURBS)
-----------------------

Not supported by Assimp, use the native M3D SDK if you need this feature.

Voxel Models
------------

Not supported by Assimp, however the M3D SDK post-process filter will convert voxel images into triangle meshes automatically.

Differences in Assimp's and M3D's Philosophy
--------------------------------------------

Loading Model 3D files using Assimp works perfectly, but it is not very efficient, because their data structures are conceptually
different. A lot of conversation has to be done, and if you want to pass the model data to a shader in a VBO, you'll have
to convert most of it from Assimp structures into something that essentially resembles what in-memory Model 3D originally had.

Because Assimp core developers are afraid to use libc, the ASCII variant support had to be removed. Assimp only supports
the binary format. There's no place for shape based models in aiScene, therefore you can only load mesh based models with Assimp,
CAD models and annotations are only available with the native Model 3D SDK. Likewise, procedural surfces and textures are only
supported by the native SDK, not by Assimp. Another difference is, that while the native SDK loads and decodes the textures for
you, Assimp only returns the texture filenames, you are on your own to load and decode them.

Assimp tries to mirror the various structures stored in various formats. On the other hand, Model 3D mandates exactly one
well-defined structure, and leaves the conversion to a command line tool. Therefore parsing an aiScene is a complex task,
while parsing m3d_t is simple. To support all those structures, aiNode is extremely flexible, has a lot more cross-references,
therefore it requires more RAM and it's memory is much more fragmented:
```
$ valgrind tests/test01 models/WusonBlitz0.m3d 2>&1 | grep still
==26242==    still reachable: 581,017 bytes in 4,088 blocks
$ valgrind tests/test02 models/WusonBlitz0.m3d 2>&1 | grep still
==26274==    still reachable: 229,534 bytes in 4 blocks
```

Not a full list, just a few examples to represent the complexity involved:
- M3D stores material id per face
- Assimp requires the same material id for all faces per mesh
- M3D stores a single, global vertex list, referenced by mesh face vertices and normals independently
- Assimp needs mesh local vertex and normal lists, referenced by mesh local faces simultaneously
- M3D stores bone id/weight pairs referenced from the global vertex list
- Assimp needs mesh local vertex index/weight pairs per bone lists per mesh
- M3D stores animations with a timestamp and list of changed bone id/position/orientation triplets per frame
- Assimp needs lists of position and orientation each with its own timestamp per bone per frame

Moreover, while in M3D you can access all data by simple indexing, Assimp only provides this for materials and meshes. For
the rest, the bones and animation channels, you'll have to recursively walk through a node tree, matching strings to find
what you're looking for. For the experts: M3D SDK is using O(1) algorithms, while Assimp uses O(n*m).

With Assimp, you can load many model formats in run-time, not just one. With Model 3D SDK, first you have to convert those into
.m3d format in compile-time using the [m3dconv](https://gitlab.com/bztsrc/model3d/tree/master/m3dconv) utility. In return, the SDK
is a single file, provides a well-defined structure and [your code that interfaces with that structure](https://gitlab.com/bztsrc/model3d/blob/master/docs/usage.md)
is lot more simpler.

