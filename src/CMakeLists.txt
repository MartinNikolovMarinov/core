cmake_minimum_required(VERSION 3.2)

project(core)

set(src_files
    char_ptr.cpp
    intrin.cpp
    ints.cpp
    math.cpp
    rnd.cpp
    mem.cpp
    bits.cpp
    utf.cpp
    utils.cpp
)

if(OS STREQUAL "linux" OR OS STREQUAL "osx")
    # Add os specific files here:
    # set(src_files ${src_files}
    #    unix/os.cpp
    # )
endif()

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

# Adds the core library header files to the compiler include path.
# This allows including them without specifiling the exact path on the file system.
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Compiler flags for DEBUG and for RELEASE builds:
set(COMMON_CFLAGS -nostdlib -Wno-unknown-pragmas)
if(DEBUG)
    target_compile_options(${PROJECT_NAME} PRIVATE ${COMMON_CFLAGS} -O0 -g -Wall)
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
