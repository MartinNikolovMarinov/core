#include <utils.h>

namespace core {
namespace {
global_assert_handler_ptr gAssertHandler = nullptr;
} // namespace

void                      set_global_assert_handler(global_assert_handler_ptr handler) { gAssertHandler = handler; }
global_assert_handler_ptr get_global_assert_handler()                                  { return gAssertHandler; }

} // namespace core
