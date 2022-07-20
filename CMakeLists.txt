cmake_minimum_required(VERSION 3.2)

project(test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Generates compile_commands.json

macro(set_platform_variables)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(COMPILER "gcc" CACHE INTERNAL "used compiler")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(COMPILER "clang" CACHE INTERNAL "used compiler")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        set(COMPILER "msvc" CACHE INTERNAL "used compiler")
    endif()

    if (WIN32 OR MSVC OR MSYS OR MINGW)
        set(OS "windows" CACHE INTERNAL "operating system")
    elseif(APPLE)
        set(OS "osx" CACHE INTERNAL "operating system")
    elseif(UNIX AND NOT APPLE)
        set(OS "linux" CACHE INTERNAL "operating system")
    endif()

    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        set(DEBUG 0 CACHE INTERNAL "debug mode")
    else()
        set(CMAKE_BUILD_TYPE DEBUG CACHE INTERNAL "")
        set(DEBUG 1 CACHE INTERNAL "debug mode")
    endif()
endmacro()

# Check system requrenments:
set_platform_variables()
message(STATUS "[INFO] Compiler: ${COMPILER}")
message(STATUS "[INFO] Platform: ${OS}")
message(STATUS "[INFO] Debug: ${DEBUG}")

add_executable(${PROJECT_NAME} test.cpp)

target_link_libraries(${PROJECT_NAME} core)

set(COMMON_CFLAGS
    -std=c++17 -fno-exceptions
    -Wall -Wextra -Wconversion
    -Wno-unknown-pragmas)

if (COMPILER STREQUAL "gcc" OR COMPILER STREQUAL "clang")
    target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE ${COMMON_CFLAGS} -O0 -std=c++17 -ggdb -fno-exceptions -Wno-unknown-pragmas)
else()
    message(FATAL_ERROR "Unsupported compiler!")
endif()

option(CORE_LIBRARY_SHARED "Should core be build as a shared library" OFF) # Off by default
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
