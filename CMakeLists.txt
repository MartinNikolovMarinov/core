cmake_minimum_required(VERSION 3.2)

project(test)

# Global CMake options:
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Generates compile_commands.json

# Create executable
add_executable(${PROJECT_NAME} test.cpp)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE
        -O0 -g -ggdb
        -std=c++17 -fno-exceptions
        -Wall -Wextra -Wno-unknown-pragmas
    )
else()
    message(FATAL_ERROR "Unsupported compiler!")
endif()

# Core library related:
option(CORE_LIBRARY_SHARED "" ON)
target_link_libraries(${PROJECT_NAME} core)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)

# Inlcude path:
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
