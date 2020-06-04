cmake_minimum_required(VERSION 3.16)

set (EXTERNAL_SRC_DIR "${PPROJECT_SOURCE_DIR}/externals")

# GLFW include
# --------------------------------------------------------------------------------
if (BUILD_FOR_DESKTOP)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "")
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "")
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "")
    add_subdirectory(${EXTERNAL_SRC_DIR}/glfw GLFW)
endif()

