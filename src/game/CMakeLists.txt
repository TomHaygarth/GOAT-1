cmake_minimum_required(VERSION 3.16)

# game src files
set(game_src_includes
    "main.cpp"
)

# TODO: Make platform specific entry point files
# # Add platform specific entry point
# # --------------------------------------------------------------------------------
# if (BUILD_FOR_DESKTOP)
#     set(game_src_includes
#         "main.cpp"
#     )
# endif()


set(game_target "${CMAKE_PROJECT_NAME}")

add_executable(${game_target}
    "${game_src_includes}"
)

# Precompile Headers
# --------------------------------------------------------------------------------
target_precompile_headers(${game_target} PRIVATE
                          <glm/vec4.hpp>
                          <glm/mat4x4.hpp>
                          )

# glfw libs
find_package(GLFW3 3.3 REQUIRED)
target_include_directories(${game_target} PUBLIC ${GLFW3_INCLUDE_DIR})
target_link_libraries(${game_target} ${GLFW3_LIBRARY})

find_package(Vulkan REQUIRED)
target_link_libraries(${game_target} Vulkan::Vulkan)


# Platform includes
# --------------------------------------------------------------------------------
if (APPLE)
	find_library(ObjC objc)
	find_library(CoreFoundation CoreFoundation)
	find_library(CoreVideo CoreVideo)
	find_library(CoreGraphics CoreGraphics)
	find_library(IOKit IOKit)
	find_library(AppKit AppKit)
    target_link_libraries(${game_target} ${CoreGraphics} ${CoreVideo} ${CoreFoundation} ${IOKit} ${AppKit} ${ObjC})
    
    set_target_properties(${game_target} PROPERTIES
                          MACOSX_BUNDLE TRUE)
endif()

if ( CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU" )
    target_compile_options( ${game_target} PRIVATE -Werror -Wall -Wextra -Wunreachable-code -Wpedantic)
endif()
if ( CMAKE_CXX_COMPILER_ID MATCHES "Clang" )
    target_compile_options( ${game_target} PRIVATE -Wweak-vtables -Wexit-time-destructors -Wglobal-constructors -Wmissing-noreturn )
endif()
if ( CMAKE_CXX_COMPILER_ID MATCHES "MSVC" )
    target_compile_options( ${game_target} PRIVATE /WX /W4 /w44265 /w44061 /w44062 )
endif()