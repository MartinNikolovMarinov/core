cmake_minimum_required(VERSION 3.2)

project(core)

# Options.
# The following options can be defined by the parent CMakeLists.txt file:
if (NOT DEFINED CORE_LIBRARY_SHARED)
    set(CORE_LIBRARY_SHARED OFF)
endif()
if (NOT DEFINED CORE_DEBUG)
    set(CORE_DEBUG OFF)
endif()

# OS check:
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
    system_checks.h
    tuple.h
    utf.cpp
    utf.h
    utils.cpp
    utils.h
)

# OS specific source files:
if(OS STREQUAL "linux")
    set(src_files ${src_files}
        std/platform/unix/plt.cpp
        std/platform/unix/time.cpp
    )
elseif(OS STREQUAL "windows")
    set(src_files ${src_files}
        std/platform/win64/plt.cpp
        std/platform/win64/time.cpp
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
    ${CMAKE_CURRENT_SOURCE_DIR}/std
)

# Compiler options:
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # Common flags:
    target_compile_options(${PROJECT_NAME} PRIVATE
        -fno-exceptions
        -Wall -Wextra -Wno-unknown-pragmas
    )
    if (CORE_DEBUG)
        # Debug flags:
        target_compile_options(${PROJECT_NAME} PRIVATE -O0 -g -ggdb)
    else()
        # Release flags:
        target_compile_options(${PROJECT_NAME} PRIVATE -O3)
    endif()

    # Definitions:
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "CORE_LIBRARY_SHARED=$<BOOL:${CORE_LIBRARY_SHARED}>" # turn CMake Boolean to 1 or 0
        -DCORE_LIBRARY_BUILD=1
        "CORE_DEBUG=$<BOOL:${CORE_DEBUG}>"
    )

elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message(FATAL_ERROR "TODO: MSVC support not implemented yet")
else()
    message(FATAL_ERROR "Unsupported compiler")
endif()

message(STATUS "[CORE] ---------------------------------------------")
