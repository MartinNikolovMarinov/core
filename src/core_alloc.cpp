#include <core_alloc.h>

namespace core {

void defaultOOMHandler() {
    Panic(false, "Out of memory!");
};

} // namespace core
