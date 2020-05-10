cmake_minimum_required(VERSION 3.16)

# editor src files
set(editor_src_includes
    "main.cpp"
)

set(editor_target "${CMAKE_PROJECT_NAME}-editor")

add_executable(${editor_target}
    "${editor_src_includes}"
)

# Precompile Headers
# --------------------------------------------------------------------------------
target_precompile_headers(${editor_target} PRIVATE
                          <glm/vec4.hpp>
                          <glm/mat4x4.hpp>
                          )

# glfw libs
find_package(GLFW3 3.3 REQUIRED)
target_include_directories(${editor_target} PUBLIC ${GLFW3_INCLUDE_DIR})
target_link_libraries(${editor_target} ${GLFW3_LIBRARY})

find_package(Vulkan REQUIRED)
target_link_libraries(${editor_target} Vulkan::Vulkan)


# Platform includes
# --------------------------------------------------------------------------------
if (APPLE)
	find_library(ObjC objc)
	find_library(CoreFoundation CoreFoundation)
	find_library(CoreVideo CoreVideo)
	find_library(CoreGraphics CoreGraphics)
	find_library(IOKit IOKit)
	find_library(AppKit AppKit)
	target_link_libraries(${editor_target} ${CoreGraphics} ${CoreVideo} ${CoreFoundation} ${IOKit} ${AppKit} ${ObjC})
endif()

if ( CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU" )
    target_compile_options( ${editor_target} PRIVATE -Werror -Wall -Wextra -Wunreachable-code -Wpedantic)
endif()
if ( CMAKE_CXX_COMPILER_ID MATCHES "Clang" )
    target_compile_options( ${editor_target} PRIVATE -Wweak-vtables -Wexit-time-destructors -Wglobal-constructors -Wmissing-noreturn )
endif()
if ( CMAKE_CXX_COMPILER_ID MATCHES "MSVC" )
    target_compile_options( ${editor_target} PRIVATE /WX /W4 /w44265 /w44061 /w44062 )
endif()