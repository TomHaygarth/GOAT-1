# GOAT Engine

Game On A Train engine is a cross platform framework for building games and an asset editor. The plan is to have a few different rendering backends supported as well as a few different platforms.

## Current Platforms
* Windows 10
* Mac OS 10.13+

Currently supported Renderer
* opengl

## Prerequisites
* CMake 3.5+
* GLFW 3.3
* Vulkan SDK (if using vulkan renderer)
* Dear ImGUI (docking branch)
* glad (if using opengl renderer)
* glm
* SPIRV-Cross (if using opengl renderer)
* sokol (for audio)

## Building
Make a "build" folder inside the root of the project and inside it run the cmake generator
e.g. `cmake -GXcode ..`
replacing `Xcode` with your platforms build tool.
By default this will generate a project for your current platform (currently only windows and macOS are supported. Linux is available but currently untested). This will also use the default renderer (which is currently vulkan and it's unfinished.

To change the platform you can supply the `-DPlatform=` argument
Current valid options are:
* macos
* windows
* linux

To change the default renderer you can supply the `-DRenderer=` argument
Current valid options are
* vulkan
* opengl
* dx12

e.g. `cmake -GXcode .. -DPlatform=macos -DRenderer=opengl`

## Roadmap
Current roadmap is roughly organised in order of priority
* Basic opengl 3.3+ renderer with simple primitives
* Asset Loading/Management
* Support for Scene hierarchy
* Basic Audio Manager using Sokol for cross-platform playback
* Maybe some scripting support through LUA or Python
* Animation system
* UI System
* DirectX 12 Renderer
* UWP platform support
* Vulkan Renderer
