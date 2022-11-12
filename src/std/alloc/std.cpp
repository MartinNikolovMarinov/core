#include <std/alloc/std.h>

namespace core::alloc
{

StdAllocator::StdAllocator() noexcept : m_onOOM(nullptr) {}

StdAllocator::~StdAllocator() noexcept {
    Clear();
}

void* StdAllocator::Alloc(ptr_size size) noexcept {
    if (size == 0) return nullptr;
    void* addr = std::malloc(size);
    if (addr == nullptr) {
        if (m_onOOM != nullptr) m_onOOM(nullptr);
        return nullptr;
    }

    auto block = static_cast<AllocedBlock*>(std::malloc(sizeof(AllocedBlock)));
    if (block == nullptr) {
        std::free(addr); // give up on this memory
        if (m_onOOM != nullptr) m_onOOM(nullptr);
        return nullptr;
    }

    block->addr = addr;
    block->size = size;
    m_allocedBlocks.AddBlock(block);
    return addr;
}

void StdAllocator::Free(void* addr) noexcept {
    if (addr == nullptr) return;
    std::free(addr);
    m_allocedBlocks.RemoveBlock(addr);
}

void StdAllocator::Clear() noexcept {
    // free the memory at the addresses stored in the blocks.
    AllocedBlock* block = m_allocedBlocks.Head();
    while (block != nullptr) {
        AllocedBlock* next = block->next;
        std::free(block->addr);
        block = next;
    }
    m_allocedBlocks.FreeBlocks();
}

ptr_size StdAllocator::UsedMem() noexcept { return m_allocedBlocks.Used(); }
void StdAllocator::OnOutOfMemory(OnOutOfMemoryFp cb) noexcept { m_onOOM = cb; }

} // namespace core::alloc
