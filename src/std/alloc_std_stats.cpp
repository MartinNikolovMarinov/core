#include <std/alloc_std_stats.h>

#include <cstdlib>

namespace core {

namespace details {

struct alloced_block {
    void* addr;
    alloced_block* next;
    ptr_size size;
};

struct allocated_block_list {

    alloced_block* head() noexcept { return m_head; }
    alloced_block* tail() noexcept { return m_tail; }
    ptr_size used() noexcept { return m_used; }

    void add_block(alloced_block* block) noexcept {
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

    void remove_block(void* addr) noexcept {
        if (m_head == nullptr) return;

        alloced_block* block = nullptr;
        if (m_head->addr == addr) {
            block = m_head;
            m_head = m_head->next;
            if (m_head == nullptr) m_tail = nullptr;
        } else {
            alloced_block* prev = m_head;
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

    void free_blocks() noexcept {
        alloced_block* block = m_head;
        while (block != nullptr) {
            alloced_block* next = block->next;
            std::free(block);
            block = next;
        }
        m_head = nullptr;
        m_tail = nullptr;
        m_used = 0;
    }

private:
    alloced_block* m_head = nullptr;
    alloced_block* m_tail = nullptr;
    ptr_size m_used = 0; // in bytes
};

}

std_stats_allocator::std_stats_allocator(on_oom_fp cb)
    : m_oomCb(cb)
    , m_allocatedBlocks(new details::allocated_block_list()) {}

std_stats_allocator::~std_stats_allocator() {
    clear();
    if (m_allocatedBlocks) {
        delete m_allocatedBlocks;
    }
}

void* std_stats_allocator::alloc(ptr_size size) noexcept {
    using details::alloced_block;

    if (size == 0) return nullptr;
    void* addr = std::malloc(size);
    if (addr == nullptr) {
        if (m_oomCb != nullptr) m_oomCb(nullptr);
        return nullptr;
    }

    auto block = static_cast<alloced_block*>(std::malloc(sizeof(alloced_block)));
    if (block == nullptr) {
        // not enough memory to keep track of this allocation.
        std::free(addr); // give up on this memory
        if (m_oomCb != nullptr) m_oomCb(nullptr);
        return nullptr;
    }

    block->addr = addr;
    block->size = size;
    m_allocatedBlocks->add_block(block);
    return addr;
}

void std_stats_allocator::free(void* ptr) noexcept {
    if (ptr == nullptr) return;
    std::free(ptr);
    m_allocatedBlocks->remove_block(ptr);
}

void std_stats_allocator::clear() noexcept {
    // free the memory at the addresses stored in the blocks.
    details::alloced_block* block = m_allocatedBlocks->head();
    while (block != nullptr) {
        details::alloced_block* next = block->next;
        std::free(block->addr);
        block = next;
    }
    m_allocatedBlocks->free_blocks();
}

ptr_size std_stats_allocator::used_mem() noexcept {
    return m_allocatedBlocks->used();
}

}
