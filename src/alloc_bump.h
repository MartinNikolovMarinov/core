#pragma once

#include <types.h>
#include <utils.h>
#include <mem.h>

namespace core {

using namespace coretypes;

template<addr_size TCap>
struct bump_allocator {
    static_assert(TCap > 1, "Capacity must be greater than 1");

    using on_oom_fp = void(*)(void* userData);
    using size_type = addr_size;

    static constexpr size_type maxCap = TCap;
    static constexpr on_oom_fp defaultOOMfp = [](void*) { Assert(false, "ran out of memory"); };

    static constexpr const char* allocator_name() { return "bump allocator"; }

    constexpr bump_allocator(on_oom_fp cb = defaultOOMfp) noexcept : m_oomCb(cb) {
        clear();
    }

    // no copy
    bump_allocator(const bump_allocator&) = delete;
    bump_allocator& operator=(const bump_allocator&) = delete;

    // no move
    bump_allocator(bump_allocator&&) = delete;
    bump_allocator& operator=(bump_allocator&&) = delete;

    constexpr void* alloc(size_type size) noexcept {
        size = core::align(size);
        if (m_used + size > maxCap) {
            m_oomCb(nullptr);
            return nullptr;
        }
        void* p = reinterpret_cast<void*>(&m_data[m_used]);
        m_used += size;
        return p;
    }

    constexpr void* calloc(size_type count, size_type size) noexcept {
        size_type totalSize = count * size;
        void* p = alloc(totalSize);
        if (p == nullptr) return nullptr;
        core::memset(p, 0, totalSize);
        return p;
    }

    constexpr void free(void*) noexcept { /* does nothing */ }

    constexpr void clear() noexcept {
        m_startAddr = &m_data[0];
        m_used = 0;
    }

    constexpr size_type used_mem() noexcept { return m_used; }

    u8 m_data[maxCap];
    const u8* m_startAddr;
    size_type m_used; // in bytes
    const on_oom_fp m_oomCb;
};

}
