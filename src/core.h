#pragma once

#include "API.h"
#include "types.h"

namespace core {

using namespace coretypes;

#pragma region System Checks

#if defined(__clang__)
    #ifndef COMPILER_CLANG
        #define COMPILER_CLANG 1
    #endif
#elif defined(__GNUC__) || defined(__GNUG__)
    #ifndef COMPILER_GCC
        #define COMPILER_GCC 1
    #endif
#elif defined(_MSC_VER)
    #ifndef COMPILER_MSVC
        #define COMPILER_MSVC 1
    #endif
#else
    #ifndef COMPILER_UNKNOWN
        #define COMPILER_UNKNOWN 1
    #endif
#endif

#if defined(_WIN32) || defined(_WIN64)
    #ifndef OS_WIN32
        #define OS_WIN32 1
    #endif
#elif defined(__linux__)
    #ifndef OS_LINUX
        #define OS_LINUX 1
    #endif
#elif defined(__APPLE__) || defined(__MACH__)
    #ifndef OS_UNIX
        #define OS_UNIX 1
    #endif
#else
    #ifndef OS_UNKNOWN
        #define OS_UNKNOWN 1
    #endif
#endif

#pragma endregion System Checks

#pragma region Utils

// General Macro Magic for matching different macros for different number of arguments:
#define __C_RSEQ_N__() 10,9,8,7,6,5,4,3,2,1,0
#define __C_ARG_N__(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N,...) N
#define __C_NARG_I__(...) __C_ARG_N__(__VA_ARGS__)
#define __C_NARG__(...)  __C_NARG_I__(__VA_ARGS__,__C_RSEQ_N__())
#define __C_VFUNC_A__(name, n) name##n
#define __C_VFUNC__(name, n) __C_VFUNC_A__(name, n)
#define C_VFUNC(func, ...) __C_VFUNC__(func, __C_NARG__(__VA_ARGS__)) (__VA_ARGS__)

// Customizeble global assert handler function pointer:
static bool(*global_assert_handler)(
    const char* failedExpr,
    const char* file, i32 line,
    const char* errMsg) = nullptr;

#ifndef Assert
    // This macro will dereference null to force a crash,
    // unless the global_assert_handler is set and returns false.
    #define Assert(...) C_VFUNC(Assert, __VA_ARGS__)
    #define Assert1(expr) Assert2(expr, "")
    #define Assert2(expr, msg)                                                                  \
        if (!(expr)) {                                                                          \
            if (core::global_assert_handler) {                                                  \
                bool shouldCrash = core::global_assert_handler(#expr, __FILE__, __LINE__, msg); \
                if (shouldCrash) *(volatile i32 *)0 = 0;                                        \
            } else {                                                                            \
                *(volatile i32 *)0 = 0;                                                         \
            }                                                                                   \
        }
#endif

#pragma endregion Utils

#pragma region Bits

// Least Significant N Bits are equal to bitSeq
CORE_API_EXPORT bool LSNBits(u8 v, u8 bitSeq, u8 n);
// Most Significant N Bits are equal to bitSeq
CORE_API_EXPORT bool MSNBits(u8 v, u8 bitSeq, u8 n);
template <typename TUint> CORE_API_EXPORT TUint SwapByteOrder(TUint n);
template <typename TFloat> CORE_API_EXPORT void FloatToBin(u8 bytes[sizeof(TFloat)], TFloat v);

#pragma endregion Bits

#pragma region Tuple

template <typename...> struct CORE_API_EXPORT Tuple;
template <typename T1, typename T2> struct Tuple<T1, T2> { T1 first; T2 second; };
template <typename T1, typename T2, typename T3> struct Tuple<T1, T2, T3> { T1 first; T2 second; T3 third; };
template <typename T1, typename T2, typename T3, typename T4> struct Tuple<T1, T2, T3, T4> { T1 first; T2 second; T3 third; T4 fourth; };

#pragma endregion Tuple

#pragma region Intrinsics

template<typename TUint> u32 LeadingZeros(TUint n);
u64 GetCPUTicks();

#pragma endregion Intrinsics

#pragma region Math

i64 PowI64(i64 n, i32 p);

#pragma endregion Math

#pragma region Integers

CORE_API_EXPORT bool IsInRange(u32 min, u32 n, u32 max);
CORE_API_EXPORT bool IsInRangeInc(u32 min, u32 n, u32 max);
CORE_API_EXPORT bool IsInRange(i32 min, i32 n, i32 max);
CORE_API_EXPORT bool IsInRangeInc(i32 min, i32 n, i32 max);

template <typename TUint> CORE_API_EXPORT TUint DigitCount(TUint n);

#pragma endregion Integers

#pragma region Memory

CORE_API_EXPORT void MemCopy(void *dest, const void *src, ptr_size len);

#pragma endregion Memory

#pragma region Char Pointer

template<typename UInt>
CORE_API_EXPORT UInt     DigitToCh(UInt digit);
template<typename TInt>
CORE_API_EXPORT void     IntToCptr(TInt n, char* out);
CORE_API_EXPORT ptr_size CptrLen(const char* p);
CORE_API_EXPORT i32      CptrCmp(const char* a, const char* b);
CORE_API_EXPORT i32      CptrCmp(const uchar* a, const uchar* b);
CORE_API_EXPORT void     CptrCpy(const char *src, ptr_size n, char *dst);
CORE_API_EXPORT u64      CptrIndexOf(const char *src, ptr_size srcLen, const char *val, ptr_size valLen);
CORE_API_EXPORT u64      CptrIndexOfCh(const char *src, ptr_size srcLen, char val);

#pragma endregion Char Pointer

#pragma region Error

template<i32 CAP = 254>
struct CORE_API_EXPORT Error {
    Error() : errLen(0) {}
    template<i32 N>
    explicit Error(const char (&msg)[N]) { Reset(msg); }
    explicit Error(const char* msg, ptr_size len) { Reset(msg, len); }

    template<i32 N>
    void Reset(const char (&msg)[N]) { Reset(msg, N); }
    void Reset(const char* msg, ptr_size len) {
        Assert(len < CAP, "provided len is larger than the errMsg buffer capacity!");
        MemCopy(errMsg, msg, len);
        errMsg[len] = '\0';
        errLen = len + 1;
    }

    bool HasError() { return errLen > 0; }
    Tuple<const char*, i32> Err() { return {errMsg, errLen}; }

private:
    char errMsg[CAP + 1];
    i32 errLen;
};

#define TryOrReturn(expr)                   \
    {                                       \
        auto err = (expr);                  \
        if (err.HasError()) { return ret; } \
    }

#define TryOrFail(expr)                               \
    {                                                 \
        auto err = (expr);                            \
        if (err.HasError()) {                         \
            Assert(!err.HasError(), err.Err().first); \
        }                                             \
    }

#pragma endregion Error

#pragma region Random

/**
 * Initializes the global state needed for pseudo-random number generation.
 * By default the initial seed is based on system clock time.
 *
 * The algorithm/s chosen will prioritize speed above security, therefore:
 * - These functions should not be used for applications that need a
 *   cryptographically-secure random number generation.
 * - The random number generation is not thread safe.
 * - None of the provided parameters, in any of the functions, will
 *   be checked for validity. If, for example, a min value is bigger
 *   that max value the behaviour is undefined.
*/
CORE_API_EXPORT void RandInit();
CORE_API_EXPORT void RandInit(u64 seed);
CORE_API_EXPORT u32  RandU32();
CORE_API_EXPORT u32  RandU32(u32 min, u32 max);
CORE_API_EXPORT u64  RandU64();
CORE_API_EXPORT u64  RandU64(u64 min, u64 max);
CORE_API_EXPORT i32  RandI32();
CORE_API_EXPORT i32  RandI32(i32 min, i32 max);
CORE_API_EXPORT i64  RandI64();
CORE_API_EXPORT i64  RandI64(i64 min, i64 max);
CORE_API_EXPORT void RandCptr(char* out, ptr_size len);

#pragma endregion Random

#pragma region UTF

bool IsValidUTF8Encoding(const uchar* utf, i32 len);
Tuple<rune, bool> RuneFromBytes(const uchar* utf, i32 len);
Tuple<i32, bool> RuneToBytes(const rune rune, uchar* utf);

#pragma endregion UTF
} // namespace core
