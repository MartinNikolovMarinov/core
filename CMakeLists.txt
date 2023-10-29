cmake_minimum_required(VERSION 3.2)

project(core)

# BEGIN Options

# Global CMake options:
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Generates compile_commands.json

if(NOT DEFINED CORE_LIBRARY_SHARED)
    set(CORE_LIBRARY_SHARED OFF CACHE BOOL "Build core as a shared library.")
endif()

if(NOT DEFINED CORE_DEBUG)
    if(NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "Release")
        # By default, in release builds, disable debug.
        set(CORE_DEBUG OFF CACHE BOOL "Enable debug mode.")
    else()
        set(CORE_DEBUG ON CACHE BOOL "Enable debug mode.")
    endif()
endif()

if(NOT DEFINED CORE_ASSERT_ENABLED)
    if(NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "Release")
        # By default, in release builds, disable asserts.
        set(CORE_ASSERT_ENABLED OFF CACHE BOOL "Enable asserts.")
    else()
        set(CORE_ASSERT_ENABLED ON CACHE BOOL "Enable asserts.")
    endif()
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Windows" OR
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

if(NOT DEFINED CORE_BUILD_TESTS)
    set(CORE_BUILD_TESTS OFF CACHE BOOL "Build core tests.")
endif()

if(NOT DEFINED CORE_RUN_COMPILETIME_TESTS)
    # This option can slow down compilation significantly.
    set(CORE_RUN_COMPILETIME_TESTS OFF CACHE BOOL "Run compile-time tests.")
endif()

if(NOT DEFINED CORE_USE_CPPCHECK)
    set(CORE_USE_CPPCHECK OFF CACHE BOOL "Use cppcheck. [WARNING] This makes compilation incredibly slow!")
endif()

if(CORE_USE_CPPCHECK)
    # IMPORTANT: This makes compilation incredibly slow!
    find_program(CMAKE_CXX_CPPCHECK NAMES cppcheck)
    if(CMAKE_CXX_CPPCHECK)
        list(APPEND CMAKE_CXX_CPPCHECK
                "--enable=all"
                "--inconclusive"
                "--inline-suppr"
                "--std=c++17"
                "--check-level=exhaustive"
                "--output-file=${CMAKE_BINARY_DIR}/cppcheck.out.txt"
                "--suppressions-list=${CMAKE_SOURCE_DIR}/CppCheckSuppressions.txt")
    endif()
endif()

# END Options

# BEGIN Print Selected Options

message(STATUS "[CORE] ---------------------------------------------")
message(STATUS "[CORE] Platform:          ${OS}")
# IMPORTANT: Do not relly on CMAKE_HOST_SYSTEM_PROCESSOR for anything serious !
message(STATUS "[CORE] Arch:              ${CMAKE_HOST_SYSTEM_PROCESSOR}")
message(STATUS "[CORE] C++ Version:       ${CMAKE_CXX_STANDARD}")
message(STATUS "[CORE] Compiler:          ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "[CORE] Debug:             ${CORE_DEBUG}")
message(STATUS "[CORE] Assert:            ${CORE_ASSERT_ENABLED}")
message(STATUS "[CORE] Shared:            ${CORE_LIBRARY_SHARED}")
message(STATUS "[CORE] Build Tests:       ${CORE_BUILD_TESTS}")
message(STATUS "[CORE] Run Compile Tests: ${CORE_RUN_COMPILETIME_TESTS}")
message(STATUS "[CORE] Use CppCheck:      ${CORE_USE_CPPCHECK}")

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
    src/bits.cpp
    src/bits.h
    src/char_ptr_conv.h
    src/char_ptr.h
    src/cmd_flag.h
    src/compiler.h
    src/core_math.cpp
    src/core_math.h
    src/core_traits.h
    src/expected.h
    src/hash_map.h
    src/hash.cpp
    src/hash.h
    src/intrinsic.cpp
    src/intrinsic.h
    src/ints.h
    src/io.cpp
    src/io.h
    src/mem.cpp
    src/mem.h
    src/rnd.cpp
    src/rnd.h
    src/static_arr.h
    src/str_builder.h
    src/tuple.h
    src/unique_ptr.h
    src/utf.h
    src/utils.cpp
    src/utils.h

    src/std/bbox.h
    src/std/ccmath.h
    src/std/core.h
    src/std/fs.cpp
    src/std/fs.h
    src/std/lines.h
    src/std/mat.h
    src/std/plt_error.h
    src/std/plt.h
    src/std/projections.cpp
    src/std/projections.h
    src/std/stacktrace.h
    src/std/stringer.h
    src/std/threading.h
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
        src/std/plt/unix/unix_plt.h
        src/std/plt/unix/unix_plt.cpp
        src/std/plt/unix/unix_stacktrace.cpp
        src/std/plt/unix/unix_threading.cpp
    )
elseif(OS STREQUAL "darwin")
    set(src_files ${src_files}
        src/std/plt/unix/unix_plt.h
        src/std/plt/unix/unix_plt.cpp
        src/std/plt/unix/unix_stacktrace.cpp
        src/std/plt/unix/unix_threading.cpp
    )
elseif(OS STREQUAL "windows")
    set(src_files ${src_files}
        src/std/plt/win64/win64_plt.cpp
        src/std/plt/win64/win64_stacktrace.cpp
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
    "CORE_RUN_COMPILETIME_TESTS=$<BOOL:${CORE_RUN_COMPILETIME_TESTS}>"
)

# END Definitions

# BEGIN Compile Options

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR
   CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR
   CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    # Use -Werror if you want to treat warnings as errors. For large code refactor that might be prefereable.
    set(mm_core_common_flags -std=c++17 -pthread
        -Wall -Wextra -Wfatal-errors -Wuninitialized -Wconversion -Wunused-value
        -Wdouble-promotion -Wswitch-enum -Wundef
        -Wno-unknown-pragmas -Wno-unused-function -Wno-variadic-macros
        -Wdisabled-optimization # warn if the compailer disables requestd level of optimization
    )
    set(mm_core_debug_flags -O0 -g -ggdb -Wnull-dereference)
    set(mm_core_release_flags -O3)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(mm_core_common_flags
        -std:c++17
        -nologo -FC
        -Wall -W4
        -WX # Compile warnings are errors
        /fastfail
    )
    set(mm_core_debug_flags -Zi -Od)
    set(mm_core_release_flags -O2)
else()
    message(FATAL_ERROR "Unsupported compiler")
endif()

if(CORE_DEBUG)
    target_compile_options(${PROJECT_NAME} PRIVATE ${mm_core_common_flags} ${mm_core_debug_flags})
else()
    target_compile_options(${PROJECT_NAME} PRIVATE ${mm_core_common_flags} ${mm_core_release_flags})
endif()

# END Compile Options

# BEGIN Adding Tests

if(CORE_BUILD_TESTS)
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
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR
       CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR
       CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
        set(mm_test_common_flags -std=c++17 -g -ggdb -pthread
            -Wall -Wextra -Wfatal-errors -Wuninitialized -Wconversion -Wunused-value
            -Wdouble-promotion -Wswitch-enum -Wundef
            -Wno-unknown-pragmas -Wno-variadic-macros
            -Wdisabled-optimization # warn if the compailer disables requestd level of optimization
        )
        set(mm_test_debug_flags -O0 -Wnull-dereference)
        set(mm_test_release_flags -O3)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        set(mm_test_common_flags
            -std:c++17
            -nologo -FC
            -Wall -W4
            /fastfail
        )
        set(mm_test_debug_flags -Zi -Od)
        set(mm_test_release_flags -O2)
    else()
        message(FATAL_ERROR "Unsupported compiler!")
    endif()

    if(CORE_DEBUG)
        target_compile_options(core_test PRIVATE ${mm_test_common_flags} ${mm_test_debug_flags})
    else()
        target_compile_options(core_test PRIVATE ${mm_test_common_flags} ${mm_test_release_flags})
    endif()

    # Add the test
    enable_testing()
    add_test(NAME core_test COMMAND core_test)
endif()

# END Adding Test

message(STATUS "[CORE] ---------------------------------------------")
