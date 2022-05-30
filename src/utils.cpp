#include "core.h"

namespace core
{
namespace
{
static GlobalAssertHandlerPtr global_assert_handler = nullptr;
} // namespace

void SetGlobalAssertHandler(GlobalAssertHandlerPtr handler) {
    global_assert_handler = handler;
}

GlobalAssertHandlerPtr GetGlobalAssertHandler() {
    return global_assert_handler;
}

} // namespace core
