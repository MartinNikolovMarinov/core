cmake_minimum_required(VERSION 3.2)

project(core)

# Options.
# The following options can be defined by the parent CMakeLists.txt file:
if (NOT DEFINED CORE_LIBRARY_SHARED)
    set(CORE_LIBRARY_SHARED OFF)
endif()

if (NOT DEFINED CORE_DEBUG)
    if (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "Release")
        # Build Type not set, defaulting to Relese...
        set(CORE_DEBUG OFF)
    else()
        set(CORE_DEBUG ON)
    endif()
endif()

# OS check:
# TODO: Use CMAKE_SYSTEM_NAME for the OS check!
if(WIN32 OR MSVC OR MSYS OR MINGW)
    set(OS "windows")
elseif(APPLE)
    set(OS "darwin")
elseif(UNIX AND NOT APPLE)
    set(OS "linux")
else()
    message(FATAL_ERROR "Unsupported OS: ${OS}!")
endif()

# Print build info:
message(STATUS "[CORE] ---------------------------------------------")
message(STATUS "[CORE] Platform: ${OS}")
message(STATUS "[CORE] Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "[CORE] Debug:    ${CORE_DEBUG}")
message(STATUS "[CORE] Shared:   ${CORE_LIBRARY_SHARED}")

# Source files list:
set(src_files
    API.h
    system_checks.h
    types.h
    core.h

    intrinsic.h
    intrinsic.cpp
    utils.h
    utils.cpp
    tuple.h
    core_math.h
    core_math.cpp
    mem.h
    mem.cpp
    ints.h
    char_ptr.h
    char_ptr.cpp
    rnd.h
    rnd.cpp
    bits.h
    bits.cpp
    expected.h
    utf.h
    utf.cpp
    alloc.h
    arr.h
    alloc_bump.h
    ansi_escape_codes.h

    std/core.h
    std/alloc_std_stats.cpp
    std/alloc_std_stats.h
    std/alloc_std.cpp
    std/alloc_std.h
    std/fs.h
    std/once.h
    std/plt.h
    std/traits.h
    std/vec.h
)

# OS specific source files:
if(OS STREQUAL "linux")
    set(src_files ${src_files}
        std/plt/unix/unix_plt.cpp
    )
elseif(OS STREQUAL "windows")
    set(src_files ${src_files}
        std/platform/win64/win64_plt.cpp
    )
endif()

# Support building as both shared and static library:
if(CORE_LIBRARY_SHARED)
    message(STATUS "[CORE] Building corelib as a shared library")
    add_library(${PROJECT_NAME} SHARED ${src_files})
    # Set symbols visibility to be hidden by default:
    set_target_properties(${PROJECT_NAME} PROPERTIES
        C_VISIBILITY_PRESET hidden
        C_VISIBILITY_INLINE_PRESET hidden
        CXX_VISIBILITY_PRESET hidden
        CXX_VISIBILITY_INLINE_PRESET hidden
    )
else()
    message(STATUS "[CORE] Building corelib as a static library")
    add_library(${PROJECT_NAME} STATIC ${src_files})
endif()

# Include Directories:
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Compiler options:
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # Common flags:
    target_compile_options(${PROJECT_NAME} PRIVATE
        -std=c++17
        -fno-exceptions
        -Wall -Wextra -Wno-unknown-pragmas -Wno-unused-function
    )
    if (CORE_DEBUG)
        # Debug flags:
        target_compile_options(${PROJECT_NAME} PRIVATE -O0 -g -ggdb)
    else()
        # Release flags:
        target_compile_options(${PROJECT_NAME} PRIVATE -O3)
    endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # Common flags:
    target_compile_options(${PROJECT_NAME} PRIVATE
        -std:c++17
        -nologo -FC -EHa
        -W4
    )
    if (CORE_DEBUG)
        # Debug flags:
        target_compile_options(${PROJECT_NAME} PRIVATE -Zi -Od -Oi)
    else()
        # Release flags:
        target_compile_options(${PROJECT_NAME} PRIVATE -O2)
    endif()
else()
    message(FATAL_ERROR "Unsupported compiler")
endif()

# Definitions:
target_compile_definitions(${PROJECT_NAME} PRIVATE
    "CORE_LIBRARY_SHARED=$<BOOL:${CORE_LIBRARY_SHARED}>" # turn CMake Boolean to 1 or 0
    -DCORE_LIBRARY_BUILD=1
    "CORE_DEBUG=$<BOOL:${CORE_DEBUG}>"
)

message(STATUS "[CORE] ---------------------------------------------")
