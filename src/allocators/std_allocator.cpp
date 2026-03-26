#include <core_alloc.h>
#include <core_assert.h>

#include <cstdlib>

namespace core {

StdAllocator::StdAllocator() : oomHandler(getDefaultOOMHandler()) {}

void* StdAllocator::alloc(addr_size count, addr_size size) {
    Assert(count > 0 && size > 0, "Invalid Arguments");

    void* ret = std::malloc(count * size);
    if (ret == nullptr && oomHandler) {
        oomHandler();
        return nullptr;
    }
    return ret;
}

void* StdAllocator::calloc(addr_size count, addr_size size) {
    Assert(count > 0 && size > 0, "Invalid Arguments");

    void* ret = std::calloc(count, size);
    if (ret == nullptr && oomHandler) {
        oomHandler();
        return nullptr;
    }
    return ret;
}

void StdAllocator::free(void* ptr, addr_size, addr_size) {
    std::free(ptr);
}

void* StdAllocator::realloc(void* ptr, addr_size newCount, addr_size newSize, addr_size, addr_size) {
    Assert(newCount > 0 && newSize > 0, "Invalid Argument");

    void* ret = std::realloc(ptr, newCount * newSize);
    if (ret == nullptr && oomHandler) {
        oomHandler();
        return nullptr;
    }
    return ret;
}

void StdAllocator::clear() {}
addr_size StdAllocator::totalMemoryAllocated() { return 0; }
addr_size StdAllocator::inUseMemory() { return 0; }

} // namespace core
