# Template for the Future Game (IS NOT READY! IN PROGRESS!)

The project is using [Victor Stone's](https://github.com/SandSnip3r) pathfinder [library](https://github.com/SandSnip3r/Pathfinder). 

### Dependencies

Main SDL2 only and TBB for multithreading.
A compiler with C++20 support is required.
You can install dependencies with these commands (Debian/Ubuntu):
```bash
sudo apt install libsdl2-dev cmake g++ libtbb-dev
```
### Usage

Any key to create an object (will be removed) <br/>
Dragging the object (will be removed) <br/>
Selecting the area, multi objects <br/>
Select single object <br/>
Move object (Mouse right click)
 


### Building


The code can be built using cmake (tested on Linux)

To clone Pathfinder implementation 
```bash
cd include && git clone https://github.com/SandSnip3r/Pathfinder.git && cd ..
```
Build and run
```bash
mkdir build && cd build
cmake ..
make
./game
```
### TODO
1. Performance (flood fill optimization, available map for object )
2. Need to Use OpenCV !?
3. Complete moving logic
4. Remove dragging option
5. Add Base-object that produse new objects (instead of creating new object by pressing the key)
6. More and more

### Example

![Alt text](examples/sample.gif?raw=true "Title")
