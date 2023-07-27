#pragma once

#include <API.h>
#include <types.h>
#include <utils.h>

namespace core {

using namespace coretypes;

struct CORE_API_EXPORT std_allocator {
    using on_oom_fp = void(*)(void* userData);

    static constexpr on_oom_fp defaultOOMfp = [](void*) { Assert(false, "ran out of memory"); };

    static constexpr const char* allocator_name() { return "standard library allocator"; }

    std_allocator(on_oom_fp cb = defaultOOMfp) : m_oomCb(cb) {};

    // no copy
    std_allocator(const std_allocator&) = delete;
    std_allocator& operator=(const std_allocator&) = delete;

    // no move
    std_allocator(std_allocator&&) = delete;
    std_allocator& operator=(std_allocator&&) = delete;

    void* alloc(ptr_size size) noexcept;

    void free(void* ptr) noexcept;

    ptr_size used_mem() noexcept;

    const on_oom_fp m_oomCb;
};

}
