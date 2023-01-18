#pragma once

#include <API.h>
#include <types.h>

#pragma once

namespace core {

using namespace coretypes;

// General Macro Magic for matching different macros for different number of arguments:
#define __C_RSEQ_N__() 10,9,8,7,6,5,4,3,2,1,0
#define __C_ARG_N__(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N,...) N
#define __C_NARG_I__(...) __C_ARG_N__(__VA_ARGS__)
#define __C_NARG__(...) __C_NARG_I__(__VA_ARGS__,__C_RSEQ_N__())
#define __C_VFUNC_A__(name, n) name##n
#define __C_VFUNC__(name, n) __C_VFUNC_A__(name, n)
#define C_VFUNC(func, ...) __C_VFUNC__(func, __C_NARG__(__VA_ARGS__)) (__VA_ARGS__)

// Customizeble global assert handler:
using global_assert_handler_ptr = bool (*)(const char* failedExpr, const char* file, i32 line, const char* errMsg);
CORE_API_EXPORT void set_global_assert_handler(global_assert_handler_ptr handler);
CORE_API_EXPORT global_assert_handler_ptr get_global_assert_handler();

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

template<typename T> struct CORE_API_EXPORT remove_ref      { typedef T type; };
template<typename T> struct CORE_API_EXPORT remove_ref<T&>  { typedef T type; };
template<typename T> struct CORE_API_EXPORT remove_ref<T&&> { typedef T type; };

template<typename T>
CORE_API_EXPORT constexpr typename remove_ref<T>::type && move(T && arg) {
    return static_cast<typename remove_ref<T>::type &&>(arg);
}

template<typename T> struct CORE_API_EXPORT is_L_value     { static constexpr bool value = false; };
template<typename T> struct CORE_API_EXPORT is_L_value<T&> { static constexpr bool value = true; };

template<typename T>
constexpr CORE_API_EXPORT T&& forward(typename remove_ref<T>::type & arg) {
    return static_cast<T&&>(arg);
}

template<typename T>
constexpr CORE_API_EXPORT T&& forward(typename remove_ref<T>::type && arg) {
    static_assert(!is_L_value<T>::value, "invalid rvalue to lvalue conversion");
    return static_cast<T&&>(arg);
}

} // namespace core
