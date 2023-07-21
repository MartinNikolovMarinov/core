cmake_minimum_required(VERSION 3.2)

project(core)

# BEGIN Options

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

message(STATUS "[CORE] ---------------------------------------------")
