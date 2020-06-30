cmake_minimum_required(VERSION 3.16)
# set(shared_target "${CMAKE_PROJECT_NAME}_shared")

# Source files
# --------------------------------------------------------------------------------
set (shared_sources
    "renderer/render_context.cpp"
    "renderer/render_context.hpp"

    "utility/logging.cpp"
    "utility/logging.hpp"
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${shared_sources})

add_library(shared_target STATIC
    ${shared_sources}
)

target_include_directories(shared_target PUBLIC
    "./"
)

message ("INCLUDING ${CMAKE_CURRENT_SOURCE_DIR}")

find_package(Vulkan REQUIRED)
target_link_libraries(shared_target Vulkan::Vulkan)
target_link_libraries(shared_target glfw)

# Precompile Headers
# --------------------------------------------------------------------------------
target_precompile_headers(shared_target PRIVATE
                          <glm/vec4.hpp>
                          <glm/mat4x4.hpp>
                          utility/logging.hpp
                          )

target_compile_features(shared_target PUBLIC cxx_std_14)