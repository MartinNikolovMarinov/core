#include "../t-index.h"

i32 bumpAllocatorBasicValidityTest() {
    constexpr addr_size BUFF_SIZE = 128;
    void* buff[BUFF_SIZE];
    core::BumpAllocator allocator (buff, BUFF_SIZE);

    {
        void* data = allocator.alloc(4, sizeof(u8));
        CT_CHECK(data != nullptr);
        CT_CHECK(allocator.inUseMemory() == 8);
    }

    {
        void* data = allocator.alloc(9, sizeof(u8));
        CT_CHECK(data != nullptr);
        CT_CHECK(allocator.inUseMemory() == 24);
    }

    {
        void* data = allocator.alloc(1, sizeof(u8));
        CT_CHECK(data != nullptr);
        CT_CHECK(allocator.inUseMemory() == 32);
    }

    {
        void* ptrBuff[10];

        for (addr_size i = 0; i < 8; i++) {
            ptrBuff[i] = allocator.alloc(i + 1, sizeof(u8));
            CT_CHECK(ptrBuff[i] != nullptr);
        }
        CT_CHECK(allocator.inUseMemory() == 96);

        ptrBuff[8] = allocator.alloc(9, sizeof(u8));
        CT_CHECK(allocator.inUseMemory() == 112);
        ptrBuff[9] = allocator.alloc(10, sizeof(u8));
        CT_CHECK(allocator.inUseMemory() == 128);

        for (addr_size i = 0; i < 10; i++) {
            allocator.free(ptrBuff[i], i + 1, sizeof(u8));
        }
    }

    CT_CHECK(allocator.inUseMemory() == 128);
    CT_CHECK(allocator.totalMemoryAllocated() == 128);

    {
        allocator.oomHandler = nullptr;
        void* data = allocator.alloc(1, 1);
        CT_CHECK(data == nullptr); // should be out of memory at this point.
    }

    return 0;
}

i32 onOomBumpAllocatorTest() {
    static i32 testOOMCount = 0;
    void* data;
    constexpr addr_size BUFF_SIZE = 256;
    void* buff[BUFF_SIZE];
    core::BumpAllocator allocator (buff, BUFF_SIZE);

    CT_CHECK(allocator.oomHandler == core::defaultOOMHandler);

    allocator.oomHandler = []() { testOOMCount++; };

    data = allocator.alloc(0x7ffffffffffffffd, sizeof(u32));
    CT_CHECK(data == nullptr);
    CT_CHECK(testOOMCount == 1);

    data = allocator.calloc(257, sizeof(u8));
    CT_CHECK(data == nullptr);
    CT_CHECK(testOOMCount == 2);

    allocator.oomHandler = nullptr;
    data = allocator.alloc(0x7ffffffffffffffd, sizeof(u32));
    CT_CHECK(data == nullptr, "Setting the oomHandler to null is not properly handled.");

    return 0;
}

i32 runBumpAllocatorTestsSuite() {
    using namespace core::testing;

    i32 ret = 0;

    TestInfo tInfo;
    tInfo.trackMemory = false;

    tInfo.name = FN_NAME_TO_CPTR(bumpAllocatorBasicValidityTest);
    if (runTest(tInfo, bumpAllocatorBasicValidityTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(onOomBumpAllocatorTest);
    if (runTest(tInfo, onOomBumpAllocatorTest) != 0) { ret = -1; }

    return ret;
}
