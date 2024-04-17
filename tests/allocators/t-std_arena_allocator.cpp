#include "../t-index.h"

i32 arenaAllocatorBasicValidityTest() {
    constexpr addr_size REGION_SIZE = 256;
    core::StdArenaAllocator allocator (REGION_SIZE);
    defer { allocator.clear(); };

    {
        void* data;

        data = allocator.alloc(REGION_SIZE, sizeof(u8));
        CT_CHECK(data != nullptr);
        CT_CHECK(allocator.inUseMemory() == REGION_SIZE);
        CT_CHECK(allocator.totalMemoryAllocated() == REGION_SIZE);

        data = allocator.alloc(8, sizeof(u8));
        CT_CHECK(data != nullptr);
        CT_CHECK(allocator.inUseMemory() == REGION_SIZE + 8);
        CT_CHECK(allocator.totalMemoryAllocated() == REGION_SIZE * 2);

        data = allocator.alloc(64, sizeof(u8));
        CT_CHECK(data != nullptr);
        CT_CHECK(allocator.inUseMemory() == REGION_SIZE + 8 + 64);
        CT_CHECK(allocator.totalMemoryAllocated() == REGION_SIZE * 2);

        data = allocator.alloc(200, sizeof(u8));
        CT_CHECK(data != nullptr);
        CT_CHECK(allocator.inUseMemory() == REGION_SIZE + 8 + 64 + 200);
        CT_CHECK(allocator.totalMemoryAllocated() == REGION_SIZE * 3);

        // Free space in buckets at this point:
        // 0, 184, 56

        data = allocator.alloc(176, sizeof(u8));
        CT_CHECK(data != nullptr);
        CT_CHECK(allocator.inUseMemory() == REGION_SIZE + 8 + 64 + 176 + 200);
        CT_CHECK(allocator.totalMemoryAllocated() == REGION_SIZE * 3);

        // Free space in buckets at this point:
        // 0, 8, 56

        data = allocator.alloc(3, sizeof(u8));
        CT_CHECK(data != nullptr);
        CT_CHECK(allocator.inUseMemory() == REGION_SIZE * 2 + 200);
        CT_CHECK(allocator.totalMemoryAllocated() == REGION_SIZE * 3);

        // Free space in buckets at this point:
        // 0, 0, 56

        data = allocator.alloc(56, sizeof(u8));
        CT_CHECK(data != nullptr);
        CT_CHECK(allocator.inUseMemory() == REGION_SIZE * 3);
        CT_CHECK(allocator.totalMemoryAllocated() == REGION_SIZE * 3);
    }

    allocator.reset();

    {
        void* data;

        data = allocator.alloc(1, sizeof(u8));
        CT_CHECK(data != nullptr);
        CT_CHECK(allocator.inUseMemory() == 8);
        CT_CHECK(allocator.totalMemoryAllocated() == REGION_SIZE * 3);

        data = allocator.alloc(REGION_SIZE, sizeof(u8));
        CT_CHECK(data != nullptr);
        CT_CHECK(allocator.inUseMemory() == REGION_SIZE + 8);
        CT_CHECK(allocator.totalMemoryAllocated() == REGION_SIZE * 3);
    }

    return 0;
}

i32 onOomArenaAllocatorTest() {
    static i32 testOOMCount = 0;
    constexpr addr_size REGION_SIZE = 0x7ffffffffffffffd;
    core::StdArenaAllocator allocator (REGION_SIZE);
    defer { allocator.clear(); };
    void* data;

    CT_CHECK(allocator.oomHandler == core::defaultOOMHandler);

    allocator.oomHandler = []() { testOOMCount++; };

    // Will fail to allocate the region
    data = allocator.alloc(1, sizeof(u32));
    CT_CHECK(data == nullptr);
    CT_CHECK(testOOMCount == 1);

    return 0;
}

i32 runArenaAllocatorTestsSuite() {
    using namespace core::testing;

    i32 ret = 0;
    TestInfo tInfo;
    tInfo.trackMemory = false;

    tInfo.name = FN_NAME_TO_CPTR(arenaAllocatorBasicValidityTest);
    if (runTest(tInfo, arenaAllocatorBasicValidityTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(onOomArenaAllocatorTest);
    if (runTest(tInfo, onOomArenaAllocatorTest) != 0) { ret = -1; }

    return ret;
}
