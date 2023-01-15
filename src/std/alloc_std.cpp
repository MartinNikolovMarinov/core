#include <std/alloc_std.h>

#include <cstdlib>

namespace core {

void* std_allocator::alloc(ptr_size size) noexcept {
    void* ret = std::malloc(size);
    if (ret == nullptr && m_oomCb != nullptr) m_oomCb(nullptr);
    return ret;
}

void std_allocator::free(void* ptr) noexcept {
    std::free(ptr);
}

ptr_size std_allocator::used_mem() noexcept {
    return 0;
}

}
