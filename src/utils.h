#include "core.h"

namespace core
{

// General Macro Magic for matching different macros for different number of arguments:
#define __C_RSEQ_N__() 10,9,8,7,6,5,4,3,2,1,0
#define __C_ARG_N__(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N,...) N
#define __C_NARG_I__(...) __C_ARG_N__(__VA_ARGS__)
#define __C_NARG__(...)  __C_NARG_I__(__VA_ARGS__,__C_RSEQ_N__())
#define __C_VFUNC_A__(name, n) name##n
#define __C_VFUNC__(name, n) __C_VFUNC_A__(name, n)
#define C_VFUNC(func, ...) __C_VFUNC__(func, __C_NARG__(__VA_ARGS__)) (__VA_ARGS__)

// Customizeble global assert handler:
using GlobalAssertHandlerPtr = bool (*)(const char* failedExpr, const char* file, i32 line, const char* errMsg);
CORE_API_EXPORT void SetGlobalAssertHandler(GlobalAssertHandlerPtr handler);
CORE_API_EXPORT GlobalAssertHandlerPtr GetGlobalAssertHandler();

#ifndef Assert
    // This macro will dereference null to force a crash,
    // unless the global_assert_handler is set and returns false.
    #define Assert(...) C_VFUNC(Assert, __VA_ARGS__)
    #define Assert1(expr) Assert2(expr, "")
    #define Assert2(expr, msg)                                                                     \
        if (!(expr)) {                                                                             \
            if (core::GetGlobalAssertHandler()) {                                                  \
                bool shouldCrash = core::GetGlobalAssertHandler()(#expr, __FILE__, __LINE__, msg); \
                if (shouldCrash) *(volatile coretypes::i32 *)0 = 0;                                \
            } else {                                                                               \
                *(volatile coretypes::i32 *)0 = 0;                                                 \
            }                                                                                      \
        }
#endif

/* Zero cost defer */
#ifndef defer
    struct CORE_API_EXPORT deferDummy {};
    template <class F> struct deferrer { F f; ~deferrer() { f(); } };
    template <class F> deferrer<F> operator*(core::deferDummy, F f) { return {f}; }
    #define DEFER_(LINE) zz_defer##LINE
    #define DEFER(LINE) DEFER_(LINE)
    #define defer auto DEFER(__LINE__) = core::deferDummy{} *[&]()
#endif

} // namespace core
