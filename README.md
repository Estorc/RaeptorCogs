# RaeptorCogs

<!-- markdownlint-disable MD033 -->
<p align="center">
    <img src="raeptor-cogs-logo.png" alt="RaeptorCogs logo" style="width: 60%" />
    <br><br>
    Simple cross-platform graphic framework for C++ using OpenGL.
    <br><br>
</p>
<!-- markdownlint-disable MD033 -->

## The cross-platform C++ graphics framework of “freedom”

**RaeptorCogs is a cross-platform C++ graphics framework using OpenGL and planned for supporting both WebGL and Vulkan**.
The philosophy behind RaeptorCogs is to provide a beginner-friendly API while allowing advanced users to customize and extend the framework as needed. Because I always found graphics programming and game development to be quite complex and too far from my design preferences, I wanted to create a framework that simplifies common tasks while still being powerful enough for more experienced developers. RaeptorCogs is about giving developers the tools they need without overwhelming them with unnecessary complexity and preserving a huge degree of flexibility.

In simple terms, RaeptorCogs is **flexibility, simplicity, modularity, and cross-platform support all in one** package made by game and graphics developers for game and graphics developers.

## Features

- Rendering Pipeline - Batched 2D sprite/text/shape rendering with OpenGL
- Node Hierarchy - Graphics relationships and transformations
- Resource Management - Cached loading of images, fonts, shaders
- Input System - Keyboard and mouse handling
- Asset Loading - Images, fonts, files
- Serialization - Object persistence
- Utilities - Time, random, threading, bit operations
- Window Management - Multi-window support with events
- Cross-Platform - Windows (stable), Web/Emscripten (experimental), Linux (planned)

## Roadmap

- Vulkan rendering backend
- Enhanced WebGL support
- RTBE development
- Advanced Linux platform support
- Built-in shader editor
- Optional audio module
- Optional physics module

## Building

### Requirements

- C++17 or newer
- CMake 3.16 or newer
- OpenGL-capable environment (desktop)
- Emscripten (for Web builds)

### Desktop build

```bash
git clone https://github.com/Estorc/RaeptorCogs.git
cd RaeptorCogs
mkdir build && cd build
cmake ..
cmake --build .
```

### Web build (Emscripten)

```bash
git clone https://github.com/yourusername/RaeptorCogs.git
cd RaeptorCogs
mkdir build_web && cd build_web
emcmake cmake ..
cmake --build .
```

CMake configuration will automatically build for Web if emscripten CMake is used.

## Documentation and demos

The official documentation for RaeptorCogs is hosted on Github Pages and can be found on the [GitHub Pages site](https://estorc.github.io/RaeptorCogs/). It is made with Sphinx and Doxygen and provides detailed information on the framework's architecture, classes, and functions with some tutorials. The documentation is in construction and will be expanded over time, help and contributions are welcome!

You can also find some demo applications in the [examples folder](https://github.com/Estorc/RaeptorCogs/blob/main/examples) of the repository, showcasing various features of RaeptorCogs.
Examples will be expanded over time.

## License

RaeptorCogs is licensed under the MIT License.
See the [**LICENSE**](https://github.com/Estorc/RaeptorCogs/blob/main/LICENSE.txt) file for details.
