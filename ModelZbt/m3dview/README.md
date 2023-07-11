Model 3D Viewer
===============

A very very simple viewer for [Model 3D](https://gitlab.com/bztsrc/model3d/blob/master/docs/m3d_format.md) files.

Main purpose: quickly check a model. It does not support all features and material properties of a M3D file, only vertex colors
and basic properties and textures in materials. But it can load complex models, uses lights (so you can check normals), and
shows action animations frame timestamp correctly.

```
Model 3D Viewer by bzt Copyright (C) 2019 MIT license

./m3dview <m3d file>
```

## Compilation

Just run `make`, it will autodetect if you have GLFW, GLUT or SDL2 installed, and uses the appropriate library. GLFW is
preferred and recommended as that's the simplest and most portable. The platform independent part is in `viewer.c`, the other
`.c` files are the framework drivers. To override the autodetected one, use `DRIVER="MINGW_GLUT" make` for example.

### Under Windows

You'll need a couple of tools, here's a step-by-step how to:
1. install [MinGW](https://osdn.net/projects/mingw/releases), this will give you gcc under Windows
2. download [SDL2-devel-X-mingw.tar.gz](http://libsdl.org/download-2.0.php) under the section Development Libraries
3. extract SDL2 into a directory under MinGW's home directory
4. open Makefile in Notepad, and edit MINGWSDL to the path where you've extracted the tarball, add the last SDL2-X part too
5. copy $(MINGWSDL)/i686-w64-mingw32/bin/SDL2.dll into C:\Windows
6. run `make`

Possible issues: with some configuration, gcc was unable to find gl.h. To solve this, you have to copy the C:\MinGW\include\GL
folder into msys\1.0\include (whereever your msys directory is). You should also have C:\Windows\System32\opengl32.dll, but
that's all.

On Windows, if command line argument is not specified, the viewer will fire up an Open File dialog.

## Controls

| Keybinding                                                     | Description                                        |
|----------------------------------------------------------------|----------------------------------------------------|
| mouse left                                                     | drag the model, move the mouse to rotate           |
| mouse middle                                                   | zoom in                                            |
| mouse right                                                    | zoom out                                           |
| mouse scroll                                                   | zoom in / out                                      |
| <kbd>up</kbd>/<kbd>down</kbd>/<kbd>left</kbd>/<kbd>right</kbd> | rotate the model                                   |
| <kbd>page up</kbd>/<kbd>page down</kbd>                        | change action to animate                           |
| <kbd>0</kbd> - <kbd>9</kbd>                                    | change playback FPS (should not needed, for debug) |
| <kbd>,</kbd> / <kbd>.</kbd>                                    | previous / next frame (without time interpolation) |
| <kbd>space</kbd>                                               | toggle continous playback (with interpolation)     |
| <kbd>m</kbd>                                                   | toggle drawing mesh                                |
| <kbd>s</kbd>                                                   | toggle drawing skeleton                            |
| <kbd>Esc</kbd>, <kbd>q</kbd>                                   | quit                                               |

Note that there's an explicit "action animation" for the bind-pose. By pressing Page Down, you can always select it and see
the pure model without any animations.

