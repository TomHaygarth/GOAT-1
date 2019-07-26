cmake_minimum_required(VERSION 3.5)
project(OGAT-1 LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# base src files
set(base_src_includes
    "src/main.cpp"
)

set(executable_name "OGAT-1-test")

add_executable(${executable_name}
    "${base_src_includes}"
)


# glfw libs
find_package(glfw3 3.3 REQUIRED)
target_include_directories(${executable_name} PUBLIC ${GLFW3_INCLUDE_DIR})
target_link_libraries(${executable_name} ${GLFW3_LIBRARY})

find_package(OpenGL REQUIRED)
target_include_directories(${executable_name} PUBLIC ${OPENGL_INCLUDE_DIR})
target_link_libraries(${executable_name} ${OPENGL_gl_LIBRARY})

if (APPLE)
	find_library(ObjC objc)
	find_library(CoreFoundation CoreFoundation)
	find_library(CoreVideo CoreVideo)
	find_library(CoreGraphics CoreGraphics)
	find_library(IOKit IOKit)
	find_library(AppKit AppKit)
	target_link_libraries(${executable_name} ${CoreGraphics} ${CoreVideo} ${CoreFoundation} ${IOKit} ${AppKit} ${ObjC})
endif()

if ( CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU" )
    target_compile_options( ${executable_name} PRIVATE -Werror -Wall -Wextra -Wunreachable-code -Wpedantic)
endif()
if ( CMAKE_CXX_COMPILER_ID MATCHES "Clang" )
    target_compile_options( ${executable_name} PRIVATE -Wweak-vtables -Wexit-time-destructors -Wglobal-constructors -Wmissing-noreturn )
endif()
if ( CMAKE_CXX_COMPILER_ID MATCHES "MSVC" )
    target_compile_options( ${executable_name} PRIVATE /WX /W4 /w44265 /w44061 /w44062 )
endif()