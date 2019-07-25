cmake_minimum_required(VERSION 3.5)
project(OGAT-1 LANGUAGES CXX)

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

if ( CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU" )
    target_compile_options( ${executable_name} PRIVATE -Werror -Wall -Wextra -Wunreachable-code -Wpedantic)
endif()
if ( CMAKE_CXX_COMPILER_ID MATCHES "Clang" )
    target_compile_options( ${executable_name} PRIVATE -Wweak-vtables -Wexit-time-destructors -Wglobal-constructors -Wmissing-noreturn )
endif()
if ( CMAKE_CXX_COMPILER_ID MATCHES "MSVC" )
    target_compile_options( ${executable_name} PRIVATE /WX /W4 /w44265 /w44061 /w44062 )
endif()