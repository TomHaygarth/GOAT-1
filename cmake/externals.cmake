cmake_minimum_required(VERSION 3.16)

set (EXTERNAL_SRC_DIR "${PROJECT_SOURCE_DIR}/externals")

# GLFW include
# --------------------------------------------------------------------------------
if (BUILD_FOR_DESKTOP)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "")
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "")
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "")
    message("ADDING SUBDIR GLFW : ${EXTERNAL_SRC_DIR}/glfw")
    add_subdirectory(${EXTERNAL_SRC_DIR}/glfw GLFW)

    set(VULKAN_EXTERNALS_DIR "${EXTERNAL_SRC_DIR}/vulkan-1.2.135.0")

endif()

