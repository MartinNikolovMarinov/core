cmake_minimum_required(VERSION 3.2)
project(core VERSION 0.5.0)

set(target_core core)
set(target_core_upper CORE)
set(target_test core_test)
# set(target_test core_sandbox)

# Standard Requirements:

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Generates compile_commands.json

# Download and update git submodules:

find_package(Git QUIET)
if (GIT_FOUND)
    message(STATUS "Submodule update")
    execute_process(
        COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        RESULT_VARIABLE GIT_SUBMOD_RESULT
    )
    if (NOT GIT_SUBMOD_RESULT EQUAL "0")
        message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
    endif()
else()
    message(WARNING "Git is not found. It is required to update submodules.")
endif()

# Include cmake modules from git submodules:

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake_local")

include(DetectOS)
include(CompilerOptions)
include(Logger)
include(CoreDefaultFlags)

init_logger("[CORE]")

# ---------------------------------------- Begin Options ---------------------------------------------------------------

if(NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CORE_DEBUG OFF)
else()
    set(CORE_DEBUG ON)
endif()

option(CORE_LIBRARY_SHARED "Build core as a shared library." OFF)
option(CORE_ASSERT_ENABLED "Enable asserts." OFF)
option(CORE_BUILD_TESTS "Build core tests." OFF)
option(CORE_TESTS_USE_ANSI "Use ANSI escape codes in tests." OFF)
option(CORE_TESTS_STOP_ON_FIRST_FAILED "Stop running tests on first failure." OFF)
option(CORE_RUN_COMPILETIME_TESTS "Run compile-time tests." OFF)
option(CORE_CODE_COVERAGE "Enable code coverage." OFF)
option(CORE_SAVE_TEMPORARY_FILES "Save compiler temporary files. [WARNING] This down slows compilation." OFF)

# Print Selected Options:

log_info("---------------------------------------------")
log_info("Version:                   ${PROJECT_VERSION}")
log_info("Platform:                  ${OS}")
log_info("Arch:                      ${CMAKE_SYSTEM_PROCESSOR}")
log_info("C++ Version:               ${CMAKE_CXX_STANDARD}")
log_info("Compiler:                  ${CMAKE_CXX_COMPILER_ID}")
log_info("Debug:                     ${CORE_DEBUG}")
log_info("Assert:                    ${CORE_ASSERT_ENABLED}")
log_info("Shared:                    ${CORE_LIBRARY_SHARED}")
log_info("Build Tests:               ${CORE_BUILD_TESTS}")
log_info("Use ANSI in Tests:         ${CORE_TESTS_USE_ANSI}")
log_info("Stop on first failed test: ${CORE_TESTS_STOP_ON_FIRST_FAILED}")
log_info("Run Compile Tests:         ${CORE_RUN_COMPILETIME_TESTS}")
log_info("Code Coverage:             ${CORE_CODE_COVERAGE}")
log_info("Save Temporary Files:      ${CORE_SAVE_TEMPORARY_FILES}")

# ---------------------------------------- End Options -----------------------------------------------------------------

# ---------------------------------------- Begin Declare Source Files --------------------------------------------------

set(core_src
    src/core_alloc.cpp
    src/core_assert.cpp
    src/core_exec_ctx.cpp
    src/core_hash.cpp
    src/core_logger.cpp
    src/core_rnd.cpp

    src/allocators/bump_allocator.cpp
    src/allocators/std_allocator.cpp
    src/allocators/std_arena_allocator.cpp
    src/allocators/std_stats_allocator.cpp

    src/math/core_projections.cpp

    src/testing/testing_framework.cpp

    src/plt/core_fs.cpp
)

if(OS STREQUAL "linux")
    set(core_src ${core_src}
        src/plt/unix/unix_threading.cpp
        src/plt/unix/unix_time.cpp
        src/plt/unix/unix_plt_error.cpp
        src/plt/unix/unix_pages.cpp
        src/plt/unix/unix_fs.cpp
        src/plt/unix/unix_stacktrace.cpp
    )
elseif(OS STREQUAL "darwin")
    set(core_src ${core_src}
        src/plt/unix/unix_threading.cpp
        src/plt/unix/unix_time.cpp
        src/plt/unix/unix_plt_error.cpp
        src/plt/unix/unix_pages.cpp
        src/plt/unix/unix_fs.cpp
        src/plt/unix/unix_stacktrace.cpp
    )
elseif(OS STREQUAL "windows")
    set(core_src ${core_src}
        src/plt/win/win_threading.cpp
        src/plt/win/win_time.cpp
        src/plt/win/win_plt_error.cpp
        src/plt/win/win_pages.cpp
        src/plt/win/win_fs.cpp
        src/plt/win/win_stacktrace.cpp
    )
endif()

if(CMAKE_SYSTEM_PROCESSOR MATCHES "(x86)|(X86)|(amd64)|(AMD64)")
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        set(core_src ${core_src}
            src/asm/core_asm_x86_64_msvc.cpp
            src/asm/msvc_add_no_overflow.asm
            src/asm/msvc_sub_no_overflow.asm
            src/asm/msvc_mul_no_overflow.asm
        )

        # FIXME: Find a way to fix the problem of passing cpp msvc arguments to the asm compiler. This breaks the windows pipeline.
        set_source_files_properties(
            src/asm/msvc_add_no_overflow.asm
            src/asm/msvc_sub_no_overflow.asm
            src/asm/msvc_mul_no_overflow.asm
            PROPERTIES
            LANGUAGE ASM_MASM
        )
        enable_language(ASM_MASM)
        set(CMAKE_ASM_MASM_FLAGS "/nologo")
    else()
        set(core_src ${core_src}
            src/asm/core_asm_x86_64.cpp
        )
    endif()
endif()

# ---------------------------------------- End Declare Source Files ----------------------------------------------------

# Add library:

if(CORE_LIBRARY_SHARED)
    add_library(${target_core} SHARED ${core_src})
    # Set symbols visibility to be hidden by default. This is done to unify behavior across platforms.
    set_target_properties(${target_core} PROPERTIES
        C_VISIBILITY_PRESET hidden
        C_VISIBILITY_INLINE_PRESET hidden
        CXX_VISIBILITY_PRESET hidden
        CXX_VISIBILITY_INLINE_PRESET hidden
    )
else()
    add_library(${target_core} STATIC ${core_src})
endif()

# Set Configure File:

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/core_config.h.in core_config.h)

# Include Path:

target_include_directories(${target_core} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${PROJECT_BINARY_DIR} # for core_config.h
)

# Definitions:

target_compile_definitions(${target_core} PRIVATE
    "CORE_LIBRARY_SHARED=$<BOOL:${CORE_LIBRARY_SHARED}>" # turn CMake Boolean to 1 or 0
    -DCORE_LIBRARY_BUILD=1
)
target_compile_definitions(${target_core} PUBLIC
    "CORE_DEBUG=$<BOOL:${CORE_DEBUG}>"
    "CORE_ASSERT_ENABLED=$<BOOL:${CORE_ASSERT_ENABLED}>"
    "CORE_RUN_COMPILETIME_TESTS=$<BOOL:${CORE_RUN_COMPILETIME_TESTS}>"
)

# Set Default Flags:

core_target_set_default_flags(${target_core} ${CORE_DEBUG} ${CORE_SAVE_TEMPORARY_FILES})

if(CORE_BUILD_TESTS)
    # ------------------------------------- Begin Testing --------------------------------------------------------------
    log_info("Configuring tests")

    # Create test executable
    add_executable(${target_test}
        ${target_test}.cpp

        tests/t-index_core_init.cpp

        tests/t-algorithms.cpp
        tests/t-arr.cpp
        tests/t-bits.cpp
        tests/t-cmd_flag_parser.cpp
        tests/t-cstr_conv.cpp
        tests/t-cstr_conv_cstr_to_float.cpp
        tests/t-cstr_conv_float_to_cstr.cpp
        tests/t-cstr.cpp
        tests/t-defer.cpp
        tests/t-expected.cpp
        tests/t-hash_map.cpp
        tests/t-hash.cpp
        tests/t-intrinsics.cpp
        tests/t-ints.cpp
        tests/t-logger.cpp
        tests/t-mem.cpp
        tests/t-rnd.cpp
        tests/t-sarr.cpp
        tests/t-str_builder.cpp
        tests/t-tuple.cpp
        tests/t-unique_ptr.cpp
        tests/t-utf.cpp

        tests/allocators/t-bump_allocator.cpp
        tests/allocators/t-std_allocator.cpp
        tests/allocators/t-std_arena_allocator.cpp
        tests/allocators/t-std_stats_allocator.cpp

        tests/math/t-math.cpp
        tests/math/t-matrix.cpp
        tests/math/t-transforms.cpp
        tests/math/t-vec.cpp

        tests/plt/t-pages.cpp
        tests/plt/t-plt_err.cpp
        tests/plt/t-stacktrace.cpp
        tests/plt/t-threading.cpp
        tests/plt/t-time.cpp
        tests/plt/t-fs.cpp

        tests/asm/t-asm.cpp

        tests/common/ryu_reference_impl/s2f.cpp
        tests/common/ryu_reference_impl/s2d.cpp
        tests/common/ryu_reference_impl/f2s.cpp
        tests/common/ryu_reference_impl/d2s.cpp
        tests/common/ryu_reference_impl/t-verify.cpp
    )

    # Link core
    target_link_libraries(${target_test} PRIVATE ${target_core})

    # Definitions
    target_compile_definitions(${target_test} PRIVATE
        -DPATH_TO_TEST_DATA="${CMAKE_CURRENT_BINARY_DIR}"
        "CORE_TESTS_USE_ANSI=$<BOOL:${CORE_TESTS_USE_ANSI}>"
        "CORE_TESTS_STOP_ON_FIRST_FAILED=$<BOOL:${CORE_TESTS_STOP_ON_FIRST_FAILED}>"
    )

    core_target_set_default_flags(${target_test} ${CORE_DEBUG} ${CORE_SAVE_TEMPORARY_FILES})

    if(CORE_CODE_COVERAGE)
        if (CMAKE_COMPILER_IS_GNUCXX)
            include(CodeCoverage)
            append_coverage_compiler_flags_to_target(${target_test})
            append_coverage_compiler_flags_to_target(${target_core})
            setup_target_for_coverage_lcov(
                NAME ${target_test}_coverage # NOTE: make ${target_test}_coverage to run this
                EXECUTABLE ${target_test}
                DEPENDENCIES ${target_test}
            )
        else()
            log_warning("Code coverage is not supported for ${CMAKE_CXX_COMPILER_ID} compiler.")
        endif()
    endif()

    # Add the test
    enable_testing()
    add_test(NAME ${target_test} COMMAND ${target_test})

    # ------------------------------------- End Testing ----------------------------------------------------------------
endif()

log_info("---------------------------------------------")
