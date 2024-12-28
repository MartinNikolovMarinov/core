#include <core_alloc.h>
#include <core_assert.h>
#include <core_macros.h>
#include <core_mem.h>

#include <cstdlib>

namespace core {

namespace {

thread_local ArenaBlock* tl_blocks = nullptr;
thread_local addr_size tl_blockCount = 0;
thread_local addr_size tl_blockSize = 0;

inline void* _alloc(ArenaBlock** blocksPtr, addr_size* blockCountPtr, const addr_size blockSize,
                    const OOMHandlerFn oomHandler,
                    addr_size size, addr_size count) {

    auto& blocks = *blocksPtr;
    auto& blockCount = *blockCountPtr;

    addr_size effectiveSize = count * size;
    Panic(effectiveSize > 0, "Invalid allocation size");

    effectiveSize = core::align(effectiveSize);
    Panic(effectiveSize <= blockSize, "Allocation size exceeds block size");

    for (addr_size i = 0; i < blockCount; ++i) {
        void* blockEnd = core::ptrAdvance(blocks[i].begin, blockSize);
        addr_size freeSpace = core::ptrDiff(blockEnd, blocks[i].curr);
        if (freeSpace >= effectiveSize) {
            // Found a block with enough space
            void* ret = blocks[i].curr;
            blocks[i].curr = core::ptrAdvance(blocks[i].curr, effectiveSize);
            return ret;
        }
    }

    // No block with enough space. Allocate a new block.

    void* newBlockMemory = std::malloc(blockSize);
    if (newBlockMemory == nullptr) {
        if (oomHandler) {
            oomHandler();
        }
        return nullptr;
    }

    void* newBlocks = static_cast<ArenaBlock*>(std::realloc(blocks, (blockCount + 1) * sizeof(ArenaBlock)));
    if (newBlocks == nullptr) {
        std::free(newBlockMemory); // give up on the new block
        if (oomHandler) {
            oomHandler();
        }
        return nullptr;
    }

    blocks = static_cast<ArenaBlock*>(newBlocks);
    ArenaBlock& block = blocks[blockCount];
    block.begin = newBlockMemory;
    block.curr = block.begin;
    blockCount++;

    void* ret = block.curr;
    block.curr = core::ptrAdvance(block.curr, effectiveSize);
    return ret;
}

inline void* _calloc(ArenaBlock** blocks, addr_size* blockCount, const addr_size blockSize,
                     const OOMHandlerFn oomHandler,
                     addr_size size, addr_size count) {
    void* ret = _alloc(blocks, blockCount, blockSize, oomHandler, size, count);
    if (ret) {
        addr_size effectiveSize = size * count;
        effectiveSize = core::align(effectiveSize);
        core::memset(ret, 0, effectiveSize);
    }
    return ret;
}

inline void _clear(ArenaBlock* blocks, addr_size blockCount) {
    if (blocks == nullptr || blockCount == 0) {
        return;
    }

    for (addr_size i = 0; i < blockCount; ++i) {
        std::free(blocks[i].begin);
    }
    std::free(blocks);
}

constexpr inline addr_size _totalMemoryAllocated(addr_size blockCount, addr_size blockSize) {
    return blockCount * blockSize;
}

inline addr_size _inUseMemory(ArenaBlock* blocks, addr_size blockCount) {
    addr_size ret = 0;
    for (addr_size i = 0; i < blockCount; ++i) {
        ret += core::ptrDiff(blocks[i].curr, blocks[i].begin);
    }
    return ret;
}

inline void _reset(ArenaBlock* blocks, addr_size blockCount) {
    for (addr_size i = 0; i < blockCount; ++i) {
        blocks[i].curr = blocks[i].begin;
    }
}

} // namespace

// Std Arena Allocator

StdArenaAllocator::StdArenaAllocator(addr_size blockSize)
    : oomHandler(getDefaultOOMHandler())
    , m_blocks(nullptr)
    , m_blockCount(0)
    , m_blockSize(blockSize) {}

StdArenaAllocator::StdArenaAllocator(StdArenaAllocator&& other) {
    m_blocks = other.m_blocks;
    m_blockCount = other.m_blockCount;
    m_blockSize = other.m_blockSize;
    oomHandler = other.oomHandler;

    other.m_blocks = nullptr;
    other.m_blockCount = 0;
    other.m_blockSize = 0;
    other.oomHandler = nullptr;
}

void StdArenaAllocator::setBlockSize(addr_size blockSize) {
    clear();
    m_blockSize = blockSize;
}

void* StdArenaAllocator::alloc(addr_size count, addr_size size) {
    return _alloc(&m_blocks, &m_blockCount, m_blockSize, oomHandler, size, count);
}

void* StdArenaAllocator::calloc(addr_size count, addr_size size) {
    return _calloc(&m_blocks, &m_blockCount, m_blockSize, oomHandler, size, count);
}

void StdArenaAllocator::free(void*, addr_size, addr_size) {}

void StdArenaAllocator::clear() {
    _clear(m_blocks, m_blockCount);
    m_blocks = nullptr;
    m_blockCount = 0;
}

addr_size StdArenaAllocator::totalMemoryAllocated() {
    return _totalMemoryAllocated(m_blockCount, m_blockSize);
}

addr_size StdArenaAllocator::inUseMemory() {
    return _inUseMemory(m_blocks, m_blockCount);
}

void StdArenaAllocator::reset() {
    _reset(m_blocks, m_blockCount);
}

// Thread Local Std Arena Allocator

ThreadLocalStdArenaAllocator::ThreadLocalStdArenaAllocator() : oomHandler(getDefaultOOMHandler()) {}

ThreadLocalStdArenaAllocator ThreadLocalStdArenaAllocator::create(addr_size blockSize) {
    Panic(tl_blockSize == 0, "ThreadLocalStdArenaAllocator::create() called twice in the same thread");
    Panic(blockSize > 0, "ThreadLocalStdArenaAllocator::create() Invalid block size");
    tl_blockSize = blockSize;
    tl_blockCount = 0;
    tl_blocks = nullptr;
    return ThreadLocalStdArenaAllocator{};
}

void ThreadLocalStdArenaAllocator::setBlockSize(addr_size blockSize) {
    clear();
    tl_blockSize = blockSize;
}

void* ThreadLocalStdArenaAllocator::alloc(addr_size count, addr_size size) {
    return _alloc(&tl_blocks, &tl_blockCount, tl_blockSize, oomHandler, size, count);
}

void* ThreadLocalStdArenaAllocator::calloc(addr_size count, addr_size size) {
    return _calloc(&tl_blocks, &tl_blockCount, tl_blockSize, oomHandler, size, count);
}

void ThreadLocalStdArenaAllocator::free(void*, addr_size, addr_size) {}

void ThreadLocalStdArenaAllocator::clear() {
    _clear(tl_blocks, tl_blockCount);
    tl_blocks = nullptr;
    tl_blockCount = 0;
}

addr_size ThreadLocalStdArenaAllocator::totalMemoryAllocated() {
    return _totalMemoryAllocated(tl_blockCount, tl_blockSize);
}

addr_size ThreadLocalStdArenaAllocator::inUseMemory() {
    return _inUseMemory(tl_blocks, tl_blockCount);
}

void ThreadLocalStdArenaAllocator::reset() {
    _reset(tl_blocks, tl_blockCount);
}

} // namespace core
