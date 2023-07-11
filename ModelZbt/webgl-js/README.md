Model 3D WebGL Javascript Polyfill
==================================

This code was written in pure Javascript (no jQuery or other bloated libraries), it is totally dependency free. It is not a fully
featured loader, it can load vertices, meshes and vertex colors (same features as the 80 SLoC example on the repository's main
page, plus generating smooth normals). It is technically a polyfill which allows you to use .m3d files in `IMG` tags on HTML pages.
When loaded, you can rotate the models by drag'n'drop. Should also work on mobile devices with touchscreens.

Note: this is a rape on Javascript. It was simply not designed to handle binary data efficiently, and WebGL itself is a hack as
it needs binary input.

Usage
-----

```
<img src="example_model.m3d" width="320" height="240">
<img src="another_model.m3d" width="320" height="240">

<script src="https://gitlab.com/bztsrc/model3d/raw/master/webgl-js/m3d.min.js"></script>
```
That's all. For inflating models, it uses [pako](http://nodeca.github.io/pako/) (already included in the minified version).
