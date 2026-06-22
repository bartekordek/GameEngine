[![Linux build](https://github.com/bartekordek/GameEngine/actions/workflows/cmake.yml/badge.svg)](https://github.com/bartekordek/GameEngine/actions/workflows/cmake.yml)
[![Windows Build](https://github.com/bartekordek/GameEngine/actions/workflows/cmake-windows.yml/badge.svg)](https://github.com/bartekordek/GameEngine/actions/workflows/cmake-windows.yml)

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
