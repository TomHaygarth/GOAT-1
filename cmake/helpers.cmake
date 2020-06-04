cmake_minimum_required(VERSION 3.16)

# MacOs copy vulkan dependencies into 
# --------------------------------------------------------------------------------
macro(copy_vulkan_libs_to_app_bundle)
    if (BUILD_FOR_MAC)

        set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "")
        set(GLFW_BUILD_DOCS OFF CACHE BOOL "")
        set(GLFW_BUILD_TESTS OFF CACHE BOOL "")
        add_subdirectory(${EXTERNAL_SRC_DIR}/glfw GLFW)
    endif()
endmacro(copy_vulkan_libs_to_app_bundle)



