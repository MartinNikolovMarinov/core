cmake_minimum_required(VERSION 3.2)

project(core)

# BEGIN Options

# Global CMake options:
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Generates compile_commands.json

if (NOT DEFINED CORE_LIBRARY_SHARED)
    set(CORE_LIBRARY_SHARED OFF CACHE BOOL "Build core as a shared library.")
endif()

if (NOT DEFINED CORE_DEBUG)
    if (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "Release")
        # By default, in release builds, disable debug.
        set(CORE_DEBUG OFF CACHE BOOL "Enable debug mode.")
    else()
        set(CORE_DEBUG ON CACHE BOOL "Enable debug mode.")
    endif()
endif()

if (NOT DEFINED CORE_ASSERT_ENABLED)
    if (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "Release")
        # By default, in release builds, disable asserts.
        set(CORE_ASSERT_ENABLED OFF CACHE BOOL "Enable asserts.")
    else()
        set(CORE_ASSERT_ENABLED ON CACHE BOOL "Enable asserts.")
    endif()
endif()

if(MAKE_SYSTEM_NAME STREQUAL "Windows" OR
   CMAKE_SYSTEM_NAME STREQUAL "CYGWIN" OR
   CMAKE_SYSTEM_NAME STREQUAL "MSYS" OR
   CMAKE_SYSTEM_NAME STREQUAL "WindowsCE")
    set(OS "windows")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(OS "darwin")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(OS "linux")
else()
    set(OS "unsupported")
    message(FATAL_ERROR "Unsupported OS: ${CMAKE_SYSTEM_NAME}!")
endif()

if (CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "x86_64")
    set(ARCH "x64")
else()
    set(ARCH "unsupported")
    message(FATAL_ERROR "Unsupported architecture: ${CMAKE_HOST_SYSTEM_PROCESSOR}!")
endif()

if (NOT DEFINED CORE_BUILD_TESTS)
    enable_testing()
    set(CORE_BUILD_TESTS OFF CACHE BOOL "Build core tests.")
endif()

if (NOT DEFINED RUN_COMPILETIME_TESTS)
    # This option can slow down compilation significantly.
    set(RUN_COMPILETIME_TESTS OFF CACHE BOOL "Run compile-time tests.")
endif()

# END Options

# BEGIN Print Selected Options

message(STATUS "[CORE] ---------------------------------------------")
message(STATUS "[CORE] Platform:          ${OS}")
message(STATUS "[CORE] Arch:              ${ARCH}")
message(STATUS "[CORE] Compiler:          ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "[CORE] Debug:             ${CORE_DEBUG}")
message(STATUS "[CORE] Assert:            ${CORE_ASSERT_ENABLED}")
message(STATUS "[CORE] Shared:            ${CORE_LIBRARY_SHARED}")
message(STATUS "[CORE] Build Tests:       ${CORE_BUILD_TESTS}")
message(STATUS "[CORE] Run Compile Tests: ${RUN_COMPILETIME_TESTS}")

# END Print Selected Options

# BEGIN Declare Source Files

set(src_files
    src/API.h
    src/system_checks.h
    src/types.h
    src/core.h

    src/algorithms.h
    src/alloc_bump.h
    src/alloc.h
    src/ansi_escape_codes.h
    src/arr.h
    src/bits.h
    src/char_ptr_conv.h
    src/char_ptr.h
    src/cmd_flag.h
    src/core_math.h
    src/core_traits.h
    src/expected.h
    src/intrinsic.cpp
    src/intrinsic.h
    src/ints.h
    src/mem.cpp
    src/mem.h
    src/rnd.cpp
    src/rnd.h
    src/static_arr.h
    src/tuple.h
    src/utf.h
    src/utils.cpp
    src/utils.h

    src/std/bbox.h
    src/std/core.h
    src/std/fs.h
    src/std/lines.h
    src/std/mat.h
    src/std/once.h
    src/std/plt.h
    src/std/projections.h
    src/std/stacktrace.h
    src/std/stringer.h
    src/std/traits.h
    src/std/transforms.h
    src/std/vec.h

    src/std/allocators/alloc_std_stats.cpp
    src/std/allocators/alloc_std_stats.h
    src/std/allocators/alloc_std.cpp
    src/std/allocators/alloc_std.h
)

# OS specific files
if(OS STREQUAL "linux")
    set(src_files ${src_files}
        src/std/plt/unix/unix_plt.cpp
        src/std/plt/unix/unix_stacktrace.cpp
    )
elseif(OS STREQUAL "windows")
    set(src_files ${src_files}
        src/std/plt/win64/win64_plt.cpp
    )
endif()

# END Declare Source Files

# BEGIN Add Library

if(CORE_LIBRARY_SHARED)
    message(STATUS "[CORE] Configuring corelib as a shared library")
    add_library(${PROJECT_NAME} SHARED ${src_files})
    # Set symbols visibility to be hidden by default:
    set_target_properties(${PROJECT_NAME} PROPERTIES
        C_VISIBILITY_PRESET hidden
        C_VISIBILITY_INLINE_PRESET hidden
        CXX_VISIBILITY_PRESET hidden
        CXX_VISIBILITY_INLINE_PRESET hidden
    )
else()
    message(STATUS "[CORE] Configuring corelib as a static library")
    add_library(${PROJECT_NAME} STATIC ${src_files})
endif()

# END Add Library.

# BEGIN Include Path Directories

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# END Include Path Directories

# BEGIN Definitions

target_compile_definitions(${PROJECT_NAME} PRIVATE
    "CORE_LIBRARY_SHARED=$<BOOL:${CORE_LIBRARY_SHARED}>" # turn CMake Boolean to 1 or 0
    -DCORE_LIBRARY_BUILD=1
)
target_compile_definitions(${PROJECT_NAME} PUBLIC
    "CORE_DEBUG=$<BOOL:${CORE_DEBUG}>"
    "CORE_ASSERT_ENABLED=$<BOOL:${CORE_ASSERT_ENABLED}>"
    "RUN_COMPILETIME_TESTS=$<BOOL:${RUN_COMPILETIME_TESTS}>"
)

# END Definitions

# BEGIN Compile Options

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # Common flags:
    target_compile_options(${PROJECT_NAME} PRIVATE
        -std=c++17
        -fno-exceptions
        -Wall -Wextra
        -Wfatal-errors
        -Wno-unknown-pragmas -Wno-unused-function
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
        /std:c++17
        /nologo /FC
        /W4
        /EHa-
    )
    if (CORE_DEBUG)
        # Debug flags:
        target_compile_options(${PROJECT_NAME} PRIVATE /O0 /Zi /Od /Oi)
    else()
        # Release flags:
        target_compile_options(${PROJECT_NAME} PRIVATE /O2)
    endif()
else()
    message(FATAL_ERROR "Unsupported compiler")
endif()

# END Compile Options

# BEGIN Adding Tests

if (CORE_BUILD_TESTS)
    message(STATUS "[CORE] Configuring tests")

    # Create executable
    add_executable(core_test test.cpp)

    # Link core
    target_link_libraries(core_test ${PROJECT_NAME})

    # Definitions
    target_compile_definitions(core_test PRIVATE
        -DPATH_TO_TEST_DATA="${CMAKE_CURRENT_SOURCE_DIR}/test/data"
    )

    # Compile options
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        # Common flags:
        target_compile_options(core_test PRIVATE
            -Wall -Wextra -Wno-unknown-pragmas
            -g -ggdb
            -Wfatal-errors
        )

        if (CORE_DEBUG)
            # Debug flags:
            target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -O3)
        else()
            # Release flags:
            target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -O0)
        endif()
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        # Common flags:
        target_compile_options(core_test PRIVATE
            /std:c++17 /nologo /FC /EHa /W4
        )

        if (CORE_DEBUG)
            # Debug flags:
            target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE /O2)
        else()
            # Release flags:
            target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE /O0 /Zi /Od /Oi)
        endif()
    else()
        message(FATAL_ERROR "Unsupported compiler!")
    endif()

    # Add the test
    add_test(NAME core_test COMMAND core_test)
endif()

# END Adding Test

message(STATUS "[CORE] ---------------------------------------------")
