# Anthology Game Framework

**Anthology** is a modular and modern 3D Game Engine.

## Features

- **Rendering**
  - Deferred renderer with physically based rendering (PBR)
  - Cascaded Shadow Maps (CSM)
  - Point Light Shadows
  - Screen-Space Ambient Occlusion (SSAO)
  - HDR, Tone Mapping - ACES, ASX
- **Audio**
  - Integrated with [miniaudio](https://github.com/mackron/miniaudio)
- **Physics (Planned)**
  - [Jolt Physics](https://github.com/jrouwe/JoltPhysics) integration
- **Asset Pipeline**
  - External asset converter using:
    - [Assimp](https://github.com/assimp/assimp)
    - [tinygltf](https://github.com/syoyo/tinygltf)
- **Dependencies**
  - Input and windowing via [GLFW](https://www.glfw.org/)
  - GL function loading via [GLAD](https://github.com/Dav1dde/glad)
  - Math with [GLM](https://github.com/g-truc/glm)
  - Text rendering with [FreeType](https://freetype.org/)
  - Experimental networking via [GameNetworkingSockets](https://github.com/ValveSoftware/GameNetworkingSockets)

---

## 🛠️ Build and Installation

> **Note:** Anthology uses CMake (version X.X or higher). Tested on Windows / Linux / macOS.

### Prerequisites

Make sure you have the following installed:

- C++ compiler (C++17 or later)
- CMake
- Git
- Dependencies listed below (some fetched automatically via submodules)

Linux
- OpenGL development devel : libglvnd-devel
- X11 sdk : libX11-devel libXrandr-devel libXi-devel libXcursor-devel libXinerama-devel
- Installed nvidia drivers for gpu double check for opengl support (apparently not supported)
- TODO: Check if it works with wayland (for now assumes and only works for x11)

### Building

```bash
git clone --recurse-submodules https://github.com/Hamad-AM/Gameframework.git
cd gameframework
mkdir build
cd build
cmake ..
cmake --build .
