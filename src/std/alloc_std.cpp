#include <std/alloc_std.h>

#include <cstdlib>

namespace core {

namespace {

std_allocator::on_oom_fp g_std_allocator_oom_cb = [](void*) { Assert(false, "ran out of memory"); };

}

void* std_allocator::alloc(ptr_size size) noexcept {
    void* ret = std::malloc(size);
    if (ret == nullptr) {
        if (g_std_allocator_oom_cb != nullptr) {
            g_std_allocator_oom_cb(nullptr);
        }
    }
    return ret;
}

void std_allocator::free(void* ptr) noexcept {
    std::free(ptr);
}

ptr_size std_allocator::used_mem() noexcept {
    return 0;
}

void std_allocator::on_out_of_memory(on_oom_fp cb) noexcept {
    g_std_allocator_oom_cb = cb;
}

}
