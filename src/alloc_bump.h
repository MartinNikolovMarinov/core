#pragma once

#include <API.h>
#include <types.h>
#include <utils.h>
#include <mem.h>

namespace core {

using namespace coretypes;

template<ptr_size TCap>
struct CORE_API_EXPORT bump_allocator {
    static_assert(TCap > 1, "Capacity must be greater than 1");

    using on_oom_fp = void(*)(void* userData);

    static constexpr ptr_size maxCap = TCap;
    static constexpr on_oom_fp defaultOOMfp = [](void*) { Assert(false, "ran out of memory"); };

    static constexpr const char* allocator_name() { return "bump allocator"; }

    constexpr bump_allocator(on_oom_fp cb = defaultOOMfp) : m_oom_cb(cb) {
        clear();
    }

    // no copy
    bump_allocator(const bump_allocator&) = delete;
    bump_allocator& operator=(const bump_allocator&) = delete;

    // no move
    bump_allocator(bump_allocator&&) = delete;
    bump_allocator& operator=(bump_allocator&&) = delete;

    constexpr void* alloc(ptr_size size) noexcept {
        size = core::align(size);
        if (m_used + size > maxCap) {
            m_oom_cb(nullptr);
            return nullptr;
        }
        void* p = reinterpret_cast<void*>(&m_data[m_used]);
        m_used += size;
        return p;
    }

    constexpr  void free(void*) noexcept { /* does nothing */ }

    constexpr void clear() noexcept {
        m_startAddr = &m_data[0];
        m_used = 0;
    }

    constexpr ptr_size used_mem() noexcept { return m_used; }

    u8 m_data[maxCap];
    const u8* m_startAddr;
    ptr_size m_used; // in bytes
    const on_oom_fp m_oom_cb;
};

}
