[![Linux build](https://github.com/bartekordek/GameEngine/actions/workflows/cmake.yml/badge.svg)](https://github.com/bartekordek/GameEngine/actions/workflows/cmake.yml)
[![Windows Build](https://github.com/bartekordek/GameEngine/actions/workflows/cmake-win.yml/badge.svg)](https://github.com/bartekordek/GameEngine/actions/workflows/cmake-win.yml)

# GameEngine

A custom C++ game engine project focused on learning, experimentation,
and building core real-time rendering and gameplay systems from scratch.


## Overview

This project is a personal game engine implementation. It is intended to
explore and implement fundamental systems commonly found in modern game
engines, such as:

-   Core application loop
-   Rendering pipeline abstraction
-   Scene / entity management
-   Input handling
-   Resource management
-   Basic physics integration (if applicable)
-   Platform layer abstraction

The project is structured in a modular way to allow iterative
development and experimentation without being locked into a specific
engine architecture.

## Dependencies

Vendored third-party code lives under `deps/` (SDL, freeglut, GLEW, imgui, rapidobj, CUL, …).

### Third-party warning policy

GameEngine keeps **full warnings** for first-party code under `gameengine/` (and samples/tools). Vendored code under `deps/` is treated as third-party: we silence its compile warnings in CMake instead of fixing upstream noise.

How it works (Windows + Linux):

1. **Compiling dependency sources** — After all `deps/` subdirectories are added, `ge_silence_warnings_for_deps_dir()` (see `cmake/GameEngineTargetHelpers.cmake`) walks every CMake target whose `SOURCE_DIR` is under `deps/` and applies:
   - MSVC: `/W0`
   - GCC / Clang: `-w`  
   On CMake ≥ 3.25 those targets are also marked `SYSTEM`.
2. **CUL exception** — Targets under `deps/CUL/libcul` and `deps/CUL/libculTests` are **not** silenced here, so CUL first-party warnings stay visible when building GameEngine. Nested CUL dependencies are already silenced by CUL’s own `cul_silence_warnings_for_deps_dir()` (see `deps/CUL/README.md`).
3. **Including dependency headers from our code** — `gameengine` uses `include_directories(SYSTEM …)` for GLEW/GLM/CUL/SDL/freeglut. On MSVC it also enables `/external:W0` and `/external:anglebrackets` so those headers do not spam `/Wall`.

First-party targets (`gameengine_static`, samples, cooker, …) are added **after** the deps silence pass.

## Build System

The project uses CMake as its primary build system.

### Typical build steps:
1. Configure
``` bash
git clone https://github.com/bartekordek/GameEngine
cd GameEngine
git submodule update --recursive --init
cmake -S . -B your_build_dir -DCMAKE_BUILD_TYPE=your_wanted_build_configuration
```
2. In case you using msvc/windows, there should be already solution in `your_build_dir`.
Otherwise, use cmake:

Build all:
``` bash
cmake --build your_build_dir --config your_wanted_build_configuration
```
Or if you want some specific sample/example:
``` bash
cmake --build your_build_dir --config your_wanted_build_configuration --target playgroundApp
```

## Goals

-   Understand low-level engine architecture
-   Build reusable engine components
-   Learn rendering and game loop design patterns
-   Experiment with performance and memory management techniques

## Notes

This project is actively evolving and may change structure frequently as
systems are refactored and replaced.
