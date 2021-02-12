cmake_minimum_required(VERSION 3.16)
# set(shared_target "${CMAKE_PROJECT_NAME}_shared")

# Source files
# --------------------------------------------------------------------------------

set (shared_sources
    "renderer/render_context.cpp"
    "renderer/render_context.hpp"

    "utility/logging.cpp"
    "utility/logging.hpp"
    "utility/optional.hpp"
    "utility/file/file_helper.cpp"
    "utility/file/file_helper.hpp"
    )
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${shared_sources})

add_library(shared_target STATIC
    ${shared_sources}
)

target_include_directories(shared_target PUBLIC
    "./"
)

message ("INCLUDING ${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(shared_target glm::glm)

# Renderer Options
# --------------------------------------------------------------------------------

if (BUILD_FOR_DESKTOP)
    if (BUILD_RENDERER_VULKAN)

        set(shared_window_platform_sources
            "window/platform/glfw_vulkan_window.cpp"
            )

        find_package(Vulkan REQUIRED)
        target_link_libraries(shared_target Vulkan::Vulkan)
        target_link_libraries(shared_target glfw)

    elseif(BUILD_RENDERER_OPENGL)

        target_link_libraries(shared_target glfw)
        message(FATAL_ERROR "NO OPENGL DESKTOP WINDOW HAS BEEN IMPLEMENTED YET")

    elseif(BUILD_RENDERER_DX12)

        message(FATAL_ERROR "NO DX12 DESKTOP WINDOW HAS BEEN IMPLEMENTED YET")

    endif()

else()
    message(FATAL_ERROR "NO NONE DESKTOP WINDOW HAS BEEN IMPLEMENTED YET")

endif()

set (shared_window_sources
    "window/window.hpp"
    ${shared_window_platform_sources}
    )
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${shared_window_sources})

target_sources(shared_target PUBLIC ${shared_window_sources})

# Precompile Headers
# --------------------------------------------------------------------------------
target_precompile_headers(shared_target PRIVATE
                          <cinttypes>
                          <fstream>
                          <string>
                          <vector>
                          <glm/vec4.hpp>
                          <glm/mat4x4.hpp>
                          utility/logging.hpp
                          utility/optional.hpp
                          utility/file/file_helper.hpp
                          )

target_compile_features(shared_target PUBLIC cxx_std_14)