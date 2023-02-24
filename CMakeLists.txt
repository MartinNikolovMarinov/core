cmake_minimum_required(VERSION 3.2)

project(test)

# Global CMake options:
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Generates compile_commands.json

# Create executable:
add_executable(${PROJECT_NAME} test.cpp)

# Compilation options:
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    if (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "Release")
        # Release build:
        target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE
            -O3 -g -ggdb
            -Wall -Wextra -Wno-unknown-pragmas
        )
    elseif (CMAKE_BUILD_TYPE STREQUAL "Debug")
        # Debug build:
        target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE
            -O0 -g -ggdb
            -Wall -Wextra -Wno-unknown-pragmas
        )
    else()
        message(FATAL_ERROR "Unsupported build type!")
    endif()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    if (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "Release")
        # Release build:
        target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE
            /O2 /Zi /Od /Oi
            /nologo /FC
            /EHa
            /W4
        )
    elseif (CMAKE_BUILD_TYPE STREQUAL "Debug")
        # Debug build:
        target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE
            /O0 /Zi /Od /Oi
            /nologo /FC
            /EHa
            /W4
        )
    else()
        message(FATAL_ERROR "Unsupported build type!")
    endif()
else()
    message(FATAL_ERROR "Unsupported compiler!")
endif()

# Core library related:
option(CORE_LIBRARY_SHARED "" ON)
target_link_libraries(${PROJECT_NAME} core)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)

# Inlcude path:
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
