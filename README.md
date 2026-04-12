# FreeRS42

## Overview
FreeRS42 is a modern C++ re-implementation of Ricochet Xtreme, originally developed by Reflexive Entertainment. Built on the NuVelocity engine, FreeRS42 aims for accuracy, performance, and maintainability, recreating the classic brick-breaking gameplay with modern code and tools.

## Features
- Ricochet Xtreme gameplay re-implementation
- Modular, type-safe C++ codebase
- Powered by NuVelocity engine
- Modern build system (CMake)
- Extensible for mods and enhancements

## Directory Structure
- `engine/` — NuVelocity engine source (see `engine/README.md`)
- `src/` — Game logic, objects, and scenes

## Build Instructions
1. Install dependencies (see `engine/README.md`)
2. Clone repository and submodules
3. Configure with CMake:
  ```sh
  cmake -S . -B build
  ```
4. Build:
  ```sh
  cmake --build build
  ```

## License
Main code and engine are licensed under the Mozilla Public License 2.0 (MPL 2.0).
For third-party code, see the `third_party` directory for license information.

## Contact
For questions or contributions, open an issue or pull request.
