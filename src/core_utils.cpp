#include <core_utils.h>

namespace core {

namespace {
GlobalAssertHandlerFn g_AssertHandler = nullptr;
} // namespace

void                   setGlobalAssertHandler(GlobalAssertHandlerFn handler) { g_AssertHandler = handler; }
GlobalAssertHandlerFn getGlobalAssertHandler()                               { return g_AssertHandler; }

} // namespace core
