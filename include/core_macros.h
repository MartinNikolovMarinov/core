#pragma once

#include <core_API.h>
#include <core_types.h>
#include <core_system_checks.h>

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

// Zero cost defer:
#ifndef defer
    struct CORE_API_EXPORT deferDummy {};
    template <typename F> struct deferrer { F f; ~deferrer() { f(); } };
    template <typename F> deferrer<F> operator*(core::deferDummy, F f) { return {f}; }
    #define DEFER_(LINE) zz_defer##LINE
    #define DEFER(LINE) DEFER_(LINE)
    #define defer auto DEFER(__LINE__) = core::deferDummy{} *[&]()
#endif

// No copy macro:

#define NO_COPY(T) \
    T(const T&) = delete; \
    T& operator=(const T&) = delete

// No move macro:

#define NO_MOVE(T) \
    T(T&&) = delete; \
    T& operator=(T&&) = delete

// Disable mangling for a function:

#define NO_MANGLE extern "C"

// Concat macro:

#define CORE_NAME_CONCAT2(A, B) A##B
#define CORE_NAME_CONCAT(A, B) CORE_NAME_CONCAT2(A, B)

// Take the name of a function:

#define FN_NAME_TO_CPTR(test) #test

// No don't inline function

#if COMPILER_MSVC == 1
    #define CORE_NEVER_INLINE __declspec(noinline)
#elif COMPILER_GCC == 1 || COMPILER_CLANG == 1
    #define CORE_NEVER_INLINE __attribute__((__noinline__))
#else
    #define CORE_NEVER_INLINE
#endif

#define CORE_NO_INLINE

} // namespace core
