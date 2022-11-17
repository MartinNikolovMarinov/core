#include <utils.h>

namespace core
{
namespace
{
GlobalAssertHandlerPtr gAssertHandler = nullptr;
} // namespace

void SetGlobalAssertHandler(GlobalAssertHandlerPtr handler) {
    gAssertHandler = handler;
}

GlobalAssertHandlerPtr GetGlobalAssertHandler() {
    return gAssertHandler;
}

} // namespace core
