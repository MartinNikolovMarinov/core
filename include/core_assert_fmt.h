#pragma once

#include <core_alloc.h>
#include <core_assert.h>
#include <core_cstr_format.h>

#include <cstdlib>

namespace core {

using namespace coretypes;

#if defined(CORE_ASSERT_ENABLED) && CORE_ASSERT_ENABLED
    #ifndef AssertFmt
        // This debug-only AssertFmt macro allocates a 3 KB message buffer when an assertion fails.
        // The program crashes immediately afterward, so freeing the memory is unnecessary.
        //
        // IMPORTANT: Do not use the core allocator here. This code must work even if the memory system is broken.
        // IMPORTANT: The format string must be valid. If it is not, the formatted message will be lost,
        //            and only the raw format string will appear as a fallback!
        #define AssertFmt(expr, fmt, ...)                                                                 \
            if (!(expr)) {                                                                                \
                if (core::getGlobalAssertHandler()) {                                                     \
                    constexpr addr_size ASSERT_MSG_MAX_SIZE = CORE_KILOBYTE * 3;                          \
                    char* msg = reinterpret_cast<char*>(std::malloc(ASSERT_MSG_MAX_SIZE * sizeof(char))); \
                    auto res = core::format(msg, ASSERT_MSG_MAX_SIZE, fmt, __VA_ARGS__);                  \
                    if (res.hasValue()) {                                                                 \
                        i32 n = res.value();                                                              \
                        msg[n] = '\0';                                                                    \
                        core::getGlobalAssertHandler()(#expr, __FILE__, __LINE__, __func__, msg);         \
                    }                                                                                     \
                    else {                                                                                \
                        core::getGlobalAssertHandler()(#expr, __FILE__, __LINE__, __func__, fmt);         \
                    }                                                                                     \
                }                                                                                         \
                else {                                                                                    \
                    volatile i32* __forceCrash = nullptr;                                                 \
                    [[maybe_unused]] i32 __ignored = *__forceCrash;                                       \
                }                                                                                         \
            }
    #endif
#else
    #ifndef AssertFmt
        #define AssertFmt(...)
    #endif
#endif

#ifndef PanicFmt
    #if defined(CORE_ASSERT_ENABLED) && CORE_ASSERT_ENABLED
        #define PanicFmt(...) AssertFmt(__VA_ARGS__)
    #else
        #define PanicFmt(expr, ...) Panic(expr)
    #endif
#endif

} // namespace core

