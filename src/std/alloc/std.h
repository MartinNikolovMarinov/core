#pragma once

#include <API.h>
#include <types.h>
#include <utils.h>

#include <cstdlib>

namespace core::alloc
{

using namespace coretypes;

namespace detail {

// NOTE: intentianally not exported.
struct AllocedBlock {
    void* addr;
    AllocedBlock* next;
    ptr_size size;
};

// NOTE: intentianally not exported.
struct AllocedBlockList {

    AllocedBlock* Head() noexcept { return m_head; }
    AllocedBlock* Tail() noexcept { return m_tail; }
    ptr_size Used() noexcept { return m_used; }

    void AddBlock(AllocedBlock* block) noexcept {
        if (m_head == nullptr) {
            m_head = block;
            m_tail = block;
            block->next = nullptr;
        } else {
            m_tail->next = block;
            m_tail = block;
            block->next = nullptr;
        }

        m_used += block->size;
    }

    void RemoveBlock(void* addr) noexcept {
        if (m_head == nullptr) return;

        AllocedBlock* block = nullptr;
        if (m_head->addr == addr) {
            block = m_head;
            m_head = m_head->next;
            if (m_head == nullptr) m_tail = nullptr;
        } else {
            AllocedBlock* prev = m_head;
            block = m_head->next;
            while (block != nullptr) {
                if (block->addr == addr) {
                    prev->next = block->next;
                    if (block->next == nullptr) m_tail = prev;
                    break;
                }
                prev = block;
                block = block->next;
            }
        }

        if (block != nullptr) {
            m_used -= block->size;
            std::free(block);
        }
    }

    void FreeBlocks() noexcept {
        AllocedBlock* block = m_head;
        while (block != nullptr) {
            AllocedBlock* next = block->next;
            std::free(block);
            block = next;
        }
        m_head = nullptr;
        m_tail = nullptr;
        m_used = 0;
    }

private:
    AllocedBlock* m_head = nullptr;
    AllocedBlock* m_tail = nullptr;
    ptr_size m_used = 0; // in bytes
};

}

// FIXME: allow std allocator to be used without the allocation counting slowness. It should be just for debugging.

struct CORE_API_EXPORT StdAllocator {
    StdAllocator() noexcept;
    // no copy
    StdAllocator(const StdAllocator&) = delete;
    StdAllocator& operator=(const StdAllocator&) = delete;
    // no move
    StdAllocator(StdAllocator&&) = delete;
    StdAllocator& operator=(StdAllocator&&) = delete;
    ~StdAllocator() noexcept;

    void* Alloc(ptr_size size) noexcept;

    void Free(void*) noexcept;

    template<typename T, typename ...Args>
    T* Construct(T* out, Args... args) noexcept {
        void* p = Alloc(sizeof(T));
        if (p != nullptr) out = new (p) T(core::Forward<Args>(args)...);
        return out;
    }

    void Clear() noexcept;

    ptr_size UsedMem() noexcept;

    using OnOutOfMemoryFp = void(*)(void* userData);
    void OnOutOfMemory(OnOutOfMemoryFp cb) noexcept;

private:
    using AllocedBlockList = detail::AllocedBlockList;
    using AllocedBlock = detail::AllocedBlock;

    AllocedBlockList m_allocedBlocks;
    OnOutOfMemoryFp m_onOOM;
};

} // namespace core::alloc
