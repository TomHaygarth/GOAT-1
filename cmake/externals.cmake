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
    add_subdirectory(${EXTERNAL_SRC_DIR}/glm)

    set(VULKAN_EXTERNALS_DIR "${EXTERNAL_SRC_DIR}/vulkan-1.2.135.0")
    set(MOLTENVK_EXTERNALS_DIR "${EXTERNAL_SRC_DIR}/MoltenVK")

    if (BUILD_FOR_MAC)
        set(ENV{VULKAN_SDK} ${VULKAN_EXTERNALS_DIR}/macos)
        message("Set VULKAN_SDK env var to $ENV{VULKAN_SDK}")
	elseif(BUILD_FOR_WINDOWS)
	    find_package(VULKAN REQUIRED)
    endif()

endif()

if (BUILD_RENDERER_OPENGL)
    add_subdirectory(${EXTERNAL_SRC_DIR}/glad)
    add_subdirectory(${EXTERNAL_SRC_DIR}/SPIRV-Cross)
endif()

