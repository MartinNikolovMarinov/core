#pragma once

#include <core_alloc.h>
#include <core_assert.h>
#include <core_cstr_format.h>

namespace core {

using namespace coretypes;

// In this debug-only AssertFmt macro, a 3 KB message buffer is dynamically allocated when an assertion fails. Since the
// program is designed to crash immediately after the assertion, explicitly freeing this memory is unnecessary.

#if defined(CORE_ASSERT_ENABLED) && CORE_ASSERT_ENABLED
    #ifndef AssertFmt
        #define AssertFmt(expr, fmt, ...)                                                              \
            if (!(expr)) {                                                                             \
                if (core::getGlobalAssertHandler()) {                                                  \
                    constexpr addr_size ASSERT_MSG_MAX_SIZE = CORE_KILOBYTE * 3;                       \
                    char* msg = reinterpret_cast<char*>(core::getAllocator(core::DEFAULT_ALLOCATOR_ID) \
                        .alloc(ASSERT_MSG_MAX_SIZE, sizeof(char)));                                    \
                    i32 n = core::Unpack(core::format(msg, ASSERT_MSG_MAX_SIZE, fmt, __VA_ARGS__));    \
                    msg[n] = '\0';                                                                     \
                    core::getGlobalAssertHandler()(#expr, __FILE__, __LINE__, __func__, msg);          \
                }                                                                                      \
                else {                                                                                 \
                    volatile i32* __forceCrash = nullptr;                                              \
                    [[maybe_unused]] i32 __ignored = *__forceCrash;                                    \
                }                                                                                      \
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

