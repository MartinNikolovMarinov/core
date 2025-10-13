#pragma once

#include <core_system_checks.h>

#if defined(COMPILER_MSVC) && COMPILER_MSVC == 1
    #define PRAGMA_WARNING_PUSH __pragma(warning(push))
    #define PRAGMA_WARNING_POP __pragma(warning(pop))
    #define DISABLE_MSVC_WARNING(w) __pragma(warning(disable : w))
    #define PRAGMA_COMPILER_MESSAGE(x) __pragma(message(#x))
    #define PRAGMA_WARNING_SUPPRESS_ALL \
            __pragma(warning(push, 0))
#endif

#if defined(COMPILER_GCC) && COMPILER_GCC == 1
    #define PRAGMA_WARNING_PUSH _Pragma("GCC diagnostic push")
    #define PRAGMA_WARNING_POP _Pragma("GCC diagnostic pop")
    #define _QUOTED_PRAGMA(x) _Pragma (#x)
    #define DISABLE_GCC_AND_CLANG_WARNING(w) _QUOTED_PRAGMA(GCC diagnostic ignored #w)
    #define DISABLE_GCC_WARNING(w) _QUOTED_PRAGMA(GCC diagnostic ignored #w)
    #define PRAGMA_COMPILER_MESSAGE(x) _QUOTED_PRAGMA(message #x)

    // TODO2: Unfortunately I did not find a better way to do this. Listing all warnings is time consumming so, for now,
    // I will just be adding what is currently necessary.
    #define PRAGMA_WARNING_SUPPRESS_ALL                             \
        _Pragma("GCC diagnostic push")                              \
        _Pragma("GCC diagnostic ignored \"-Wall\"")                 \
        _Pragma("GCC diagnostic ignored \"-Wextra\"")               \
        _Pragma("GCC diagnostic ignored \"-Wpedantic\"")            \
        _Pragma("GCC diagnostic ignored \"-Wconversion\"")          \
        _Pragma("GCC diagnostic ignored \"-Wsign-conversion\"")     \
        _Pragma("GCC diagnostic ignored \"-Wshadow\"")              \
        _Pragma("GCC diagnostic ignored \"-Wdouble-promotion\"")    \
        _Pragma("GCC diagnostic ignored \"-Wfloat-equal\"")         \
        _Pragma("GCC diagnostic ignored \"-Wformat=2\"")            \
        _Pragma("GCC diagnostic ignored \"-Wstrict-aliasing\"")     \
        _Pragma("GCC diagnostic ignored \"-Wold-style-cast\"")      \
        _Pragma("GCC diagnostic ignored \"-Wundef\"")               \
        _Pragma("GCC diagnostic ignored \"-Wnull-dereference\"")
#endif

#if defined(COMPILER_CLANG) && COMPILER_CLANG == 1
    #define PRAGMA_WARNING_PUSH _Pragma("clang diagnostic push")
    #define PRAGMA_WARNING_POP _Pragma("clang diagnostic pop")
    #define _QUOTED_PRAGMA(x) _Pragma (#x)
    #define DISABLE_GCC_AND_CLANG_WARNING(w) _QUOTED_PRAGMA(clang diagnostic ignored #w)
    #define DISABLE_CLANG_WARNING(w) _QUOTED_PRAGMA(clang diagnostic ignored #w)
    #define PRAGMA_COMPILER_MESSAGE(x) _QUOTED_PRAGMA(message #x)
    #define PRAGMA_WARNING_SUPPRESS_ALL \
        _Pragma("clang diagnostic ignored \"-Weverything\"")
#endif

#ifndef PRAGMA_WARNING_PUSH
    #define PRAGMA_WARNING_PUSH
#endif
#ifndef PRAGMA_WARNING_POP
    #define PRAGMA_WARNING_POP
#endif
#ifndef DISABLE_MSVC_WARNING
    #define DISABLE_MSVC_WARNING(...)
#endif
#ifndef DISABLE_GCC_AND_CLANG_WARNING
    #define DISABLE_GCC_AND_CLANG_WARNING(...)
#endif
#ifndef DISABLE_GCC_WARNING
    #define DISABLE_GCC_WARNING(...)
#endif
#ifndef DISABLE_CLANG_WARNING
    #define DISABLE_CLANG_WARNING(...)
#endif
#ifndef PRAGMA_COMPILER_MESSAGE
    #define PRAGMA_COMPILER_MESSAGE(...)
#endif
#ifndef PRAGMA_WARNING_SUPPRESS_ALL
    #define PRAGMA_WARNING_SUPPRESS_ALL
#endif
