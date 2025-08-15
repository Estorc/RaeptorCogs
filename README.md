# RaeptorCogs

**RaeptorCogs** is a cross-platform C++ graphics library using OpenGL and WebGL.  
It is designed to provide a smooth workflow for both beginners and advanced developers building games or interactive applications.  
The long-term goal is to evolve from a rendering library into a lightweight framework or engine, with embedded tools for shaders and asset management.

Currently, RaeptorCogs supports batched 2D rendering of sprites and text on Windows and Web (via Emscripten). Linux support is under development.  
The build system is based on CMake and automatically detects whether to build for the Web or desktop.

---

## Features

- Cross-platform rendering: Windows and Web (Linux in progress)
- Batched sprite rendering
- Batched text rendering
- Beginner-friendly API with flexibility for advanced use
- CMake build system with automatic platform detection

Planned:
- Linux support
- Embedded shader editing tools
- Scene and asset management utilities
- Physics integration as an optional module

---

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

---

### Example of a sprite drawn at the center of a window
```cpp
#include <RaeptorCogs/RaeptorCogs.hpp>
#include <RaeptorCogs/Sprite.hpp>

constexpr int WINDOW_WIDTH = 1280;
constexpr int WINDOW_HEIGHT = 720;

RaeptorCogs::Window *main_window = nullptr;
RaeptorCogs::Sprite sprite;

void update() {
    RaeptorCogs::Renderer().render(main_window);
}

int main() {
    RaeptorCogs::Initialize();
    main_window = RaeptorCogs::Renderer().initialize(WINDOW_WIDTH, WINDOW_HEIGHT, "RaeptorCogs Demo");
    RaeptorCogs::Texture texture = RaeptorCogs::Texture("assets/textures/texture.png");
    sprite = RaeptorCogs::Sprite(texture).addToRenderer(RaeptorCogs::Renderer());
    sprite.setPosition(WINDOW_WIDTH/2, WINDOW_HEIGHT/2);
    RaeptorCogs::StartLoop(update, main_window);
    RaeptorCogs::Destroy();

    return 0;
}
```

---

### Roadmap
- Linux platform support
- Built-in shader editor
- Scene management
- Optional physics module

---

### License

RaeptorCogs is licensed under the MIT License.
See the LICENSE file for details.
