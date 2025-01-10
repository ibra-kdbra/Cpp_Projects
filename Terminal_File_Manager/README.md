# tfm

A terminal file manager implemented in C++20 using the [ncurses](https://www.gnu.org/software/ncurses/) library.

&emsp; **Screenshot:**

&emsp; <img alt="preview" src="./screenshot/1.png" width="600"> 

*The screenshot above demonstrates how the terminal file manager (TFM) operates within directories. It displays a preview of a file on the right side, and if you select a directory, it shows the files contained within that directory and so on.*

## Dependencies

- [cmake](https://cmake.org)
- [ncurses](https://invisible-island.net/ncurses)

## Build

Run this from the root directory of the project.

```bash
mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Release .. && cmake --build .
```
