cmake_minimum_required(VERSION 3.16)
set(shared_target "${CMAKE_PROJECT_NAME}_shared")

# Source files
# --------------------------------------------------------------------------------
set (shared_sources
    "renderer/render_context.hpp"
)

# Precompile Headers
# --------------------------------------------------------------------------------
target_precompile_headers(${shared_target} PRIVATE
                          <glm/vec4.hpp>
                          <glm/mat4x4.hpp>
                          )

add_library(${shared_target} STATIC
    ${shared_target}
)

target_compile_features(${shared_target} PUBLIC cxx_std_14)