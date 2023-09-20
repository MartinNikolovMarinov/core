#pragma once

#include <API.h>
#include <types.h>
#include <utils.h>

namespace core {

using namespace coretypes;

namespace details { struct allocated_block_list; }

struct CORE_API_EXPORT std_stats_allocator {
    using on_oom_fp = void(*)(void* userData);

    static constexpr on_oom_fp defaultOOMfp = [](void*) { Assert(false, "ran out of memory"); };

    static constexpr const char* allocator_name() { return "standard library allocator"; }

    std_stats_allocator(on_oom_fp cb = defaultOOMfp);
    ~std_stats_allocator();

    // no copy
    std_stats_allocator(const std_stats_allocator&) = delete;
    std_stats_allocator& operator=(const std_stats_allocator&) = delete;

    // no move
    std_stats_allocator(std_stats_allocator&&) = delete;
    std_stats_allocator& operator=(std_stats_allocator&&) = delete;

    void* alloc(addr_size size) noexcept;

    void* calloc(addr_size count, addr_size size) noexcept;

    void free(void* ptr) noexcept;

    void clear() noexcept;

    addr_size used_mem() noexcept;

    const on_oom_fp m_oomCb;
    details::allocated_block_list* m_allocatedBlocks;
};

}
