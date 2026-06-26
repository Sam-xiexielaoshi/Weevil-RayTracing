# Weevil RayTracing

Weevil RayTracing is a custom C++ ray tracing playground built on top of [Walnut](https://github.com/TheCherno/Walnut), Studio Cherno's C++ desktop application framework. It is being developed as a separate component from Beetle Engine and is intended to provide custom ray tracing options that can later be merged into the engine.

The project currently focuses on a simple ray tracing pipeline rendered inside an ImGui viewport. It demonstrates ray generation, analytic sphere intersection, normal calculation, and Lambert diffuse lighting.

## Repository Layout

- `/Weevil/src/WalnutApp.cpp` - application entry point and ImGui UI setup
- `/Weevil/src/Renderer.h` - renderer interface
- `/Weevil/src/Renderer.cpp` - per-pixel ray tracing implementation
- `/premake5.lua` - workspace definition
- `/Weevil/premake5.lua` - project build settings
- `/scripts/Setup.bat` - generates Visual Studio 2022 project files

## Requirements

- Windows
- Visual Studio 2022 with the C++ workload installed
- Vulkan SDK, required by Walnut
- Git with submodule support

## Getting Started

1. Clone the repository with submodules:
   ```bash
   git clone --recurse-submodules https://github.com/Sam-xiexielaoshi/Weevil-RayTracing.git
   ```
2. Run the setup script:
   - `scripts/Setup.bat`
3. Open the generated Visual Studio 2022 solution.
4. Build and run the `Weevil` project.

## Current Features

- Real-time viewport rendering in ImGui
- Per-pixel ray generation
- Analytic sphere intersection
- Surface normal calculation
- Lambert diffuse shading

## Notes

- The current scene is intentionally simple and contains a single shaded sphere.
- This repository is meant to serve as a learning and iteration base for more advanced ray tracing features.
- The project relies on Walnut and therefore requires the Vulkan SDK and submodules to be available before building.
