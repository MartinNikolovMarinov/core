#pragma once

#include <API.h>
#include <types.h>
#include <utils.h>

namespace core::alloc
{

using namespace coretypes;

template<ptr_size TCap>
struct CORE_API_EXPORT StaticBumpAllocator {
    static_assert(TCap > 1, "Capacity must be greater than 1");

    static constexpr ptr_size Cap = TCap;

    constexpr StaticBumpAllocator() noexcept {
        m_startAddr = &m_data[0];
        m_used = 0;
    }
    // no copy
    StaticBumpAllocator(const StaticBumpAllocator&) = delete;
    StaticBumpAllocator& operator=(const StaticBumpAllocator&) = delete;
    // no move
    StaticBumpAllocator(StaticBumpAllocator&&) = delete;
    StaticBumpAllocator& operator=(StaticBumpAllocator&&) = delete;

    void* Alloc(ptr_size size) noexcept {
        size = core::Align(size);
        if (m_used + size > Cap) return nullptr; // OOM
        void* p = reinterpret_cast<void*>(&m_data[m_used]);
        m_used += size;
        return p;
    }

    constexpr void Free(void*) noexcept {}

    template<typename T, typename ...Args>
    constexpr T* Construct(T* out, Args... args) noexcept {
        void* p = Alloc(sizeof(T));
        if (p != nullptr) out = new (p) T(core::Forward<Args>(args)...);
        return out;
    }

    constexpr void Clear() noexcept {
        core::MemSet(m_data, 0, Cap);
        m_used = 0;
    }

    constexpr ptr_size UsedMem() noexcept { return m_used; }

private:
    u8 m_data[Cap];
    const u8* m_startAddr;
    ptr_size m_used; // in bytes
};

} // namespace core::alloc
