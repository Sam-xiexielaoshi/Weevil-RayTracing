# Weevil RayTracing

Weevil RayTracing is a small C++ ray tracing playground built on top of [Walnut](https://github.com/TheCherno/Walnut).  
The current renderer draws a shaded sphere in an ImGui viewport and demonstrates core ray tracing steps such as ray generation, sphere intersection, normal calculation, and Lambert lighting.

## Project Structure

- `/Weevil/src/WalnutApp.cpp` - application entry/layer and ImGui UI
- `/Weevil/src/Renderer.h` - renderer interface
- `/Weevil/src/Renderer.cpp` - per-pixel ray tracing logic
- `/premake5.lua` - workspace definition
- `/Weevil/premake5.lua` - project build settings
- `/scripts/Setup.bat` - generates Visual Studio 2022 project files

## Requirements

- Windows
- Visual Studio 2022 (C++ workload)
- Vulkan SDK (required by Walnut)
- Git (with submodule support)

## Getting Started

1. Clone the repository with submodules:
   ```bash
   git clone --recurse-submodules https://github.com/Sam-xiexielaoshi/Weevil-RayTracing.git
   ```
2. Run setup script:
   - `scripts/Setup.bat`
3. Open the generated Visual Studio 2022 solution.
4. Build and run the `Weevil` project.

## Current Features

- Real-time viewport rendering in ImGui
- Per-pixel ray generation
- Analytic sphere intersection
- Diffuse (Lambert) shading

## Notes

- This project currently uses a simple single-object scene and is intended as a learning/iteration base for more advanced ray tracing features.