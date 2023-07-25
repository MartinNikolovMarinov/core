#pragma once

#include <API.h>
#include <types.h>
#include <core_traits.h>

namespace core {

using namespace coretypes;

// General Macro Magic for matching different macros for different number of arguments:
#define __C_RSEQ_N__ 10,9,8,7,6,5,4,3,2,1,0
#define __C_ARG_N__(_1_, _2_, _3_, _4_, _5_, _6_, _7_, _8_, _9_, _10_, N, ...) N
#define __EXPAND_ARGS(args) __C_ARG_N__ args
#define __COUNT_ARGS_MAX10(...) __EXPAND_ARGS((__VA_ARGS__, __C_RSEQ_N__))
#define __OVERLOAD_MACRO2(name, count) name##count
#define __OVERLOAD_MACRO1(name, count) __OVERLOAD_MACRO2(name, count)
#define __OVERLOAD_MACRO(name, count) __OVERLOAD_MACRO1(name, count)

#define C_VFUNC(func, ...) __OVERLOAD_MACRO(func, __COUNT_ARGS_MAX10(__VA_ARGS__)) (__VA_ARGS__)

// Customizeble global assert handler:
using global_assert_handler_ptr = bool (*)(const char* failedExpr, const char* file, i32 line, const char* errMsg);
CORE_API_EXPORT void set_global_assert_handler(global_assert_handler_ptr handler);
CORE_API_EXPORT global_assert_handler_ptr get_global_assert_handler();

#if defined(CORE_ASSERT_ENABLED) && CORE_ASSERT_ENABLED
    #ifndef Assert
        // This macro will dereference null to force a crash,
        // unless the global assert handler is set and returns false.
        #define Assert(...) C_VFUNC(Assert, __VA_ARGS__)
        #define Assert1(expr) Assert2(expr, "")
        #define Assert2(expr, msg)                                                                        \
            if (!(expr)) {                                                                                \
                if (core::get_global_assert_handler()) {                                                  \
                    bool shouldCrash = core::get_global_assert_handler()(#expr, __FILE__, __LINE__, msg); \
                    if (shouldCrash) *(volatile coretypes::i32 *)0 = 0;                                   \
                } else {                                                                                  \
                    *(volatile coretypes::i32 *)0 = 0;                                                    \
                }                                                                                         \
            }
    #endif
#else
    #define Assert(...)
#endif

#ifndef Panic
    #define Panic(...) *(volatile coretypes::i32 *)0 = 0;
#endif

// Zero cost defer:
#ifndef defer
    struct CORE_API_EXPORT defer_dummy {};
    template <class F> struct deferrer { F f; ~deferrer() { f(); } };
    template <class F> deferrer<F> operator*(core::defer_dummy, F f) { return {f}; }
    #define DEFER_(LINE) zz_defer##LINE
    #define DEFER(LINE) DEFER_(LINE)
    #define defer auto DEFER(__LINE__) = core::defer_dummy{} *[&]()
#endif

// Move and forward implementations copied from the standard library:

template<typename T>
constexpr typename remove_ref<T>::type && move(T && arg) {
    return static_cast<typename remove_ref<T>::type &&>(arg);
}

template<typename T>
constexpr T&& forward(typename remove_ref<T>::type & arg) {
    return static_cast<T&&>(arg);
}

template<typename T>
constexpr T&& forward(typename remove_ref<T>::type && arg) {
    static_assert(!is_lvalue<T>::value, "invalid rvalue to lvalue conversion");
    return static_cast<T&&>(arg);
}

} // namespace core
