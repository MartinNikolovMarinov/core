#pragma once

#include <core_API.h>
#include <core_macros.h>
#include <core_types.h>

namespace core {

using namespace coretypes;

// Customizeble global assert handler:
using GlobalAssertHandlerFn = void (*)(const char* failedExpr, const char* file, i32 line, const char* funcName, const char* errMsg);
CORE_API_EXPORT void setGlobalAssertHandler(GlobalAssertHandlerFn handler);
CORE_API_EXPORT GlobalAssertHandlerFn getGlobalAssertHandler();
CORE_API_EXPORT void forceCrash();

#if defined(CORE_ASSERT_ENABLED) && CORE_ASSERT_ENABLED
    #ifndef Assert
        // This macro will dereference null to force a crash,
        // unless the global assert handler is set and returns false,
        // or exists the program in some other way.
        #define Assert(...) C_VFUNC(Assert, __VA_ARGS__)
        #define Assert1(expr) Assert2(expr, "")
        #define Assert2(expr, msg)                                                            \
            if (!(expr)) {                                                                    \
                if (core::getGlobalAssertHandler()) {                                         \
                    core::getGlobalAssertHandler()(#expr, __FILE__, __LINE__, __func__, msg); \
                }                                                                             \
                else {                                                                        \
                    core::forceCrash();                                                       \
                }                                                                             \
                                                                                              \
                CORE_UNREACHABLE;                                                             \
            }
    #endif
#else
    #ifndef Assert
        #define Assert(...)
    #endif
#endif

#ifndef Panic
    #if defined(CORE_ASSERT_ENABLED) && CORE_ASSERT_ENABLED
        #define Panic(...) C_VFUNC(Panic, __VA_ARGS__)
        #define Panic1(expr) Assert1(expr)
        #define Panic2(expr, msg) Assert2(expr, msg)
    #else
        #define Panic(...) C_VFUNC(Panic, __VA_ARGS__)
        #define Panic1(expr) Panic2(expr, "")
        #define Panic2(expr, msg)                               \
            if (!(expr)) {                                      \
                core::forceCrash();                             \
                CORE_UNREACHABLE;                               \
            }
    #endif
#endif

} // namespace core
