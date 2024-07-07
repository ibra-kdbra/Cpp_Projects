# Chess_3D

A simple 3d chess game.

- U to undo last move
- R to restart game
- C to reset camera angle
- F to toggle fullscreen

## ScreenShot

![Chess_3D_screenshot](./images/screenshot.png)

## Building

```sh
mkdir build &&
cd build &&
cmake -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Release .. &&
make
```

GLFW and GLM libraries must be available on your system. They can be easily installed using vcpkg. Then, add the
following line to your `cmake` command:

`-DCMAKE_TOOLCHAIN_FILE=$VCPKG_DIR/scripts/buildsystems/vcpkg.cmake`

## Resources

- [Learn OpenGL](https://learnopengl.com)
- [Chess Programming Wiki](https://www.chessprogramming.org)
- Board and piece models are taken from [here](https://polyhaven.com/a/chess_set) (CC0).x
