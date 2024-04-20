#include "../t-index.h"

i32 stdStatsAllocatorBasicValidityTest() {
    core::StdStatsAllocator allocator;

    void* data1 = allocator.alloc(4, sizeof(u8));
    CT_CHECK(data1 != nullptr);
    CT_CHECK(allocator.inUseMemory() == 8);
    defer { allocator.free(data1, 4, sizeof(u8)); };

    void* data2 = allocator.alloc(9, sizeof(u8));
    CT_CHECK(data2 != nullptr);
    CT_CHECK(allocator.inUseMemory() == 24);
    defer { allocator.free(data2, 9, sizeof(u8)); };

    void* data3 = allocator.calloc(1, sizeof(u8));
    CT_CHECK(data3 != nullptr);
    CT_CHECK(allocator.inUseMemory() == 32);
    defer { allocator.free(data3, 1, sizeof(u8)); };

    void* buff[10];

    for (addr_size i = 0; i < 8; i++) {
        buff[i] = allocator.alloc(i + 1, sizeof(u8));
        CT_CHECK(buff[i] != nullptr);
    }
    CT_CHECK(allocator.inUseMemory() == 96);

    buff[8] = allocator.alloc(9, sizeof(u8));
    CT_CHECK(allocator.inUseMemory() == 112);
    buff[9] = allocator.calloc(10, sizeof(u8));
    CT_CHECK(allocator.inUseMemory() == 128);

    for (addr_size i = 0; i < 10; i++) {
        allocator.free(buff[i], i + 1, sizeof(u8));
    }

    CT_CHECK(allocator.inUseMemory() == 32);
    CT_CHECK(allocator.totalMemoryAllocated() == 128);

    return 0;
}

i32 stdStatsAllocatorMoveTest() {
    core::StdStatsAllocator allocator;

    allocator.alloc(4, sizeof(u8));

    core::StdStatsAllocator allocator2 = std::move(allocator);

    CT_CHECK(allocator.inUseMemory() == 0);
    CT_CHECK(allocator.totalMemoryAllocated() == 0);

    CT_CHECK(allocator2.inUseMemory() == 8);
    CT_CHECK(allocator2.totalMemoryAllocated() == 8);

    return 0;
}


i32 onOOMStdStatsAllocatorTest() {
    static i32 testOOMCount = 0;
    void* data;
    core::StdStatsAllocator allocator;

    CT_CHECK(allocator.oomHandler == core::defaultOOMHandler);

    allocator.oomHandler = []() { testOOMCount++; };

    data = allocator.alloc(0x7ffffffffffffffd, sizeof(u32));
    CT_CHECK(data == nullptr);
    CT_CHECK(testOOMCount == 1);

    data = allocator.calloc(0x7ffffffffffffffd, sizeof(u32));
    CT_CHECK(data == nullptr);
    CT_CHECK(testOOMCount == 2);

    allocator.oomHandler = nullptr;
    data = allocator.alloc(0x7ffffffffffffffd, sizeof(u32));
    CT_CHECK(data == nullptr, "Setting the oomHandler to null is not properly handled.");

    return 0;
}

i32 runStdStatsAllocatorTestsSuite() {
    using namespace core::testing;

    i32 ret = 0;

    TestInfo tInfo;
    tInfo.trackMemory = false;

    tInfo.name = FN_NAME_TO_CPTR(stdStatsAllocatorBasicValidityTest);
    if (runTest(tInfo, stdStatsAllocatorBasicValidityTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(stdStatsAllocatorMoveTest);
    if (runTest(tInfo, stdStatsAllocatorMoveTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(onOOMStdStatsAllocatorTest);
    if (runTest(tInfo, onOOMStdStatsAllocatorTest) != 0) { ret = -1; }

    return ret;
}
