cmake_minimum_required(VERSION 3.2)

project(core)

set(src_files
    core.h
    types.h

    bits.cpp
    bits.h
    char_ptr.cpp
    char_ptr.h
    intrin.cpp
    intrin.h
    ints.cpp
    ints.h
    math.cpp
    math.h
    mem.cpp
    mem.h
    rnd.cpp
    rnd.h
    tuple.h
    utf.cpp
    utf.h
    utils.cpp
    utils.h
)

# Support building as both shared and static library:
if(CORE_LIBRARY_SHARED)
    message(STATUS "Building core as a shared library")
    add_library(${PROJECT_NAME} SHARED ${src_files})
    # Set symbols visibility to be hidden by default:
    set_target_properties(${PROJECT_NAME} PROPERTIES
        C_VISIBILITY_PRESET hidden
        C_VISIBILITY_INLINE_PRESET hidden
        CXX_VISIBILITY_PRESET hidden
        CXX_VISIBILITY_INLINE_PRESET hidden
    )
else()
    message(STATUS "Building core as a static library")
    add_library(${PROJECT_NAME} STATIC ${src_files})
endif()

set(COMMON_CFLAGS
    -nostdlib -fno-exceptions
    -Wall -Wextra
    -Wno-unknown-pragmas)

# Compiler flags for DEBUG and for RELEASE builds:
if(DEBUG)
    target_compile_options(${PROJECT_NAME} PRIVATE ${COMMON_CFLAGS} -O0 -g -ggdb)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "CORE_LIBRARY_SHARED=$<BOOL:${CORE_LIBRARY_SHARED}>" # turn CMake Boolean to 1 or 0
        -DCORE_LIBRARY_BUILD=1
        -DDEBUG=1
        -DUSE_ASSERT=1
    )
else()
    target_compile_options(${PROJECT_NAME} PRIVATE ${COMMON_CFLAGS} -03)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "CORE_LIBRARY_SHARED=$<BOOL:${CORE_LIBRARY_SHARED}>" # turn CMake Boolean to 1 or 0
        -DCORE_LIBRARY_BUILD=1
        -DDEBUG=0
        -DUSE_ASSERT=0
    )
endif()
