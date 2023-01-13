#include <std/alloc_std.h>

#include <cstdlib>

namespace core {

namespace {

struct std_allocator_state {
    // TODO: Add optional stats gathering here.
    ptr_size usedMem = 0;
};

std_allocator_state g_std_allocator_state;

}

void* std_allocator::alloc(ptr_size size) noexcept {
    return std::malloc(size);
}

void std_allocator::free(void* ptr) noexcept {
    std::free(ptr);
}

ptr_size std_allocator::used_mem() noexcept {
    return g_std_allocator_state.usedMem;
}

}
