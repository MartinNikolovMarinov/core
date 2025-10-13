#include "core_compiler.h"
#include <core_assert.h>

namespace core {

namespace {
GlobalAssertHandlerFn g_AssertHandler = nullptr;
} // namespace

void                  setGlobalAssertHandler(GlobalAssertHandlerFn handler) { g_AssertHandler = handler; }
GlobalAssertHandlerFn getGlobalAssertHandler()                              { return g_AssertHandler; }

void forceCrash() {
    PRAGMA_WARNING_PUSH
    DISABLE_GCC_AND_CLANG_WARNING(-Wnull-dereference) // This is the entire point of this function.

    volatile i32* __forceCrash = nullptr;
    [[maybe_unused]] i32 __ignored = *__forceCrash;

    PRAGMA_WARNING_POP
}

} // namespace core
