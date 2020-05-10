cmake_minimum_required(VERSION 3.16)
project(OGAT-1 LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Platform properties
# --------------------------------------------------------------------------------
set (BUILD_FOR_DESKTOP OFF CACHE INTERNAL "" FORCE)
set (BUILD_FOR_MAC OFF CACHE INTERNAL "" FORCE)
set (BUILD_FOR_WINDOWS OFF CACHE INTERNAL "" FORCE)
set (BUILD_FOR_LINUX OFF CACHE INTERNAL "" FORCE)

if (APPLE)
    set (DEFAULT_PLATFORM "macos")
elseif (WIN32)
    set (DEFAULT_PLATFORM "windows")
else()
    set (DEFAULT_PLATFORM "linux")
endif()

set (Platform ${DEFAULT_PLATFORM} CACHE STRING "Platform to build for")

if (${Platform} STREQUAL "macos")

    set (BUILD_FOR_DESKTOP ON CACHE INTERNAL "" FORCE)
    set (BUILD_FOR_MAC ON CACHE INTERNAL "" FORCE)

elseif (${Platform} STREQUAL "windows")

    set (BUILD_FOR_DESKTOP ON CACHE INTERNAL "" FORCE)
    set (BUILD_FOR_WINDOWS ON CACHE INTERNAL "" FORCE)

elseif (${Platform} STREQUAL "linux")

    set (BUILD_FOR_DESKTOP ON CACHE INTERNAL "" FORCE)
    set (BUILD_FOR_LINUX ON CACHE INTERNAL "" FORCE)

endif()

# Options
# --------------------------------------------------------------------------------

option(BUILD_EDITOR "Build the editor for the project" ON)
option(BUILD_GAME "Build the game for the project" ON)

if (BUILD_EDITOR AND BUILD_FOR_DESKTOP)
	add_subdirectory(src/editor)
endif()

if (BUILD_GAME)
	add_subdirectory(src/game)
endif()