#include "../t-index.h"

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

    // tInfo.name = FN_NAME_TO_CPTR(arenaAllocatorBasicValidityTest);
    // if (runTest(tInfo, arenaAllocatorBasicValidityTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(onOomArenaAllocatorTest);
    if (runTest(tInfo, onOomArenaAllocatorTest) != 0) { ret = -1; }

    return ret;
}
