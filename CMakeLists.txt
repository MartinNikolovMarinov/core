cmake_minimum_required(VERSION 3.2)

set(executable_name core)
string(TOUPPER ${executable_name} executable_name_uppercase)

project(core VERSION 0.3.0)

# Standard Requirements:

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Generates compile_commands.json

# Download and update git submodules:

function(update_git_submodules)
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
        message(FATAL_ERROR "Git is not found. It is required to update submodules.")
    endif()
endfunction()

update_git_submodules()

# ---------------------------------------- Begin Options ---------------------------------------------------------------

if(NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "Release")
    set(${executable_name_uppercase}_DEBUG OFF)
else()
    set(${executable_name_uppercase}_DEBUG ON)
endif()

option(CORE_LIBRARY_SHARED "Build core as a shared library." OFF)
option(CORE_ASSERT_ENABLED "Enable asserts." OFF)
option(CORE_BUILD_TESTS "Build core tests." OFF)
option(CORE_BUILD_TESTS_USE_ANSI "Use ANSI escape codes in tests." OFF)
option(CORE_RUN_COMPILETIME_TESTS "Run compile-time tests." OFF)
option(CORE_USE_CPPCHECK "Use cppcheck. [WARNING] This makes compilation incredibly slow!" OFF)
option(CORE_CODE_COVERAGE "Enable code coverage." OFF)

if(CORE_USE_CPPCHECK)
    find_program(CMAKE_CXX_CPPCHECK NAMES cppcheck)
    if(CMAKE_CXX_CPPCHECK)

        # Generate a list of files in the include and src directories to use with cppcheck:

        set(fdfind_error)
        execute_process(
            COMMAND bash -c "fdfind . ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/src -e cpp -e h > CppCheckFilesToCheck.txt"
            WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
            ERROR_VARIABLE fdfind_error
        )

        if(NOT fdfind_error)

            # Configure cppcheck:

            message(STATUS "generated cpp check file list")
            list(APPEND CMAKE_CXX_CPPCHECK
                    "--enable=style,performance,portability"
                    "--inconclusive"
                    "--inline-suppr"
                    "--std=c++20"
                    "--report-progress"
                    "--check-level=exhaustive"
                    "--output-file=${CMAKE_BINARY_DIR}/cppcheck.out.txt"
                    "--suppressions-list=${CMAKE_SOURCE_DIR}/CppCheckSuppressions.txt"
                    "--file-list=${PROJECT_BINARY_DIR}/CppCheckFilesToCheck.txt")
        else()
            message(WARNING "failed to generate cpp check file list!")
        endif()

    else()
        message(WARNING "[CORE] cppcheck not found!")
    endif()

endif()

# Includes:

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

include(DetectOS)
include(CodeCoverage)
include(CompilerOptions)

# Print Selected Options:

message(STATUS "[CORE] ---------------------------------------------")
message(STATUS "[CORE] Version:           ${PROJECT_VERSION}")
message(STATUS "[CORE] Platform:          ${OS}")
message(STATUS "[CORE] Arch:              ${CMAKE_HOST_SYSTEM_PROCESSOR}") # IMPORTANT: Do not relly on CMAKE_HOST_SYSTEM_PROCESSOR for anything serious !
message(STATUS "[CORE] C++ Version:       ${CMAKE_CXX_STANDARD}")
message(STATUS "[CORE] Compiler:          ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "[CORE] Debug:             ${${executable_name_uppercase}_DEBUG}")
message(STATUS "[CORE] Assert:            ${CORE_ASSERT_ENABLED}")
message(STATUS "[CORE] Shared:            ${CORE_LIBRARY_SHARED}")
message(STATUS "[CORE] Build Tests:       ${CORE_BUILD_TESTS}")
message(STATUS "[CORE] Use ANSI in Tests: ${CORE_BUILD_TESTS_USE_ANSI}")
message(STATUS "[CORE] Run Compile Tests: ${CORE_RUN_COMPILETIME_TESTS}")
message(STATUS "[CORE] Code Coverage:     ${CORE_CODE_COVERAGE}")
message(STATUS "[CORE] Use CppCheck:      ${CORE_USE_CPPCHECK}")

# ---------------------------------------- End Options -----------------------------------------------------------------

# ---------------------------------------- Begin Declare Source Files --------------------------------------------------

set(src_files
    src/core_exec_ctx.cpp
    src/core_hash.cpp
    src/core_intrinsics.cpp
    src/core_mem.cpp
    src/core_rnd.cpp
    src/core_utils.cpp
    src/core_alloc.cpp

    src/allocators/bump_allocator.cpp
    src/allocators/std_allocator.cpp
    src/allocators/std_arena_allocator.cpp
    src/allocators/std_stats_allocator.cpp

    src/math/core_projections.cpp

    src/testing/testing_framework.cpp
)

if(OS STREQUAL "linux")
    set(src_files ${src_files}
        src/plt/unix/unix_threading.cpp
        src/plt/unix/unix_time.cpp
        src/plt/unix/unix_plt_error.cpp
        src/plt/unix/unix_pages.cpp
        # src/plt/unix/unix_fs.cpp
        src/plt/unix/unix_stacktrace.cpp
    )
elseif(OS STREQUAL "darwin")
    set(src_files ${src_files}
        src/plt/unix/unix_threading.cpp
        src/plt/unix/unix_time.cpp
        src/plt/unix/unix_plt_error.cpp
        src/plt/unix/unix_pages.cpp
        # src/plt/unix/unix_fs.cpp
        src/plt/unix/unix_stacktrace.cpp
    )
elseif(OS STREQUAL "windows")
    set(src_files ${src_files}
        src/plt/win/win_threading.cpp
        src/plt/win/win_time.cpp
        src/plt/win/win_plt_error.cpp
        # src/plt/win/win_pages.cpp
        # src/plt/win/win_fs.cpp
        src/plt/win/win_stacktrace.cpp
    )
endif()

# ---------------------------------------- End Declare Source Files ----------------------------------------------------

# Add library:

if(CORE_LIBRARY_SHARED)
    add_library(${executable_name} SHARED ${src_files})
    # Set symbols visibility to be hidden by default. This is done to unify behaviour across platforms.
    set_target_properties(${executable_name} PROPERTIES
        C_VISIBILITY_PRESET hidden
        C_VISIBILITY_INLINE_PRESET hidden
        CXX_VISIBILITY_PRESET hidden
        CXX_VISIBILITY_INLINE_PRESET hidden
    )
else()
    add_library(${executable_name} STATIC ${src_files})
endif()

# Set Configure File:

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/core_config.h.in core_config.h)

# Include Path:

target_include_directories(${executable_name} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${PROJECT_BINARY_DIR} # for core_config.h
)

# Definitions:

target_compile_definitions(${executable_name} PRIVATE
    "CORE_LIBRARY_SHARED=$<BOOL:${CORE_LIBRARY_SHARED}>" # turn CMake Boolean to 1 or 0
    -DCORE_LIBRARY_BUILD=1
)
target_compile_definitions(${executable_name} PUBLIC
    "${executable_name_uppercase}_DEBUG=$<BOOL:${${executable_name_uppercase}_DEBUG}>"
    "CORE_ASSERT_ENABLED=$<BOOL:${CORE_ASSERT_ENABLED}>"
    "CORE_RUN_COMPILETIME_TESTS=$<BOOL:${CORE_RUN_COMPILETIME_TESTS}>"
)

# ---------------------------------------- Begin Compile Options -------------------------------------------------------

macro(target_set_default_flags target)

    if(OS STREQUAL "linux")
        set(common_flags "-std=c++20" "-pthread")
        set(debug_flags "")
        set(release_flags "")
    elseif(OS STREQUAL "darwin")
        set(common_flags "-std=c++20" "-pthread")
        set(debug_flags "")
        set(release_flags "")
    elseif(OS STREQUAL "windows")
        set(common_flags "/std:c++20")
        set(debug_flags "")
        set(release_flags "")
    endif()

    generate_common_flags(
        common_flags "${common_flags}"
        debug_flags "${debug_flags}"
        release_flags "${release_flags}"
    )

    if(${executable_name_uppercase}_DEBUG)
        target_compile_options(${target} PRIVATE ${common_flags} ${debug_flags})
    else()
        target_compile_options(${target} PRIVATE ${common_flags} ${release_flags})
    endif()

endmacro()

if (${executable_name_uppercase}_DEBUG AND OS STREQUAL "windows")
    # DbgHelp.lib is required for stacktrace on Windows.
    target_link_options(${executable_name} PRIVATE /INCREMENTAL:NO /OPT:REF)
    target_link_libraries(${executable_name} PUBLIC DbgHelp.lib)
endif()

target_set_default_flags(${executable_name})

# ---------------------------------------- End Compile Options ---------------------------------------------------------

if(CORE_BUILD_TESTS)
    # ------------------------------------- Begin Testing --------------------------------------------------------------
    message(STATUS "[CORE] Configuring tests")

    set(test_executable ${executable_name}_test)
    # set(test_executable ${executable_name}_sandbox)

    # Create test executable
    add_executable(${test_executable}
        ${test_executable}.cpp

        tests/t-index_core_init.cpp

        # tests/t-algorithms.cpp
        tests/t-arr.cpp
        # tests/t-bits.cpp
        # tests/t-cmd_flag_parser.cpp
        # tests/t-cptr_conv.cpp
        # tests/t-cptr.cpp
        # tests/t-defer.cpp
        # tests/t-expected.cpp
        # tests/t-hash_map.cpp
        # tests/t-hash.cpp
        # tests/t-intrinsics.cpp
        # tests/t-ints.cpp
        # tests/t-mem.cpp
        tests/t-rnd.cpp
        # tests/t-sarr.cpp
        # tests/t-str_builder.cpp
        # tests/t-tuple.cpp
        # tests/t-unique_ptr.cpp
        # tests/t-utf.cpp

        tests/allocators/t-bump_allocator.cpp
        tests/allocators/t-std_allocator.cpp
        tests/allocators/t-std_arena_allocator.cpp
        tests/allocators/t-std_stats_allocator.cpp

        # tests/math/t-math.cpp
        # tests/math/t-matrix.cpp
        # tests/math/t-transforms.cpp
        # tests/math/t-vec.cpp

        # tests/plt/t-pages.cpp
        # tests/plt/t-plt_err.cpp
        # tests/plt/t-stacktrace.cpp
        # tests/plt/t-threading.cpp
        # tests/plt/t-time.cpp
        # tests/plt/t-fs.cpp
    )

    # Link core
    target_link_libraries(${test_executable} PRIVATE ${executable_name})

    # Definitions
    target_compile_definitions(${test_executable} PRIVATE
        -DPATH_TO_TEST_DATA="${CMAKE_CURRENT_BINARY_DIR}"
        "CORE_BUILD_TESTS_USE_ANSI=$<BOOL:${CORE_BUILD_TESTS_USE_ANSI}>"
    )

    target_set_default_flags(${test_executable})

    if(CORE_CODE_COVERAGE)
        if (CMAKE_COMPILER_IS_GNUCXX)
            include(CodeCoverage)
            append_coverage_compiler_flags_to_target(${test_executable})
            append_coverage_compiler_flags_to_target(${executable_name})
            setup_target_for_coverage_lcov(
                NAME ${test_executable}_coverage
                EXECUTABLE ${test_executable}
                DEPENDENCIES ${test_executable}
            )
        else()
            message(WARNING "[CORE] Code coverage is not supported for ${CMAKE_CXX_COMPILER_ID} compiler.")
        endif()
    endif()

    # Add the test
    enable_testing()
    add_test(NAME ${test_executable} COMMAND ${test_executable})

    # ------------------------------------- End Testing ----------------------------------------------------------------
endif()

message(STATUS "[CORE] ---------------------------------------------")
