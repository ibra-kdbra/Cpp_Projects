# volumetric-clouds

A real-time GPU volumetric clouds renderer via OpenGL, based on <a href="https://advances.realtimerendering.com/s2015/The%20Real-time%20Volumetric%20Cloudscapes%20of%20Horizon%20-%20Zero%20Dawn%20-%20ARTR.pdf">The Real-time Volumetric Cloudscapes of Horizon: Zero Dawn</a> by Andrew Schneider.

<img src="./resources/images/showcase.png" alt="Collage of realtime render results from the volumetric clouds renderer" width="100%"></img></a>

## How can I run this?

*Only tested on Linux, Windows*

### Prerequisites

- **Qt 6**: In order to build and run this program, you will need to have Qt 6 installed on your machine. See <https://www.qt.io/product/qt6> for more information.
- **OpenGL with GLSL 4.3 or higher**: Required for GPU rendering and compute shaders/SSBO.

### Build & Run

Already included a `CMakeList.txt` file for building the program. If you already have Qt installed, you can build and run the program by following these steps:

- Launch Qt Creator and click "Open Project"
- Select and open `CMakeLists.txt` from the root directory of the project
- Go to Projects - Run Settings - Run, and change "Working directory" to the root directory of the project
- Click the "Run" button to build and start the program

## Key Features

- Organic clouds shape generation with 3D fractal Worley noise
- Dynamic clouds movement over time by offsetting noise textures
- Physically-based volume rendering with anistropic scattering to account for directional dependence of light scattering within the clouds
- Rendering compatibility with solid objects by compositing with depth and color buffers
- Rich customization options to craft your own clouds: control density, coverage, shape, and lighting of the clouds in the GUI
- Realistic sky that varies throughout a day by modeling atmospheric scattering

## Optimizations

- Optimized raymarching with adaptive step sizes and backtracking
- GL Compute Shader to efficiently generate 3D Worley noise textures
