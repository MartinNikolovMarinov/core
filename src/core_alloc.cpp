#include <core_alloc.h>

namespace core {

namespace {

void defaultOOMHandler() {
    Panic(false, "Out of memory!");
}

} // namespace

OOMHandlerFn getDefaultOOMHandler() {
    return defaultOOMHandler;
}

} // namespace core
