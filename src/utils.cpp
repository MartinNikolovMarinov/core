#include <utils.h>

namespace core {
namespace {
global_assert_handler_ptr g_AssertHandler = nullptr;
} // namespace

void                      set_global_assert_handler(global_assert_handler_ptr handler) { g_AssertHandler = handler; }
global_assert_handler_ptr get_global_assert_handler()                                  { return g_AssertHandler; }

} // namespace core
