#include <core_alloc.h>
#include <core_mem.h>

#include <cstdlib>

namespace core {

StdArenaAllocator::StdArenaAllocator(addr_size blockSize)
    : oomHandler(defaultOOMHandler), m_blocks(nullptr), m_blockCount(0), m_blockSize(blockSize) {}
StdArenaAllocator::StdArenaAllocator(StdArenaAllocator&& other) : m_blockSize(other.m_blockSize) {
    m_blocks = other.m_blocks;
    m_blockCount = other.m_blockCount;
    oomHandler = other.oomHandler;

    other.m_blocks = nullptr;
    other.m_blockCount = 0;
}

void* StdArenaAllocator::alloc(addr_size count, addr_size size) {
    addr_size effectiveSize = count * size;
    Panic(effectiveSize > 0, "Invalid allocation size");

    effectiveSize = core::align(effectiveSize);
    Panic(effectiveSize <= m_blockSize, "Allocation size exceeds block size");

    for (addr_size i = 0; i < m_blockCount; ++i) {
        void* blockEnd = core::ptrAdvance(m_blocks[i].begin, m_blockSize);
        addr_size freeSpace = core::ptrDiff(blockEnd, m_blocks[i].curr);
        if (freeSpace >= effectiveSize) {
            // Found a block with enough space
            void* ret = m_blocks[i].curr;
            m_blocks[i].curr = core::ptrAdvance(m_blocks[i].curr, effectiveSize);
            return ret;
        }
    }

    // No block with enough space. Allocate a new block.

    void* newBlockMemory = std::malloc(m_blockSize);
    if (newBlockMemory == nullptr) {
        if (oomHandler) {
            oomHandler();
        }
        return nullptr;
    }

    void* newBlocks = static_cast<Block*>(std::realloc(m_blocks, (m_blockCount + 1) * sizeof(Block)));
    if (newBlocks == nullptr) {
        std::free(newBlockMemory); // give up on the new block
        if (oomHandler) {
            oomHandler();
        }
        return nullptr;
    }

    m_blocks = static_cast<Block*>(newBlocks);
    Block& block = m_blocks[m_blockCount];
    block.begin = newBlockMemory;
    block.curr = block.begin;
    m_blockCount++;

    void* ret = block.curr;
    block.curr = core::ptrAdvance(block.curr, effectiveSize);
    return ret;
}

void* StdArenaAllocator::calloc(addr_size count, addr_size size) {
    void* ret = alloc(count, size);
    if (ret != nullptr) {
        core::memset(ret, 0, count * size);
    }
    return ret;
}

void StdArenaAllocator::free(void*, addr_size, addr_size) {}

void StdArenaAllocator::clear() {
    for (addr_size i = 0; i < m_blockCount; ++i) {
        if (m_blocks[i].begin) {
            std::free(m_blocks[i].begin);
        }
    }
    std::free(m_blocks);
    m_blocks = nullptr;
    m_blockCount = 0;
}

addr_size StdArenaAllocator::totalMemoryAllocated() {
    return m_blockCount * m_blockSize;
}

addr_size StdArenaAllocator::inUseMemory() {
    addr_size ret = 0;
    for (addr_size i = 0; i < m_blockCount; ++i) {
        ret += core::ptrDiff(m_blocks[i].curr, m_blocks[i].begin);
    }
    return ret;
}

void StdArenaAllocator::reset() {
    for (addr_size i = 0; i < m_blockCount; ++i) {
        m_blocks[i].curr = m_blocks[i].begin;
    }
}

} // namespace core
