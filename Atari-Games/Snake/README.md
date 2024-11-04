# SFML-snake 

A classic snake game made in C++ using sfml library.


## Screenshots
![Screenshot](Screenshots/5.png?raw=true "Sample Main Menu")
![Screenshot](Screenshots/2.png?raw=true "Sample gameplay")

## Requirements

#### Must Have (dependencies)
- A valid C++ compiler - clang, gcc, msvc that supports c++20
- cmake >= v3.16
  
## Building

The CMAKE is set up to automatically download the dependencies and link against it. It will be done when configuring cmake.

```bash
make build
```

### Manual build steps using cmake

- Create a directory for cmake , e.g. - `mkdir build`
- `cd build`
- `cmake .. -G Ninja` or `cmake ..` (to use the default build system)
- `ninja snake` or `make snake` (depending on your selected build system)
