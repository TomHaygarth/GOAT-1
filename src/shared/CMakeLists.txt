cmake_minimum_required(VERSION 3.16)
# set(shared_target "${CMAKE_PROJECT_NAME}_shared")

# Source files
# --------------------------------------------------------------------------------

set (shared_sources

    "assets/asset_handle.hpp"

    "audio/audio_manager.cpp"
    "audio/audio_manager.hpp"

    "utility/logging.cpp"
    "utility/logging.hpp"
    "utility/optional.hpp"
    "utility/file/file_helper.cpp"
    "utility/file/file_helper.hpp"
    )
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${shared_sources})

set (renderable_sources
    "renderer/render_context.cpp"
    "renderer/render_context.hpp"
    "renderer/renderable.cpp"
    "renderer/renderable.hpp"

    "renderer/primitives/shape_2d.cpp"
    "renderer/primitives/shape_2d.hpp"
    )
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${renderable_sources})

add_library(shared_target STATIC
    ${shared_sources}
    ${renderable_sources}
)

target_include_directories(shared_target PUBLIC
    "./"
    ${EXTERNAL_SRC_DIR}/imgui
    ${EXTERNAL_SRC_DIR}/sokol
)

message ("INCLUDING ${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(shared_target glm::glm)

set (imgui_sources
    "${EXTERNAL_SRC_DIR}/imgui/imgui.cpp"
    "${EXTERNAL_SRC_DIR}/imgui/imgui.h"
    "${EXTERNAL_SRC_DIR}/imgui/imgui_internal.h"
    "${EXTERNAL_SRC_DIR}/imgui/imgui_demo.cpp"
    "${EXTERNAL_SRC_DIR}/imgui/imgui_draw.cpp"
    "${EXTERNAL_SRC_DIR}/imgui/imgui_tables.cpp"
    "${EXTERNAL_SRC_DIR}/imgui/imgui_widgets.cpp"
    "${EXTERNAL_SRC_DIR}/imgui/misc/cpp/imgui_stdlib.cpp"
    "${EXTERNAL_SRC_DIR}/imgui/misc/cpp/imgui_stdlib.h"
)

# Renderer Options
# --------------------------------------------------------------------------------

if (BUILD_FOR_DESKTOP)
    if (BUILD_RENDERER_VULKAN)

        set(shared_window_platform_sources
            "window/platform/glfw_window.cpp"

            "renderer/vulkan/vulkan_render_context.cpp"
            "renderer/vulkan/vulkan_render_context.hpp"
        )

        find_package(Vulkan REQUIRED)
        target_link_libraries(shared_target Vulkan::Vulkan)
        target_link_libraries(shared_target glfw)
        target_compile_definitions(shared_target PUBLIC GLFW_INCLUDE_VULKAN)
        target_compile_definitions(shared_target PUBLIC GOAT_USE_VULKAN)

        set(imgui_impl_sources
            "${EXTERNAL_SRC_DIR}/imgui/backends/imgui_impl_glfw.cpp"
            "${EXTERNAL_SRC_DIR}/imgui/backends/imgui_impl_glfw.h"
            "${EXTERNAL_SRC_DIR}/imgui/backends/imgui_impl_vulkan.cpp"
            "${EXTERNAL_SRC_DIR}/imgui/backends/imgui_impl_vulkan.h"
        )

    elseif(BUILD_RENDERER_OPENGL)
        set(shared_window_platform_sources
        "window/platform/glfw_window.cpp"

        "renderer/opengl/opengl_render_context.cpp"
        "renderer/opengl/opengl_render_context.hpp"
        )

        target_link_libraries(shared_target glad)
        target_link_libraries(shared_target glfw)
        target_compile_definitions(shared_target PUBLIC GOAT_USE_OPENGL)

        set(imgui_impl_sources
            "${EXTERNAL_SRC_DIR}/imgui/backends/imgui_impl_glfw.cpp"
            "${EXTERNAL_SRC_DIR}/imgui/backends/imgui_impl_glfw.h"
            "${EXTERNAL_SRC_DIR}/imgui/backends/imgui_impl_opengl3.cpp"
            "${EXTERNAL_SRC_DIR}/imgui/backends/imgui_impl_opengl3.h"
        )

    elseif(BUILD_RENDERER_DX12)

        target_compile_definitions(shared_target PUBLIC GOAT_USE_DX12)
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
target_sources(shared_target PRIVATE ${shared_window_sources})

set (imgui_sources_all
    ${imgui_sources}
    ${imgui_impl_sources}
)

source_group(TREE ${EXTERNAL_SRC_DIR} FILES ${imgui_sources_all})
target_sources(shared_target PRIVATE ${imgui_sources_all})

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
                          renderer/renderable.hpp
                          )

target_compile_features(shared_target PUBLIC cxx_std_14)
