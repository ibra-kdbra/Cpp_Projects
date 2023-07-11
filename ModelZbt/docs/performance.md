Model 3D SDK Performance Tests
==============================

For the tests I've used the [Wuson](https://github.com/assimp/assimp/raw/master/test/models/OBJ/WusonOBJ.obj) model from Assimp's
repository (Blitz version can be found in this repo's models directory too). For Wavefront OBJ, I've checked Assimp and
[tinyobjloader](https://github.com/syoyo/tinyobjloader). I repeatedly loaded and decoded the models 32 times to mitigate the disk
read overhead, and I've also run the test before the perfomance tests, so that the executable and the model file should be already
in the Linux disk cache.

For the time measurement, I've used the standard Linux `time` tool, and took the real time, which includes both time spent in
user space and the time spent serving kernel system calls.

NOTE: the uncompressed M3D format, which did not use deflate was actually smaller for this particular model. This mainly
depends on how effective can zlib create the Huffman trees from the M3D chunks.

| Model          | Format           | File Size | Library       | Required Time   |
|----------------|------------------|----------:|---------------|-----------------|
| WusonOBJ.obj   | text OBJ         |      258k | tinyobjloader | `real 0m0.748s` |
| WusonOBJ.obj   | text OBJ         |      258k | libassimp     | `real 0m2.716s` |
| WusonBlitz.b3d | binary Blitz     |       87k | libassimp     | `real 0m0.414s` |
| WusonBlitz.m3d | uncompressed M3D |       33k | libassimp     | `real 0m6.906s` |
| WusonBlitz.a3d | ASCII Model 3D   |      141k | M3D SDK       | `real 0m0.258s` |
| WusonBlitz.m3d | uncompressed M3D |       33k | M3D SDK       | `real 0m0.073s` |
| WusonCompr.m3d | deflated M3D     |       42k | M3D SDK       | `real 0m0.162s` |

Conclusion
----------

The Assimp library was the slowest, except for loading the binary Blitz model in which case it was faster than the tinyobjloader
which had to parse text. In all cases the fastest was the native M3D SDK, about __10 times faster__ than the second place
tinyobjloader. Even with zlib deflate, it was still __4 times faster__. To make it a fair compartition, comparing only the
ASCII variant with tinyobjloader, the M3D SDK is still __2.5 times faster__ (and it was faster than the binary libassimp reader).

