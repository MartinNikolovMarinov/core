#pragma once

#if defined(__clang__)
    #define COMPILER_CLANG 1
#else
    #define COMPILER_CLANG 0
#endif
#if (defined(__GNUC__) || defined(__GNUG__)) && COMPILER_CLANG == 0
    #define COMPILER_GCC 1
#else
    #define COMPILER_GCC 0
#endif
#if defined(_MSC_VER)
    #define COMPILER_MSVC 1
#else
    #define COMPILER_MSVC 0
#endif
#if COMPILER_CLANG == 0 && COMPILER_GCC == 0 && COMPILER_MSVC == 0
    #define COMPILER_UNKNOWN 1
#else
    #define COMPILER_UNKNOWN 0
#endif

#if defined(_WIN32) || defined(_WIN64) || defined(__MINGW32__) || defined(__MINGW64__)
    #define OS_WIN 1
#else
    #define OS_WIN 0
#endif
#if defined(__linux__)
    #define OS_LINUX 1
#else
    #define OS_LINUX 0
#endif
#if defined(__APPLE__) || defined(__MACH__)
    #define OS_UNIX 1
#else
    #define OS_UNIX 0
#endif
#if OS_WIN == 0 && OS_LINUX == 0 && OS_UNIX == 0
    #define OS_UNKNOWN 1
#else
    #define OS_UNKNOWN 0
#endif
