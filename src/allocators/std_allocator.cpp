#include <core_alloc.h>

#include <cstdlib>

namespace core {

StdAllocator::StdAllocator() : oomHandler(defaultOOMHandler) {}

void* StdAllocator::alloc(addr_size count, addr_size size) {
    Panic(count > 0 && size > 0, "Invalid allocation size");

    void* ret = std::malloc(count * size);
    if (ret == nullptr && oomHandler) {
        oomHandler();
        return nullptr;
    }
    return ret;
}

void* StdAllocator::calloc(addr_size count, addr_size size) {
    Panic(count > 0 && size > 0, "Invalid zero allocation size");

    void* ret = std::calloc(count, size);
    if (ret == nullptr && oomHandler) {
        oomHandler();
        return nullptr;
    }
    return ret;
}

void StdAllocator::free(void* ptr, addr_size count, addr_size size) {
    Panic(count > 0 && size > 0, "Invalid free size"); // Check this always.

    std::free(ptr);
}

void StdAllocator::clear() {}
addr_size StdAllocator::totalMemoryAllocated() { return 0; }
addr_size StdAllocator::inUseMemory() { return 0; }

} // namespace core
