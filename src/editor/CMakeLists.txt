cmake_minimum_required(VERSION 3.16)

# editor src files
set(editor_src_includes
    "main.cpp"
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${editor_src_includes})

# set(editor_target "${CMAKE_PROJECT_NAME}-editor")

add_executable(editor_target
    "${editor_src_includes}"
)

# Precompile Headers
# --------------------------------------------------------------------------------
target_precompile_headers(editor_target PRIVATE
                          <cinttypes>
                          <string>
                          <vector>
                          <glm/vec4.hpp>
                          <glm/mat4x4.hpp>
                          )

# glfw libs
# find_package(GLFW3 3.3 REQUIRED)
# target_include_directories(game_target PUBLIC ${GLFW3_INCLUDE_DIR})

if (BUILD_FOR_DESKTOP)
    find_package(Vulkan REQUIRED)

    target_link_libraries(editor_target PUBLIC
        shared_target
        glfw
        Vulkan::Vulkan
        glm::glm
    )
endif ()

# Platform includes
# --------------------------------------------------------------------------------
if (APPLE)
	find_library(ObjC objc)
	find_library(CoreFoundation CoreFoundation)
	find_library(CoreVideo CoreVideo)
	find_library(CoreGraphics CoreGraphics)
	find_library(IOKit IOKit)
	find_library(AppKit AppKit)
	target_link_libraries(editor_target PUBLIC ${CoreGraphics} ${CoreVideo} ${CoreFoundation} ${IOKit} ${AppKit} ${ObjC})

elseif (BUILD_FOR_WINDOWS)
    set_property(TARGET editor_target PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/resources")
endif()

if ( CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU" )
    target_compile_options( editor_target PRIVATE -Werror -Wall -Wextra -Wunreachable-code -Wpedantic)
endif()
if ( CMAKE_CXX_COMPILER_ID MATCHES "Clang" )
    target_compile_options( editor_target PRIVATE -Wweak-vtables -Wexit-time-destructors -Wglobal-constructors -Wmissing-noreturn )
endif()
if ( CMAKE_CXX_COMPILER_ID MATCHES "MSVC" )
    target_compile_options( editor_target PRIVATE /WX /W4 /w44265 /w44061 /w44062 )
endif()