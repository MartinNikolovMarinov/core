#pragma once

#include <API.h>
#include "types.h"

namespace core::alloc
{

using namespace coretypes;

template<ptr_size TCap>
struct CORE_API_EXPORT StaticBumpAllocator {
    static_assert(TCap > 1, "Capacity must be greater than 1");

    static constexpr ptr_size Cap = TCap;

    StaticBumpAllocator() {
        m_startAddr = &m_data[0];
        m_used = 0;
    }
    // no copy
    StaticBumpAllocator(const StaticBumpAllocator&) = delete;
    StaticBumpAllocator& operator=(const StaticBumpAllocator&) = delete;
    // no move
    StaticBumpAllocator(StaticBumpAllocator&&) = delete;
    StaticBumpAllocator& operator=(StaticBumpAllocator&&) = delete;

    void* Alloc(ptr_size size) {
        size = core::Align(size);
        if (m_used + size > Cap) return nullptr; // OOM
        void* p = reinterpret_cast<void*>(&m_data[m_used]);
        m_used += size;
        return p;
    }

    void Free(void*) {}

    template<typename T, typename ...Args>
    T* Construct(T* out, Args... args) {
        void* p = Alloc(sizeof(T));
        if (p != nullptr) out = new (p) T(std::forward<Args>(args)...);
        return out;
    }

    void Clear() {
        core::MemSet(m_data, 0, Cap);
        m_used = 0;
    }

    ptr_size UsedMem() {
        return m_used;
    }

private:
    u8 m_data[Cap];
    const u8* m_startAddr;
    ptr_size m_used; // in bytes
};

} // namespace core::alloc
